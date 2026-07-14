#include "bellezasys/core/ProfissionalImpl.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace bellezasys {

// guarda os dados e valida logo em seguida
ProfissionalBody::ProfissionalBody(std::string id, std::string nome, std::string email, std::vector<std::string> servicosAtendidos, int expedienteInicioHora, int expedienteFimHora)
    : id(std::move(id)), nome(std::move(nome)), email(std::move(email)), servicosAtendidos(std::move(servicosAtendidos)), expedienteInicioHora(expedienteInicioHora), expedienteFimHora(expedienteFimHora)
{
    validar();
}

void ProfissionalBody::validar() const
{
    if (id.empty() || nome.empty() || email.empty()) {
        throw std::invalid_argument("Profissional precisa de id, nome e email.");
    }
    // o expediente precisa ser um intervalo de horas valido dentro do dia, com inicio antes do fim
    if (expedienteInicioHora < 0 || expedienteInicioHora > 23 || expedienteFimHora < 1 || expedienteFimHora > 24 || expedienteInicioHora >= expedienteFimHora) {
        throw std::invalid_argument("Expediente invalido.");
    }
}

ProfissionalHandle::ProfissionalHandle() = default;

ProfissionalHandle::ProfissionalHandle(std::string id, std::string nome, std::string email, std::vector<std::string> servicosAtendidos, int expedienteInicioHora, int expedienteFimHora)
    : Handle<ProfissionalBody>(new ProfissionalBody(std::move(id), std::move(nome), std::move(email), std::move(servicosAtendidos), expedienteInicioHora, expedienteFimHora))
{
}

bool ProfissionalHandle::isValid() const { return !pImpl_->id.empty(); }
const std::string& ProfissionalHandle::id() const { return pImpl_->id; }
const std::string& ProfissionalHandle::nome() const { return pImpl_->nome; }
const std::string& ProfissionalHandle::email() const { return pImpl_->email; }
const std::vector<std::string>& ProfissionalHandle::servicosAtendidos() const { return pImpl_->servicosAtendidos; }
int ProfissionalHandle::expedienteInicioHora() const { return pImpl_->expedienteInicioHora; }
int ProfissionalHandle::expedienteFimHora() const { return pImpl_->expedienteFimHora; }

// procura o servico na lista de servicos atendidos
bool ProfissionalHandle::atendeServico(const std::string& servicoId) const
{
    return std::find(pImpl_->servicosAtendidos.begin(), pImpl_->servicosAtendidos.end(), servicoId) != pImpl_->servicosAtendidos.end();
}

// converte tudo para minutos desde a meia-noite e compara com o
// expediente cadastrado (inicio e fim em horas cheias)
bool ProfissionalHandle::estaNoExpediente(DateTime inicio, std::chrono::minutes duracao) const
{
    const DateTime fim = inicio + duracao;
    const int inicioMinutos = localHour(inicio) * 60 + localMinute(inicio);
    const int fimMinutos = localHour(fim) * 60 + localMinute(fim);
    const int abertura = pImpl_->expedienteInicioHora * 60;
    const int fechamento = pImpl_->expedienteFimHora * 60;

    return inicioMinutos >= abertura && fimMinutos <= fechamento;
}

void ProfissionalHandle::adicionarServico(const std::string& servicoId)
{
    if (servicoId.empty()) {
        throw std::invalid_argument("Servico nao pode ser vazio.");
    }
    // evita duplicar o mesmo servico na lista
    if (!atendeServico(servicoId)) {
        pImpl_->servicosAtendidos.push_back(servicoId);
    }
}

void ProfissionalHandle::removerServico(const std::string& servicoId)
{
    // remove todas as ocorrencias do servico da lista (idiom remove-erase)
    pImpl_->servicosAtendidos.erase(std::remove(pImpl_->servicosAtendidos.begin(), pImpl_->servicosAtendidos.end(), servicoId), pImpl_->servicosAtendidos.end());
}

} // namespace bellezasys
