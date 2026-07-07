#ifndef UNIT_USUARIO_HPP
#define UNIT_USUARIO_HPP

namespace bellezasys {

/// suite de testes unitarios da classe UsuarioHandle
class Unit_Usuario {

public:

    static bool unit_Usuario_defaultConstructor();
    static bool unit_Usuario_Constructor();
    static bool unit_Usuario_Copyconstructor();
    static bool unit_Usuario_destructor();
    static bool unit_Usuario_assignmentOperator();

    static bool unit_Usuario_getters();
    static bool unit_Usuario_setNome();
    static bool unit_Usuario_setEmail();
    static bool unit_Usuario_autenticar();
    static bool unit_Usuario_alterarSenha();
    static bool unit_Usuario_validacaoConstrutor();

    /// confere a gestao de memoria dos objetos Handle e Body de Usuario
    static bool unit_Usuario_handleBodyTest();

    static bool run_unit_tests_Usuario();
};

} // namespace bellezasys

#endif
