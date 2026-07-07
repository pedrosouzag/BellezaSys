#include "unit_Usuario.hpp"
#include "bellezasys/core/UsuarioImpl.hpp"

#include <cassert>
#include <stdexcept>

namespace bellezasys {

// confere que o construtor padrao inicializa o UsuarioBody com valores vazios
bool Unit_Usuario::unit_Usuario_defaultConstructor() {
    UsuarioHandle usuario;

    assert(usuario.pImpl_->id == "");
    assert(usuario.pImpl_->nome == "");
    assert(usuario.pImpl_->email == "");
    assert(usuario.pImpl_->senha == "");
    assert(usuario.pImpl_->papel == Papel::Cliente);

    return true;
}

// confere que o construtor parametrizado guarda os dados corretamente
bool Unit_Usuario::unit_Usuario_Constructor() {
    UsuarioHandle usuario("U1", "Ana", "ana@teste.com", "123", Papel::Funcionario);

    assert(usuario.pImpl_->id == "U1");
    assert(usuario.pImpl_->nome == "Ana");
    assert(usuario.pImpl_->email == "ana@teste.com");
    assert(usuario.pImpl_->senha == "123");
    assert(usuario.pImpl_->papel == Papel::Funcionario);

    return true;
}

// confere que o construtor de copia compartilha o mesmo body (nao duplica)
bool Unit_Usuario::unit_Usuario_Copyconstructor() {
    UsuarioHandle usuario1("U1", "Ana", "ana@teste.com", "123", Papel::Cliente);
    UsuarioHandle usuario2(usuario1);

    assert(usuario2.pImpl_->id == "U1");
    assert(usuario2.pImpl_ == usuario1.pImpl_);

    return true;
}

// confere que o destrutor nao quebra a aplicacao
bool Unit_Usuario::unit_Usuario_destructor() {
    UsuarioHandle* usuario = new UsuarioHandle("U1", "Ana", "ana@teste.com", "123", Papel::Cliente);

    delete usuario;

    return true;
}

// confere que o operador de atribuicao passa a apontar pro mesmo body
bool Unit_Usuario::unit_Usuario_assignmentOperator() {
    UsuarioHandle usuario1("U1", "Ana", "ana@teste.com", "123", Papel::Cliente);
    UsuarioHandle usuario2;

    usuario2 = usuario1;

    assert(usuario2.pImpl_->id == "U1");
    assert(usuario2.pImpl_ == usuario1.pImpl_);

    return true;
}

// confere que os getters retornam os dados guardados no body
bool Unit_Usuario::unit_Usuario_getters() {
    UsuarioHandle usuario("U1", "Ana", "ana@teste.com", "123", Papel::Administrador);

    assert(usuario.isValid());
    assert(usuario.id() == "U1");
    assert(usuario.nome() == "Ana");
    assert(usuario.email() == "ana@teste.com");
    assert(usuario.papel() == Papel::Administrador);

    return true;
}

// confere que setNome atualiza o nome e rejeita nome vazio
bool Unit_Usuario::unit_Usuario_setNome() {
    UsuarioHandle usuario("U1", "Ana", "ana@teste.com", "123", Papel::Cliente);

    usuario.setNome("Ana Paula");
    assert(usuario.pImpl_->nome == "Ana Paula");

    bool lancouExcecao = false;
    try {
        usuario.setNome("");
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    return true;
}

// confere que setEmail atualiza o email e rejeita email vazio
bool Unit_Usuario::unit_Usuario_setEmail() {
    UsuarioHandle usuario("U1", "Ana", "ana@teste.com", "123", Papel::Cliente);

    usuario.setEmail("novo@teste.com");
    assert(usuario.pImpl_->email == "novo@teste.com");

    bool lancouExcecao = false;
    try {
        usuario.setEmail("");
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    return true;
}

// confere que autenticar valida a senha certa e rejeita a errada
bool Unit_Usuario::unit_Usuario_autenticar() {
    UsuarioHandle usuario("U1", "Ana", "ana@teste.com", "123", Papel::Cliente);

    assert(usuario.autenticar("123"));
    assert(!usuario.autenticar("errada"));

    return true;
}

// confere que so troca a senha se a senha atual informada estiver correta
bool Unit_Usuario::unit_Usuario_alterarSenha() {
    UsuarioHandle usuario("U1", "Ana", "ana@teste.com", "123", Papel::Cliente);

    usuario.alterarSenha("123", "456");
    assert(usuario.autenticar("456"));

    bool lancouExcecao = false;
    try {
        usuario.alterarSenha("errada", "789");
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    return true;
}

// confere que o construtor rejeita campos obrigatorios vazios
bool Unit_Usuario::unit_Usuario_validacaoConstrutor() {
    bool lancouExcecao = false;
    try {
        UsuarioHandle usuario("", "Ana", "ana@teste.com", "123", Papel::Cliente);
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    return true;
}

// confere a gestao de memoria: quantos handles e bodies sao criados e
// destruidos ao copiar e destruir um UsuarioHandle
bool Unit_Usuario::unit_Usuario_handleBodyTest() {

#ifdef DEBUGING

    numHandleCreated = 0;
    numHandleDeleted = 0;
    numBodyCreated = 0;
    numBodyDeleted = 0;

    {
        UsuarioHandle usuario1("U1", "Ana", "ana@teste.com", "123", Papel::Cliente);

        {
            UsuarioHandle usuario2(usuario1);

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

// roda todos os testes unitarios de Usuario em sequencia
bool Unit_Usuario::run_unit_tests_Usuario() {
    assert(unit_Usuario_defaultConstructor());
    assert(unit_Usuario_Constructor());
    assert(unit_Usuario_Copyconstructor());
    assert(unit_Usuario_destructor());
    assert(unit_Usuario_assignmentOperator());

    assert(unit_Usuario_getters());
    assert(unit_Usuario_setNome());
    assert(unit_Usuario_setEmail());
    assert(unit_Usuario_autenticar());
    assert(unit_Usuario_alterarSenha());
    assert(unit_Usuario_validacaoConstrutor());

    assert(unit_Usuario_handleBodyTest());

    return true;
}

} // namespace bellezasys
