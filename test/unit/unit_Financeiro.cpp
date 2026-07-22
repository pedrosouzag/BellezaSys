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

// confere que o lancamento herda a data do atendimento, e nao a data em que
// o pagamento foi registrado. sem isso o relatorio por periodo nao bate com
// a agenda, e recarregar o arquivo jogaria tudo para a data da carga
bool Unit_Financeiro::unit_Financeiro_dataDoLancamento() {
    FinanceiroHandle financeiro;
    ServicoHandle servico("S1", "Corte", 80.0, std::chrono::minutes(45), 0.35);
    AgendamentoHandle agendamento("AG-1", "CLI-1", "PRO-1", "S1", kHorario, std::chrono::minutes(45), 80.0);
    agendamento.concluir();

    financeiro.registrarPagamento(agendamento, servico);

    assert(financeiro.movimentos().front().data == kHorario);
    assert(financeiro.comissoes().front().data == kHorario);

    return true;
}

// confere o consolidado por periodo, com o intervalo fechado nas duas pontas
bool Unit_Financeiro::unit_Financeiro_relatorioPorPeriodo() {
    FinanceiroHandle financeiro;
    ServicoHandle servico("S1", "Corte", 100.0, std::chrono::minutes(45), 0.30);

    // um atendimento em cada dia: 06, 07 e 08 de julho
    const DateTime dia6 = makeDateTime(2026, 7, 6, 10, 0);
    const DateTime dia8 = makeDateTime(2026, 7, 8, 10, 0);
    for (const auto& par : { std::make_pair(std::string("AG-1"), dia6),
             std::make_pair(std::string("AG-2"), kHorario),
             std::make_pair(std::string("AG-3"), dia8) }) {
        AgendamentoHandle agendamento(par.first, "CLI-1", "PRO-1", "S1", par.second, std::chrono::minutes(45), 100.0);
        agendamento.concluir();
        financeiro.registrarPagamento(agendamento, servico);
    }

    // periodo que pega so o dia 7
    const RelatorioFinanceiro doDia = financeiro.relatorioPorPeriodo(
        makeDateTime(2026, 7, 7, 0, 0), makeDateTime(2026, 7, 7, 23, 59));
    assert(doDia.atendimentos == 1);
    assert(doDia.entradas == 100.0);
    assert(doDia.saidas == 30.0);
    assert(doDia.saldo == 70.0);
    assert(doDia.totalComissoes == 30.0);

    // as pontas do intervalo entram: exatamente 06 ate exatamente 08
    const RelatorioFinanceiro tudo = financeiro.relatorioPorPeriodo(dia6, dia8);
    assert(tudo.atendimentos == 3);
    assert(tudo.entradas == 300.0);
    assert(tudo.saldo == 210.0);

    // periodo sem nenhum atendimento zera, em vez de lancar
    const RelatorioFinanceiro vazio = financeiro.relatorioPorPeriodo(
        makeDateTime(2026, 1, 1, 0, 0), makeDateTime(2026, 1, 31, 23, 59));
    assert(vazio.atendimentos == 0);
    assert(vazio.saldo == 0.0);

    // periodo invertido e erro de quem chamou
    bool lancou = false;
    try {
        financeiro.relatorioPorPeriodo(dia8, dia6);
    } catch (const std::invalid_argument&) {
        lancou = true;
    }
    assert(lancou);

    return true;
}

// confere a quebra por profissional, ordenada do que mais faturou
bool Unit_Financeiro::unit_Financeiro_relatorioPorProfissional() {
    FinanceiroHandle financeiro;
    ServicoHandle barato("S1", "Corte", 50.0, std::chrono::minutes(30), 0.20);
    ServicoHandle caro("S2", "Coloracao", 200.0, std::chrono::minutes(120), 0.40);

    // PRO-1 faz dois atendimentos baratos (100 no total)
    for (const std::string& id : { std::string("AG-1"), std::string("AG-2") }) {
        AgendamentoHandle agendamento(id, "CLI-1", "PRO-1", "S1", kHorario, std::chrono::minutes(30), 50.0);
        agendamento.concluir();
        financeiro.registrarPagamento(agendamento, barato);
    }
    // PRO-2 faz um caro (200), entao deve vir primeiro na ordenacao
    AgendamentoHandle coloracao("AG-3", "CLI-2", "PRO-2", "S2", kHorario, std::chrono::minutes(120), 200.0);
    coloracao.concluir();
    financeiro.registrarPagamento(coloracao, caro);

    const DateTime inicio = makeDateTime(2026, 7, 7, 0, 0);
    const DateTime fim = makeDateTime(2026, 7, 7, 23, 59);

    auto it = financeiro.relatorioPorProfissionalBegin(inicio, fim);
    assert(std::distance(it, financeiro.relatorioPorProfissionalEnd()) == 2);

    assert(it->profissionalId == "PRO-2");
    assert(it->atendimentos == 1);
    assert(it->totalGerado == 200.0);
    assert(it->totalComissoes == 80.0);

    ++it;
    assert(it->profissionalId == "PRO-1");
    assert(it->atendimentos == 2);
    assert(it->totalGerado == 100.0);
    assert(it->totalComissoes == 20.0);

    // fora do periodo, a consulta devolve lista vazia
    auto vazio = financeiro.relatorioPorProfissionalBegin(
        makeDateTime(2026, 8, 1, 0, 0), makeDateTime(2026, 8, 31, 23, 59));
    assert(vazio == financeiro.relatorioPorProfissionalEnd());

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
    assert(unit_Financeiro_dataDoLancamento());
    assert(unit_Financeiro_relatorioPorPeriodo());
    assert(unit_Financeiro_relatorioPorProfissional());
    assert(unit_Financeiro_registrarPagamento_validacoes());
    assert(unit_Financeiro_saldoETotalComissoes());

    assert(unit_Financeiro_handleBodyTest());

    return true;
}

} // namespace bellezasys
