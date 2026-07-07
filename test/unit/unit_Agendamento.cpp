#include "unit_Agendamento.hpp"
#include "bellezasys/core/AgendamentoImpl.hpp"

#include <cassert>
#include <chrono>
#include <stdexcept>

namespace bellezasys {

namespace {
// horario fixo usado em varios testes deste arquivo
const DateTime kHorario = makeDateTime(2026, 7, 7, 10, 0);
}

// confere que o construtor padrao inicializa o AgendamentoBody vazio
bool Unit_Agendamento::unit_Agendamento_defaultConstructor() {
    AgendamentoHandle agendamento;

    assert(agendamento.pImpl_->id == "");
    assert(agendamento.pImpl_->clienteId == "");
    assert(agendamento.pImpl_->profissionalId == "");
    assert(agendamento.pImpl_->servicoId == "");
    assert(agendamento.pImpl_->valor == 0.0);
    assert(agendamento.pImpl_->status == StatusAgendamento::Agendado);

    return true;
}

// confere que o construtor parametrizado guarda os dados corretamente
bool Unit_Agendamento::unit_Agendamento_Constructor() {
    AgendamentoHandle agendamento("AG-1", "CLI-1", "PRO-1", "SER-1", kHorario, std::chrono::minutes(45), 80.0);

    assert(agendamento.pImpl_->id == "AG-1");
    assert(agendamento.pImpl_->clienteId == "CLI-1");
    assert(agendamento.pImpl_->profissionalId == "PRO-1");
    assert(agendamento.pImpl_->servicoId == "SER-1");
    assert(agendamento.pImpl_->valor == 80.0);
    assert(agendamento.pImpl_->status == StatusAgendamento::Agendado);

    return true;
}

// confere que o construtor de copia compartilha o mesmo body (nao duplica)
bool Unit_Agendamento::unit_Agendamento_Copyconstructor() {
    AgendamentoHandle agendamento1("AG-1", "CLI-1", "PRO-1", "SER-1", kHorario, std::chrono::minutes(45), 80.0);
    AgendamentoHandle agendamento2(agendamento1);

    assert(agendamento2.pImpl_->id == "AG-1");
    assert(agendamento2.pImpl_ == agendamento1.pImpl_);

    return true;
}

// confere que o destrutor nao quebra a aplicacao
bool Unit_Agendamento::unit_Agendamento_destructor() {
    AgendamentoHandle* agendamento = new AgendamentoHandle("AG-1", "CLI-1", "PRO-1", "SER-1", kHorario, std::chrono::minutes(45), 80.0);

    delete agendamento;

    return true;
}

// confere que o operador de atribuicao passa a apontar pro mesmo body
bool Unit_Agendamento::unit_Agendamento_assignmentOperator() {
    AgendamentoHandle agendamento1("AG-1", "CLI-1", "PRO-1", "SER-1", kHorario, std::chrono::minutes(45), 80.0);
    AgendamentoHandle agendamento2;

    agendamento2 = agendamento1;

    assert(agendamento2.pImpl_->id == "AG-1");
    assert(agendamento2.pImpl_ == agendamento1.pImpl_);

    return true;
}

// confere que os getters retornam os dados guardados no body
bool Unit_Agendamento::unit_Agendamento_getters() {
    AgendamentoHandle agendamento("AG-1", "CLI-1", "PRO-1", "SER-1", kHorario, std::chrono::minutes(45), 80.0);

    assert(agendamento.isValid());
    assert(agendamento.id() == "AG-1");
    assert(agendamento.clienteId() == "CLI-1");
    assert(agendamento.profissionalId() == "PRO-1");
    assert(agendamento.servicoId() == "SER-1");
    assert(agendamento.inicio() == kHorario);
    assert(agendamento.duracao() == std::chrono::minutes(45));
    assert(agendamento.valor() == 80.0);
    assert(agendamento.status() == StatusAgendamento::Agendado);

    return true;
}

// confere que estaAtivo muda pra false depois do cancelamento
bool Unit_Agendamento::unit_Agendamento_estaAtivo() {
    AgendamentoHandle agendamento("AG-1", "CLI-1", "PRO-1", "SER-1", kHorario, std::chrono::minutes(45), 80.0);

    assert(agendamento.estaAtivo());
    agendamento.cancelar();
    assert(!agendamento.estaAtivo());

    return true;
}

// confere que remarcar muda o horario e o status, e rejeita se ja estiver cancelado
bool Unit_Agendamento::unit_Agendamento_remarcar() {
    AgendamentoHandle agendamento("AG-1", "CLI-1", "PRO-1", "SER-1", kHorario, std::chrono::minutes(45), 80.0);

    const DateTime novoHorario = makeDateTime(2026, 7, 7, 14, 0);
    agendamento.remarcar(novoHorario);

    assert(agendamento.inicio() == novoHorario);
    assert(agendamento.status() == StatusAgendamento::Remarcado);

    agendamento.cancelar();
    bool lancouExcecao = false;
    try {
        agendamento.remarcar(kHorario);
    } catch (const std::logic_error&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    return true;
}

// confere que cancelar muda o status e rejeita cancelar duas vezes
bool Unit_Agendamento::unit_Agendamento_cancelar() {
    AgendamentoHandle agendamento("AG-1", "CLI-1", "PRO-1", "SER-1", kHorario, std::chrono::minutes(45), 80.0);

    agendamento.cancelar();
    assert(agendamento.status() == StatusAgendamento::Cancelado);

    bool lancouExcecao = false;
    try {
        agendamento.cancelar();
    } catch (const std::logic_error&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    return true;
}

// confere que concluir muda o status e rejeita concluir duas vezes
bool Unit_Agendamento::unit_Agendamento_concluir() {
    AgendamentoHandle agendamento("AG-1", "CLI-1", "PRO-1", "SER-1", kHorario, std::chrono::minutes(45), 80.0);

    agendamento.concluir();
    assert(agendamento.status() == StatusAgendamento::Concluido);

    bool lancouExcecao = false;
    try {
        agendamento.concluir();
    } catch (const std::logic_error&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    return true;
}

// confere que o construtor rejeita ids vazios e duracao nao positiva
bool Unit_Agendamento::unit_Agendamento_validacaoConstrutor() {
    bool lancouExcecao = false;
    try {
        AgendamentoHandle agendamento("", "CLI-1", "PRO-1", "SER-1", kHorario, std::chrono::minutes(45), 80.0);
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    lancouExcecao = false;
    try {
        AgendamentoHandle agendamento("AG-1", "CLI-1", "PRO-1", "SER-1", kHorario, std::chrono::minutes(0), 80.0);
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    return true;
}

// confere a gestao de memoria: quantos handles e bodies sao criados e
// destruidos ao copiar e destruir um AgendamentoHandle
bool Unit_Agendamento::unit_Agendamento_handleBodyTest() {

#ifdef DEBUGING

    numHandleCreated = 0;
    numHandleDeleted = 0;
    numBodyCreated = 0;
    numBodyDeleted = 0;

    {
        AgendamentoHandle agendamento1("AG-1", "CLI-1", "PRO-1", "SER-1", kHorario, std::chrono::minutes(45), 80.0);

        {
            AgendamentoHandle agendamento2(agendamento1);

            assert(numHandleCreated == 2);
            assert(numBodyCreated == 1);
        }

        assert(numHandleDeleted == 1);
        assert(numBodyDeleted == 0);
    }

    assert(numHandleDeleted == 2);
    assert(numBodyDeleted == 1);

#endif

    return true;
}

// roda todos os testes unitarios de Agendamento em sequencia
bool Unit_Agendamento::run_unit_tests_Agendamento() {
    assert(unit_Agendamento_defaultConstructor());
    assert(unit_Agendamento_Constructor());
    assert(unit_Agendamento_Copyconstructor());
    assert(unit_Agendamento_destructor());
    assert(unit_Agendamento_assignmentOperator());

    assert(unit_Agendamento_getters());
    assert(unit_Agendamento_estaAtivo());
    assert(unit_Agendamento_remarcar());
    assert(unit_Agendamento_cancelar());
    assert(unit_Agendamento_concluir());
    assert(unit_Agendamento_validacaoConstrutor());

    assert(unit_Agendamento_handleBodyTest());

    return true;
}

} // namespace bellezasys
