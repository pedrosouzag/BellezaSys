#ifndef UNIT_BELLEZASYSTEM_HPP
#define UNIT_BELLEZASYSTEM_HPP

namespace bellezasys {

/// suite de testes unitarios da classe BellezaSystemHandle
class Unit_BellezaSystem {

public:

    static bool unit_BellezaSystem_defaultConstructor();
    static bool unit_BellezaSystem_Copyconstructor();
    static bool unit_BellezaSystem_destructor();
    static bool unit_BellezaSystem_assignmentOperator();

    static bool unit_BellezaSystem_cadastrarUsuario();
    static bool unit_BellezaSystem_cadastrarUsuario_duplicado();
    static bool unit_BellezaSystem_login();

    static bool unit_BellezaSystem_cadastrarServico();
    static bool unit_BellezaSystem_cadastrarProfissional();

    static bool unit_BellezaSystem_agendar();
    static bool unit_BellezaSystem_agendar_conflitoDeHorario();
    static bool unit_BellezaSystem_agendar_apenasCliente();

    static bool unit_BellezaSystem_remarcarAgendamento();
    static bool unit_BellezaSystem_cancelarAgendamento();
    static bool unit_BellezaSystem_concluirAgendamento();

    static bool unit_BellezaSystem_profissionaisDisponiveis();
    static bool unit_BellezaSystem_agendamentosDoClienteEDoProfissional();

    /// confere a gestao de memoria dos objetos Handle e Body de BellezaSystem
    static bool unit_BellezaSystem_handleBodyTest();

    static bool run_unit_tests_BellezaSystem();
};

} // namespace bellezasys

#endif
