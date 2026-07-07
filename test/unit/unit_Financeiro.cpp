#include "unit_Financeiro.hpp"
#include "bellezasys/core/AgendamentoImpl.hpp"
#include "bellezasys/core/FinanceiroImpl.hpp"
#include "bellezasys/core/ServicoImpl.hpp"

#include <cassert>
#include <chrono>
#include <stdexcept>

namespace bellezasys {

namespace {
// horario fixo usado em varios testes deste arquivo
const DateTime kHorario = makeDateTime(2026, 7, 7, 10, 0);
}

// confere que o construtor padrao inicializa o FinanceiroBody vazio
bool Unit_Financeiro::unit_Financeiro_defaultConstructor() {
    FinanceiroHandle financeiro;

    assert(financeiro.pImpl_->movimentos.empty());
    assert(financeiro.pImpl_->comissoes.empty());

    return true;
}

// confere que o construtor de copia compartilha o mesmo body (nao duplica)
bool Unit_Financeiro::unit_Financeiro_Copyconstructor() {
    FinanceiroHandle financeiro1;
    FinanceiroHandle financeiro2(financeiro1);

    assert(financeiro2.pImpl_ == financeiro1.pImpl_);

    return true;
}

// confere que o destrutor nao quebra a aplicacao
bool Unit_Financeiro::unit_Financeiro_destructor() {
    FinanceiroHandle* financeiro = new FinanceiroHandle();

    delete financeiro;

    return true;
}

// confere que o operador de atribuicao passa a apontar pro mesmo body
bool Unit_Financeiro::unit_Financeiro_assignmentOperator() {
    FinanceiroHandle financeiro1;
    FinanceiroHandle financeiro2;

    financeiro2 = financeiro1;

    assert(financeiro2.pImpl_ == financeiro1.pImpl_);

    return true;
}

// confere que registrarPagamento gera um movimento de caixa e uma comissao
// com os valores certos
bool Unit_Financeiro::unit_Financeiro_registrarPagamento() {
    FinanceiroHandle financeiro;
    ServicoHandle servico("S1", "Corte", 80.0, std::chrono::minutes(45), 0.35);
    AgendamentoHandle agendamento("AG-1", "CLI-1", "PRO-1", "S1", kHorario, std::chrono::minutes(45), 80.0);
    agendamento.concluir();

    financeiro.registrarPagamento(agendamento, servico);

    assert(financeiro.movimentos().size() == 1);
    assert(financeiro.comissoes().size() == 1);
    assert(financeiro.saldo() == 80.0 - (80.0 * 0.35));
    assert(financeiro.totalComissoes() == 80.0 * 0.35);

    return true;
}

// confere que registrarPagamento rejeita agendamento que ainda nao foi concluido
bool Unit_Financeiro::unit_Financeiro_registrarPagamento_validacoes() {
    FinanceiroHandle financeiro;
    ServicoHandle servico("S1", "Corte", 80.0, std::chrono::minutes(45), 0.35);
    AgendamentoHandle agendamentoNaoConcluido("AG-1", "CLI-1", "PRO-1", "S1", kHorario, std::chrono::minutes(45), 80.0);

    bool lancouExcecao = false;
    try {
        financeiro.registrarPagamento(agendamentoNaoConcluido, servico);
    } catch (const std::logic_error&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    return true;
}

// confere que saldo e totalComissoes somam corretamente varios pagamentos
bool Unit_Financeiro::unit_Financeiro_saldoETotalComissoes() {
    FinanceiroHandle financeiro;
    ServicoHandle servico("S1", "Corte", 80.0, std::chrono::minutes(45), 0.35);

    AgendamentoHandle agendamento1("AG-1", "CLI-1", "PRO-1", "S1", kHorario, std::chrono::minutes(45), 80.0);
    agendamento1.concluir();
    financeiro.registrarPagamento(agendamento1, servico);

    AgendamentoHandle agendamento2("AG-2", "CLI-2", "PRO-1", "S1", kHorario, std::chrono::minutes(45), 80.0);
    agendamento2.concluir();
    financeiro.registrarPagamento(agendamento2, servico);

    assert(financeiro.movimentos().size() == 2);
    assert(financeiro.saldo() == 2 * (80.0 - 80.0 * 0.35));
    assert(financeiro.totalComissoes() == 2 * (80.0 * 0.35));

    return true;
}

// confere a gestao de memoria: quantos handles e bodies sao criados e
// destruidos ao copiar e destruir um FinanceiroHandle
bool Unit_Financeiro::unit_Financeiro_handleBodyTest() {

#ifdef DEBUGING

    numHandleCreated = 0;
    numHandleDeleted = 0;
    numBodyCreated = 0;
    numBodyDeleted = 0;

    {
        FinanceiroHandle financeiro1;

        {
            FinanceiroHandle financeiro2(financeiro1);

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

// roda todos os testes unitarios de Financeiro em sequencia
bool Unit_Financeiro::run_unit_tests_Financeiro() {
    assert(unit_Financeiro_defaultConstructor());
    assert(unit_Financeiro_Copyconstructor());
    assert(unit_Financeiro_destructor());
    assert(unit_Financeiro_assignmentOperator());

    assert(unit_Financeiro_registrarPagamento());
    assert(unit_Financeiro_registrarPagamento_validacoes());
    assert(unit_Financeiro_saldoETotalComissoes());

    assert(unit_Financeiro_handleBodyTest());

    return true;
}

} // namespace bellezasys
