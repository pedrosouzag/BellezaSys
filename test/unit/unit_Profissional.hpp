#ifndef UNIT_PROFISSIONAL_HPP
#define UNIT_PROFISSIONAL_HPP

namespace bellezasys {

/// suite de testes unitarios da classe ProfissionalHandle
class Unit_Profissional {

public:

    static bool unit_Profissional_defaultConstructor();
    static bool unit_Profissional_Constructor();
    static bool unit_Profissional_Copyconstructor();
    static bool unit_Profissional_destructor();
    static bool unit_Profissional_assignmentOperator();

    static bool unit_Profissional_getters();
    static bool unit_Profissional_atendeServico();
    static bool unit_Profissional_estaNoExpediente();
    static bool unit_Profissional_adicionarServico();
    static bool unit_Profissional_removerServico();
    static bool unit_Profissional_validacaoConstrutor();

    /// confere a gestao de memoria dos objetos Handle e Body de Profissional
    static bool unit_Profissional_handleBodyTest();

    static bool run_unit_tests_Profissional();
};

} // namespace bellezasys

#endif
