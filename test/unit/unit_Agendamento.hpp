#ifndef UNIT_AGENDAMENTO_HPP
#define UNIT_AGENDAMENTO_HPP

namespace bellezasys {

/// suite de testes unitarios da classe AgendamentoHandle
class Unit_Agendamento {

public:

    static bool unit_Agendamento_defaultConstructor();
    static bool unit_Agendamento_Constructor();
    static bool unit_Agendamento_Copyconstructor();
    static bool unit_Agendamento_destructor();
    static bool unit_Agendamento_assignmentOperator();

    static bool unit_Agendamento_getters();
    static bool unit_Agendamento_estaAtivo();
    static bool unit_Agendamento_remarcar();
    static bool unit_Agendamento_cancelar();
    static bool unit_Agendamento_concluir();
    static bool unit_Agendamento_validacaoConstrutor();

    /// confere a gestao de memoria dos objetos Handle e Body de Agendamento
    static bool unit_Agendamento_handleBodyTest();

    static bool run_unit_tests_Agendamento();
};

} // namespace bellezasys

#endif
