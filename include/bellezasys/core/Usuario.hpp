#pragma once

#include "bellezasys/core/Types.hpp"

#include <string>

namespace bellezasys {

/// interface de uma conta do BellezaSys (cliente, funcionario ou
/// administrador). representa somente o contrato publico; os dados reais
/// ficam em UsuarioBody (ver UsuarioImpl.hpp)
class Usuario {
public:
    /// indica se o usuario tem um id valido (foi realmente cadastrado)
    virtual bool isValid() const = 0;

    virtual const std::string& id() const = 0;
    virtual const std::string& nome() const = 0;
    virtual const std::string& email() const = 0;
    virtual Papel papel() const = 0;

    virtual void setNome(const std::string& nome) = 0;
    virtual void setEmail(const std::string& email) = 0;

    /// confere se a senha informada bate com a senha cadastrada
    virtual bool autenticar(const std::string& senha) const = 0;

    /// troca a senha, exigindo a senha atual correta
    virtual void alterarSenha(const std::string& senhaAtual, const std::string& novaSenha) = 0;

    virtual ~Usuario() = default;
};

} // namespace bellezasys
