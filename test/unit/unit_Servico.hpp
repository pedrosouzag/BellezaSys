#ifndef UNIT_SERVICO_HPP
#define UNIT_SERVICO_HPP

namespace bellezasys {

/// suite de testes unitarios da classe ServicoHandle
class Unit_Servico {

public:

    static bool unit_Servico_defaultConstructor();
    static bool unit_Servico_Constructor();
    static bool unit_Servico_Copyconstructor();
    static bool unit_Servico_destructor();
    static bool unit_Servico_assignmentOperator();

    static bool unit_Servico_getters();
    static bool unit_Servico_setNome();
    static bool unit_Servico_setPreco();
    static bool unit_Servico_setDuracao();
    static bool unit_Servico_setPercentualComissao();
    static bool unit_Servico_validacaoConstrutor();

    /// confere a gestao de memoria dos objetos Handle e Body de Servico
    static bool unit_Servico_handleBodyTest();

    static bool run_unit_tests_Servico();
};

} // namespace bellezasys

#endif
