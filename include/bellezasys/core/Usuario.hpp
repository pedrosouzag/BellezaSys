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

    /// id do profissional que o cliente prefere ser atendido; string vazia
    /// quando ele nao tem preferencia
    virtual const std::string& profissionalPreferidoId() const = 0;

    /// texto livre com observacoes do cliente (alergias, produtos que nao
    /// pode usar, horario que costuma preferir)
    virtual const std::string& observacoes() const = 0;

    /// quem valida se o profissional existe e o BellezaSystem; aqui a troca
    /// e direta, e a string vazia significa "sem preferencia"
    virtual void setProfissionalPreferidoId(const std::string& profissionalId) = 0;
    virtual void setObservacoes(const std::string& observacoes) = 0;

    /// confere se a senha informada bate com a senha cadastrada
    virtual bool autenticar(const std::string& senha) const = 0;

    /// troca a senha, exigindo a senha atual correta
    virtual void alterarSenha(const std::string& senhaAtual, const std::string& novaSenha) = 0;

    virtual ~Usuario() = default;
};

} // namespace bellezasys
