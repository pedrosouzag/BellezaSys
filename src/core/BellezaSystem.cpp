#include "bellezasys/core/BellezaSystemImpl.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace bellezasys {

// ============================= REGISTRO =============================

std::vector<BellezaSystem*> BellezaSystem::models;

void BellezaSystem::addModel(BellezaSystem* model)
{
    models.push_back(model);
}

BellezaSystem* BellezaSystem::createModel()
{
    BellezaSystem* model = new BellezaSystemHandle();
    BellezaSystem::addModel(model);
    return model;
}

void BellezaSystem::deleteModel(BellezaSystem* model)
{
    if (model == nullptr) {
        return;
    }

    // remove o ponteiro do registro global antes de destruir o objeto
    for (auto it = models.begin(); it != models.end(); ++it) {
        if (*it == model) {
            models.erase(it);
            break;
        }
    }

    delete model;
}

// ============================= BODY =============================

void BellezaSystemBody::add(Usuario* usuario)
{
    usuarios.push_back(usuario);
}

void BellezaSystemBody::add(Servico* servico)
{
    servicos.push_back(servico);
}

void BellezaSystemBody::add(Profissional* profissional)
{
    profissionais.push_back(profissional);
}

void BellezaSystemBody::add(Agendamento* agendamento)
{
    agendamentos.push_back(agendamento);
}

Usuario* BellezaSystemBody::usuarioObrigatorio(const std::string& id) const
{
    for (Usuario* usuario : usuarios) {
        if (usuario->id() == id) {
            return usuario;
        }
    }
    throw std::invalid_argument("Usuario nao encontrado: " + id);
}

Servico* BellezaSystemBody::servicoObrigatorio(const std::string& id) const
{
    for (Servico* servico : servicos) {
        if (servico->id() == id) {
            return servico;
        }
    }
    throw std::invalid_argument("Servico nao encontrado: " + id);
}

Profissional* BellezaSystemBody::profissionalObrigatorio(const std::string& id) const
{
    for (Profissional* profissional : profissionais) {
        if (profissional->id() == id) {
            return profissional;
        }
    }
    throw std::invalid_argument("Profissional nao encontrado: " + id);
}

Agendamento* BellezaSystemBody::agendamentoObrigatorio(const std::string& id) const
{
    for (Agendamento* agendamento : agendamentos) {
        if (agendamento->id() == id) {
            return agendamento;
        }
    }
    throw std::invalid_argument("Agendamento nao encontrado: " + id);
}

// um profissional esta disponivel se: atende o servico, o horario cabe
// dentro do expediente dele, e nenhum outro agendamento ativo dele
// colide com o horario pedido (ignorarAgendamentoId serve pra nao
// comparar o proprio agendamento contra ele mesmo, no caso de remarcacao)
bool BellezaSystemBody::profissionalDisponivel(const std::string& profissionalId, const std::string& servicoId, DateTime inicio, const std::string& ignorarAgendamentoId) const
{
    Profissional* profissional = profissionalObrigatorio(profissionalId);
    Servico* servico = servicoObrigatorio(servicoId);

    if (!profissional->atendeServico(servicoId)) {
        return false;
    }
    if (!profissional->estaNoExpediente(inicio, servico->duracao())) {
        return false;
    }

    for (Agendamento* existente : agendamentos) {
        if (!existente->estaAtivo() || existente->id() == ignorarAgendamentoId || existente->profissionalId() != profissionalId) {
            continue;
        }
        if (intervalosColidem(inicio, servico->duracao(), existente->inicio(), existente->duracao())) {
            return false;
        }
    }

    return true;
}

// deleta todas as entidades que o sistema possui (o body e o unico dono
// desses ponteiros)
BellezaSystemBody::~BellezaSystemBody()
{
    for (Usuario* usuario : usuarios) delete usuario;
    for (Servico* servico : servicos) delete servico;
    for (Profissional* profissional : profissionais) delete profissional;
    for (Agendamento* agendamento : agendamentos) delete agendamento;

    usuarios.clear();
    servicos.clear();
    profissionais.clear();
    agendamentos.clear();
}

// ============================= HANDLE =============================

BellezaSystemHandle::BellezaSystemHandle() = default;

