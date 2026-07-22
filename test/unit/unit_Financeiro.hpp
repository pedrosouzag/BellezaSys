#ifndef UNIT_FINANCEIRO_HPP
#define UNIT_FINANCEIRO_HPP

namespace bellezasys {

/// suite de testes unitarios da classe FinanceiroHandle
class Unit_Financeiro {

public:

    static bool unit_Financeiro_defaultConstructor();
    static bool unit_Financeiro_Copyconstructor();
    static bool unit_Financeiro_destructor();
    static bool unit_Financeiro_assignmentOperator();

    static bool unit_Financeiro_registrarPagamento();
    static bool unit_Financeiro_dataDoLancamento();
    static bool unit_Financeiro_relatorioPorPeriodo();
    static bool unit_Financeiro_relatorioPorProfissional();
    static bool unit_Financeiro_registrarPagamento_validacoes();
    static bool unit_Financeiro_saldoETotalComissoes();

    /// confere a gestao de memoria dos objetos Handle e Body de Financeiro
    static bool unit_Financeiro_handleBodyTest();

    static bool run_unit_tests_Financeiro();
};

} // namespace bellezasys

#endif
