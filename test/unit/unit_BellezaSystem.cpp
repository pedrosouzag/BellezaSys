#include "unit_BellezaSystem.hpp"
#include "bellezasys/core/BellezaSystemImpl.hpp"

#include <cassert>
#include <chrono>
#include <cstdio>
#include <iterator>
#include <stdexcept>

namespace bellezasys {

namespace {

// horario fixo usado em varios testes deste arquivo
const DateTime kHorario = makeDateTime(2026, 7, 7, 10, 0);

// monta um BellezaSystem com um cliente, um servico e um profissional ja
// cadastrados, prontos pra agendar
BellezaSystem* montarCenarioBasico() {
    BellezaSystem* system = BellezaSystem::createModel();
    system->cadastrarUsuario("CLI-1", "Marina", "marina@email.com", "123", Papel::Cliente);
    system->cadastrarServico("SER-CORTE", "Corte", 80.0, std::chrono::minutes(45), 0.35);
    system->cadastrarProfissional("PRO-1", "Ana", "ana@belleza.com", {"SER-CORTE"}, 9, 18);
    return system;
}

}

// confere que o construtor padrao inicializa o BellezaSystemBody vazio
bool Unit_BellezaSystem::unit_BellezaSystem_defaultConstructor() {
    BellezaSystemHandle system;

    assert(system.pImpl_->usuarios.empty());
    assert(system.pImpl_->servicos.empty());
    assert(system.pImpl_->profissionais.empty());
    assert(system.pImpl_->agendamentos.empty());

    return true;
}

// confere que o construtor de copia compartilha o mesmo body (nao duplica)
bool Unit_BellezaSystem::unit_BellezaSystem_Copyconstructor() {
    BellezaSystemHandle system1;
    BellezaSystemHandle system2(system1);

    assert(system2.pImpl_ == system1.pImpl_);

    return true;
}

// confere que o destrutor nao quebra a aplicacao
bool Unit_BellezaSystem::unit_BellezaSystem_destructor() {
    BellezaSystemHandle* system = new BellezaSystemHandle();

    delete system;

    return true;
}

// confere que o operador de atribuicao passa a apontar pro mesmo body
bool Unit_BellezaSystem::unit_BellezaSystem_assignmentOperator() {
    BellezaSystemHandle system1;
    BellezaSystemHandle system2;

    system2 = system1;

    assert(system2.pImpl_ == system1.pImpl_);

    return true;
}

// confere que cadastrarUsuario cria e registra o usuario no sistema
bool Unit_BellezaSystem::unit_BellezaSystem_cadastrarUsuario() {
    BellezaSystem* system = BellezaSystem::createModel();

    Usuario* usuario = system->cadastrarUsuario("CLI-1", "Marina", "marina@email.com", "123", Papel::Cliente);

    assert(usuario != nullptr);
    assert(usuario->id() == "CLI-1");
    assert(std::distance(system->usuariosBegin(), system->usuariosEnd()) == 1);

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que cadastrarUsuario rejeita id duplicado e email duplicado
bool Unit_BellezaSystem::unit_BellezaSystem_cadastrarUsuario_duplicado() {
    BellezaSystem* system = BellezaSystem::createModel();
    system->cadastrarUsuario("CLI-1", "Marina", "marina@email.com", "123", Papel::Cliente);

    bool lancouExcecaoId = false;
    try {
        system->cadastrarUsuario("CLI-1", "Outra", "outra@email.com", "123", Papel::Cliente);
    } catch (const std::invalid_argument&) {
        lancouExcecaoId = true;
    }
    assert(lancouExcecaoId);

    bool lancouExcecaoEmail = false;
    try {
        system->cadastrarUsuario("CLI-2", "Outra", "marina@email.com", "123", Papel::Cliente);
    } catch (const std::invalid_argument&) {
        lancouExcecaoEmail = true;
    }
    assert(lancouExcecaoEmail);

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que login aceita credenciais corretas e recusa as erradas
bool Unit_BellezaSystem::unit_BellezaSystem_login() {
    BellezaSystem* system = BellezaSystem::createModel();
    system->cadastrarUsuario("CLI-1", "Marina", "marina@email.com", "123", Papel::Cliente);

    assert(system->login("marina@email.com", "123"));
    assert(!system->login("marina@email.com", "errada"));
    assert(!system->login("naoexiste@email.com", "123"));

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que cadastrarServico cria e registra o servico no sistema
bool Unit_BellezaSystem::unit_BellezaSystem_cadastrarServico() {
    BellezaSystem* system = BellezaSystem::createModel();

    Servico* servico = system->cadastrarServico("SER-CORTE", "Corte", 80.0, std::chrono::minutes(45), 0.35);

    assert(servico != nullptr);
    assert(servico->preco() == 80.0);
    assert(std::distance(system->servicosBegin(), system->servicosEnd()) == 1);

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que cadastrarProfissional cria o profissional e rejeita servico inexistente
bool Unit_BellezaSystem::unit_BellezaSystem_cadastrarProfissional() {
    BellezaSystem* system = BellezaSystem::createModel();
    system->cadastrarServico("SER-CORTE", "Corte", 80.0, std::chrono::minutes(45), 0.35);

    Profissional* profissional = system->cadastrarProfissional("PRO-1", "Ana", "ana@belleza.com", {"SER-CORTE"}, 9, 18);
    assert(profissional != nullptr);
    assert(std::distance(system->profissionaisBegin(), system->profissionaisEnd()) == 1);

    // referenciar um servico inexistente deve falhar
    bool lancouExcecao = false;
    try {
        system->cadastrarProfissional("PRO-2", "Bia", "bia@belleza.com", {"SER-INEXISTENTE"}, 9, 18);
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que agendar cria o agendamento com os dados certos
bool Unit_BellezaSystem::unit_BellezaSystem_agendar() {
    BellezaSystem* system = montarCenarioBasico();

    Agendamento* agendamento = system->agendar("CLI-1", "PRO-1", "SER-CORTE", kHorario);

    assert(agendamento != nullptr);
    assert(agendamento->id() == "AG-1");
    assert(agendamento->valor() == 80.0);
    assert(std::distance(system->agendamentosBegin(), system->agendamentosEnd()) == 1);

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que agendar recusa dois agendamentos do mesmo profissional em
// horarios conflitantes
bool Unit_BellezaSystem::unit_BellezaSystem_agendar_conflitoDeHorario() {
    BellezaSystem* system = montarCenarioBasico();
    system->agendar("CLI-1", "PRO-1", "SER-CORTE", kHorario);

    bool lancouExcecao = false;
    try {
        // mesmo profissional, horario dentro da duracao do primeiro agendamento
        system->agendar("CLI-1", "PRO-1", "SER-CORTE", makeDateTime(2026, 7, 7, 10, 15));
    } catch (const std::logic_error&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que so cliente pode receber agendamento (funcionario nao pode)
bool Unit_BellezaSystem::unit_BellezaSystem_agendar_apenasCliente() {
    BellezaSystem* system = montarCenarioBasico();
    system->cadastrarUsuario("FUN-1", "Ana Func", "func@belleza.com", "123", Papel::Funcionario);

    bool lancouExcecao = false;
    try {
        system->agendar("FUN-1", "PRO-1", "SER-CORTE", kHorario);
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que remarcarAgendamento muda horario e status
bool Unit_BellezaSystem::unit_BellezaSystem_remarcarAgendamento() {
    BellezaSystem* system = montarCenarioBasico();
    Agendamento* agendamento = system->agendar("CLI-1", "PRO-1", "SER-CORTE", kHorario);

    const DateTime novoHorario = makeDateTime(2026, 7, 7, 14, 0);
    Agendamento* remarcado = system->remarcarAgendamento(agendamento->id(), novoHorario);

    assert(remarcado->status() == StatusAgendamento::Remarcado);
    assert(remarcado->inicio() == novoHorario);

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que cancelarAgendamento muda o status pra Cancelado
bool Unit_BellezaSystem::unit_BellezaSystem_cancelarAgendamento() {
    BellezaSystem* system = montarCenarioBasico();
    Agendamento* agendamento = system->agendar("CLI-1", "PRO-1", "SER-CORTE", kHorario);

    Agendamento* cancelado = system->cancelarAgendamento(agendamento->id());
    assert(cancelado->status() == StatusAgendamento::Cancelado);

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que concluirAgendamento muda o status e atualiza o financeiro
bool Unit_BellezaSystem::unit_BellezaSystem_concluirAgendamento() {
    BellezaSystem* system = montarCenarioBasico();
    Agendamento* agendamento = system->agendar("CLI-1", "PRO-1", "SER-CORTE", kHorario);

    Agendamento* concluido = system->concluirAgendamento(agendamento->id());
    assert(concluido->status() == StatusAgendamento::Concluido);

    // concluir agendamento registra o pagamento no financeiro
    assert(system->financeiro().saldo() == 80.0 - 80.0 * 0.35);
    assert(system->financeiro().totalComissoes() == 80.0 * 0.35);

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que profissionaisDisponiveis reflete a ocupacao apos um agendamento
bool Unit_BellezaSystem::unit_BellezaSystem_profissionaisDisponiveis() {
    BellezaSystem* system = montarCenarioBasico();

    auto inicio = system->profissionaisDisponiveisBegin("SER-CORTE", kHorario);
    assert(std::distance(inicio, system->profissionaisDisponiveisEnd()) == 1);

    system->agendar("CLI-1", "PRO-1", "SER-CORTE", kHorario);

    // apos o agendamento, o profissional fica ocupado nesse horario
    inicio = system->profissionaisDisponiveisBegin("SER-CORTE", kHorario);
    assert(inicio == system->profissionaisDisponiveisEnd());

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que agendamentosDoCliente e agendamentosDoProfissional filtram certo
bool Unit_BellezaSystem::unit_BellezaSystem_agendamentosDoClienteEDoProfissional() {
    BellezaSystem* system = montarCenarioBasico();
    system->cadastrarUsuario("CLI-2", "Joao", "joao@email.com", "123", Papel::Cliente);

    system->agendar("CLI-1", "PRO-1", "SER-CORTE", kHorario);
    system->agendar("CLI-2", "PRO-1", "SER-CORTE", makeDateTime(2026, 7, 7, 14, 0));

    auto inicioCli1 = system->agendamentosDoClienteBegin("CLI-1");
    assert(std::distance(inicioCli1, system->agendamentosDoClienteEnd()) == 1);

    auto inicioCli2 = system->agendamentosDoClienteBegin("CLI-2");
    assert(std::distance(inicioCli2, system->agendamentosDoClienteEnd()) == 1);

    auto inicioPro1 = system->agendamentosDoProfissionalBegin("PRO-1");
    assert(std::distance(inicioPro1, system->agendamentosDoProfissionalEnd()) == 2);

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que a agenda filtrada devolve somente os horarios daquele
// profissional no dia escolhido, ordenados por horario
bool Unit_BellezaSystem::unit_BellezaSystem_agendaDoProfissionalNoDia() {
    BellezaSystem* system = montarCenarioBasico();
    system->cadastrarUsuario("CLI-2", "Joao", "joao@email.com", "123", Papel::Cliente);
    system->cadastrarProfissional("PRO-2", "Bia", "bia@belleza.com", {"SER-CORTE"}, 9, 18);

    system->agendar("CLI-1", "PRO-1", "SER-CORTE", makeDateTime(2026, 7, 7, 14, 0));
    system->agendar("CLI-2", "PRO-1", "SER-CORTE", makeDateTime(2026, 7, 7, 10, 0));
    system->agendar("CLI-1", "PRO-1", "SER-CORTE", makeDateTime(2026, 7, 8, 10, 0));
    system->agendar("CLI-2", "PRO-2", "SER-CORTE", makeDateTime(2026, 7, 7, 11, 0));

    auto inicio = system->agendaDoProfissionalNoDiaBegin("PRO-1", makeDateTime(2026, 7, 7, 0, 0));
    auto fim = system->agendaDoProfissionalNoDiaEnd();
    assert(std::distance(inicio, fim) == 2);
    assert((*inicio)->inicio() == makeDateTime(2026, 7, 7, 10, 0));
    ++inicio;
    assert((*inicio)->inicio() == makeDateTime(2026, 7, 7, 14, 0));

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que salvar/carregar preserva cadastros, agenda, status e
// financeiro basico reconstruido a partir de atendimentos concluidos
bool Unit_BellezaSystem::unit_BellezaSystem_persistenciaArquivo() {
    const char* caminho = "bin/unit_bellezasys_persistencia.db";
    std::remove(caminho);

    BellezaSystem* origem = montarCenarioBasico();
    origem->cadastrarUsuario("CLI-2", "Joao", "joao@email.com", "abc", Papel::Cliente);

    Agendamento* concluido = origem->agendar("CLI-1", "PRO-1", "SER-CORTE", makeDateTime(2026, 7, 7, 10, 0));
    origem->concluirAgendamento(concluido->id());
    Agendamento* cancelado = origem->agendar("CLI-2", "PRO-1", "SER-CORTE", makeDateTime(2026, 7, 7, 12, 0));
    origem->cancelarAgendamento(cancelado->id());

    origem->salvarEmArquivo(caminho);

    BellezaSystem* carregado = BellezaSystem::createModel();
    carregado->carregarDeArquivo(caminho);

    assert(carregado->login("joao@email.com", "abc"));
    assert(std::distance(carregado->usuariosBegin(), carregado->usuariosEnd()) == 2);
    assert(std::distance(carregado->servicosBegin(), carregado->servicosEnd()) == 1);
    assert(std::distance(carregado->profissionaisBegin(), carregado->profissionaisEnd()) == 1);
    assert(std::distance(carregado->agendamentosBegin(), carregado->agendamentosEnd()) == 2);
    assert(carregado->financeiro().saldo() == 80.0 - 80.0 * 0.35);

    auto inicioCli2 = carregado->agendamentosDoClienteBegin("CLI-2");
    assert(std::distance(inicioCli2, carregado->agendamentosDoClienteEnd()) == 1);
    assert((*inicioCli2)->status() == StatusAgendamento::Cancelado);

    BellezaSystem::deleteModel(origem);
    BellezaSystem::deleteModel(carregado);
    std::remove(caminho);
    return true;
}

// confere a gestao de memoria: quantos handles e bodies sao criados e
// destruidos ao copiar e destruir um BellezaSystemHandle
bool Unit_BellezaSystem::unit_BellezaSystem_handleBodyTest() {

#ifdef DEBUGING

    numHandleCreated = 0;
    numHandleDeleted = 0;
    numBodyCreated = 0;
    numBodyDeleted = 0;

    {
        // BellezaSystemBody guarda um FinanceiroHandle como membro, entao a
        // criacao de UM BellezaSystemHandle cria DOIS pares Handle/Body
        // (Financeiro + BellezaSystem)
        BellezaSystemHandle system1;
        assert(numHandleCreated == 2);
        assert(numBodyCreated == 2);

        {
            BellezaSystemHandle system2(system1);

            // a copia compartilha o mesmo BellezaSystemBody (e o mesmo
            // Financeiro dentro dele): so um novo handle e criado
            assert(numHandleCreated == 3);
            assert(numBodyCreated == 2);
        }

        assert(numHandleDeleted == 1);
        assert(numBodyDeleted == 0);
    }

    // ao destruir o ultimo BellezaSystemHandle, o BellezaSystemBody e
    // destruido, o que por sua vez destroi o FinanceiroHandle que ele contem
    assert(numHandleDeleted == 3);
    assert(numBodyDeleted == 2);

#endif

    return true;
}

// roda todos os testes unitarios de BellezaSystem em sequencia
bool Unit_BellezaSystem::run_unit_tests_BellezaSystem() {
    assert(unit_BellezaSystem_defaultConstructor());
    assert(unit_BellezaSystem_Copyconstructor());
    assert(unit_BellezaSystem_destructor());
    assert(unit_BellezaSystem_assignmentOperator());

    assert(unit_BellezaSystem_cadastrarUsuario());
    assert(unit_BellezaSystem_cadastrarUsuario_duplicado());
    assert(unit_BellezaSystem_login());

    assert(unit_BellezaSystem_cadastrarServico());
    assert(unit_BellezaSystem_cadastrarProfissional());

    assert(unit_BellezaSystem_agendar());
    assert(unit_BellezaSystem_agendar_conflitoDeHorario());
    assert(unit_BellezaSystem_agendar_apenasCliente());

    assert(unit_BellezaSystem_remarcarAgendamento());
    assert(unit_BellezaSystem_cancelarAgendamento());
    assert(unit_BellezaSystem_concluirAgendamento());

    assert(unit_BellezaSystem_profissionaisDisponiveis());
    assert(unit_BellezaSystem_agendamentosDoClienteEDoProfissional());
    assert(unit_BellezaSystem_agendaDoProfissionalNoDia());
    assert(unit_BellezaSystem_persistenciaArquivo());

    assert(unit_BellezaSystem_handleBodyTest());

    return true;
}

} // namespace bellezasys