void BellezaSystemHandle::add(Usuario* usuario) { pImpl_->add(usuario); }
void BellezaSystemHandle::add(Servico* servico) { pImpl_->add(servico); }
void BellezaSystemHandle::add(Profissional* profissional) { pImpl_->add(profissional); }
void BellezaSystemHandle::add(Agendamento* agendamento) { pImpl_->add(agendamento); }

Usuario* BellezaSystemHandle::cadastrarUsuario(std::string id, std::string nome, std::string email, std::string senha, Papel papel)
{
    for (Usuario* usuario : pImpl_->usuarios) {
        if (usuario->id() == id) {
            throw std::invalid_argument("Usuario ja cadastrado: " + id);
        }
        // email tambem precisa ser unico entre todos os usuarios
        if (usuario->email() == email) {
            throw std::invalid_argument("Email ja cadastrado: " + email);
        }
    }

    Usuario* usuario = new UsuarioHandle(std::move(id), std::move(nome), std::move(email), std::move(senha), papel);
    add(usuario);
    return usuario;
}

// procura o usuario pelo email e confere a senha
bool BellezaSystemHandle::login(const std::string& email, const std::string& senha) const
{
    for (Usuario* usuario : pImpl_->usuarios) {
        if (usuario->email() == email) {
            return usuario->autenticar(senha);
        }
    }
    return false;
}

Servico* BellezaSystemHandle::cadastrarServico(std::string id, std::string nome, double preco, std::chrono::minutes duracao, double percentualComissao)
{
    for (Servico* servico : pImpl_->servicos) {
        if (servico->id() == id) {
            throw std::invalid_argument("Servico ja cadastrado: " + id);
        }
    }

    Servico* servico = new ServicoHandle(std::move(id), std::move(nome), preco, duracao, percentualComissao);
    add(servico);
    return servico;
}

Profissional* BellezaSystemHandle::cadastrarProfissional(std::string id, std::string nome, std::string email, std::vector<std::string> servicosAtendidos, int expedienteInicioHora, int expedienteFimHora)
{
    for (Profissional* profissional : pImpl_->profissionais) {
        if (profissional->id() == id) {
            throw std::invalid_argument("Profissional ja cadastrado: " + id);
        }
    }
    // cada servico atendido precisa existir de fato no sistema
    for (const auto& servicoId : servicosAtendidos) {
        pImpl_->servicoObrigatorio(servicoId);
    }

    Profissional* profissional = new ProfissionalHandle(std::move(id), std::move(nome), std::move(email), std::move(servicosAtendidos), expedienteInicioHora, expedienteFimHora);
    add(profissional);
    return profissional;
}

Agendamento* BellezaSystemHandle::agendar(const std::string& clienteId, const std::string& profissionalId, const std::string& servicoId, DateTime inicio)
{
    Usuario* cliente = pImpl_->usuarioObrigatorio(clienteId);
    Servico* servico = pImpl_->servicoObrigatorio(servicoId);
    pImpl_->profissionalObrigatorio(profissionalId);

    // so cliente pode ser dono de um agendamento (funcionario/admin nao)
    if (cliente->papel() != Papel::Cliente) {
        throw std::invalid_argument("Somente clientes podem receber agendamentos.");
    }
    if (!pImpl_->profissionalDisponivel(profissionalId, servicoId, inicio)) {
        throw std::logic_error("Profissional indisponivel para esse horario.");
    }

    // gera um id sequencial simples (AG-1, AG-2, ...)
    const std::string id = "AG-" + std::to_string(pImpl_->proximoAgendamento++);
    Agendamento* agendamento = new AgendamentoHandle(id, clienteId, profissionalId, servicoId, inicio, servico->duracao(), servico->preco());
    add(agendamento);
    return agendamento;
}

Agendamento* BellezaSystemHandle::remarcarAgendamento(const std::string& agendamentoId, DateTime novoInicio)
{
    Agendamento* agendamento = pImpl_->agendamentoObrigatorio(agendamentoId);
    pImpl_->servicoObrigatorio(agendamento->servicoId());
    pImpl_->profissionalObrigatorio(agendamento->profissionalId());

    // confere disponibilidade no novo horario, ignorando o proprio agendamento na checagem de conflito
    if (!pImpl_->profissionalDisponivel(agendamento->profissionalId(), agendamento->servicoId(), novoInicio, agendamento->id())) {
        throw std::logic_error("Profissional indisponivel para o novo horario.");
    }
    agendamento->remarcar(novoInicio);
    return agendamento;
}

Agendamento* BellezaSystemHandle::cancelarAgendamento(const std::string& agendamentoId)
{
    Agendamento* agendamento = pImpl_->agendamentoObrigatorio(agendamentoId);
    agendamento->cancelar();
    return agendamento;
}

Agendamento* BellezaSystemHandle::concluirAgendamento(const std::string& agendamentoId)
{
    Agendamento* agendamento = pImpl_->agendamentoObrigatorio(agendamentoId);
    agendamento->concluir();
    // ao concluir, registra o pagamento no financeiro (caixa + comissao)
    Servico* servico = pImpl_->servicoObrigatorio(agendamento->servicoId());
    pImpl_->financeiro.registrarPagamento(*agendamento, *servico);
    return agendamento;
}

// recalcula a lista de profissionais disponiveis e guarda no cache do
// body; o iterator retornado (e o de profissionaisDisponiveisEnd())
// aponta pra esse cache
std::vector<Profissional*>::iterator BellezaSystemHandle::profissionaisDisponiveisBegin(const std::string& servicoId, DateTime inicio)
{
    pImpl_->servicoObrigatorio(servicoId);

    pImpl_->profissionaisDisponiveisCache.clear();
    for (Profissional* profissional : pImpl_->profissionais) {
        if (pImpl_->profissionalDisponivel(profissional->id(), servicoId, inicio)) {
            pImpl_->profissionaisDisponiveisCache.push_back(profissional);
        }
    }

    return pImpl_->profissionaisDisponiveisCache.begin();
}

std::vector<Profissional*>::iterator BellezaSystemHandle::profissionaisDisponiveisEnd()
{
    return pImpl_->profissionaisDisponiveisCache.end();
}

std::vector<Agendamento*>::iterator BellezaSystemHandle::agendamentosDoProfissionalBegin(const std::string& profissionalId)
{
    pImpl_->profissionalObrigatorio(profissionalId);

    pImpl_->agendamentosDoProfissionalCache.clear();
    for (Agendamento* agendamento : pImpl_->agendamentos) {
        if (agendamento->profissionalId() == profissionalId) {
            pImpl_->agendamentosDoProfissionalCache.push_back(agendamento);
        }
    }

    return pImpl_->agendamentosDoProfissionalCache.begin();
}

std::vector<Agendamento*>::iterator BellezaSystemHandle::agendamentosDoProfissionalEnd()
{
    return pImpl_->agendamentosDoProfissionalCache.end();
}

std::vector<Agendamento*>::iterator BellezaSystemHandle::agendamentosDoClienteBegin(const std::string& clienteId)
{
    pImpl_->usuarioObrigatorio(clienteId);

    pImpl_->agendamentosDoClienteCache.clear();
    for (Agendamento* agendamento : pImpl_->agendamentos) {
        if (agendamento->clienteId() == clienteId) {
            pImpl_->agendamentosDoClienteCache.push_back(agendamento);
        }
    }

    return pImpl_->agendamentosDoClienteCache.begin();
}

std::vector<Agendamento*>::iterator BellezaSystemHandle::agendamentosDoClienteEnd()
{
    return pImpl_->agendamentosDoClienteCache.end();
}

std::vector<Usuario*>::iterator BellezaSystemHandle::usuariosBegin() { return pImpl_->usuarios.begin(); }
std::vector<Usuario*>::iterator BellezaSystemHandle::usuariosEnd() { return pImpl_->usuarios.end(); }

std::vector<Servico*>::iterator BellezaSystemHandle::servicosBegin() { return pImpl_->servicos.begin(); }
std::vector<Servico*>::iterator BellezaSystemHandle::servicosEnd() { return pImpl_->servicos.end(); }

std::vector<Profissional*>::iterator BellezaSystemHandle::profissionaisBegin() { return pImpl_->profissionais.begin(); }
std::vector<Profissional*>::iterator BellezaSystemHandle::profissionaisEnd() { return pImpl_->profissionais.end(); }

std::vector<Agendamento*>::iterator BellezaSystemHandle::agendamentosBegin() { return pImpl_->agendamentos.begin(); }
std::vector<Agendamento*>::iterator BellezaSystemHandle::agendamentosEnd() { return pImpl_->agendamentos.end(); }

const Financeiro& BellezaSystemHandle::financeiro() const
{
    return pImpl_->financeiro;
}

} // namespace bellezasys
