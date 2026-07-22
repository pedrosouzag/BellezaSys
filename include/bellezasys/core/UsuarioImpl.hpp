#pragma once

#include "bellezasys/core/HandleBody.hpp"
#include "bellezasys/core/Usuario.hpp"

#include <string>

namespace bellezasys {

/// classe Body responsavel por guardar os dados reais de um Usuario
class UsuarioBody : public Body {
public:
    UsuarioBody() = default;

    /// construtor com validacao: lanca excecao se algum campo obrigatorio
    /// estiver vazio
    UsuarioBody(std::string id, std::string nome, std::string email, std::string senha, Papel papel);

    std::string id;
    std::string nome;
    std::string email;
    std::string senha;
    Papel papel = Papel::Cliente;

    // preferencias do cliente; ficam vazias para funcionario e administrador
    std::string profissionalPreferidoId;
    std::string observacoes;

    friend class Unit_Usuario;
};

/// classe Handle que implementa a interface Usuario, delegando os dados
/// reais para um UsuarioBody (pImpl_)
class UsuarioHandle : public Usuario, public Handle<UsuarioBody> {
public:
    UsuarioHandle();
    UsuarioHandle(std::string id, std::string nome, std::string email, std::string senha, Papel papel);

    bool isValid() const override;
    const std::string& id() const override;
    const std::string& nome() const override;
    const std::string& email() const override;
    Papel papel() const override;

    void setNome(const std::string& nome) override;
    void setEmail(const std::string& email) override;

    const std::string& profissionalPreferidoId() const override;
    const std::string& observacoes() const override;
    void setProfissionalPreferidoId(const std::string& profissionalId) override;
    void setObservacoes(const std::string& observacoes) override;
    bool autenticar(const std::string& senha) const override;
    void alterarSenha(const std::string& senhaAtual, const std::string& novaSenha) override;

    ~UsuarioHandle() override = default;

    friend class BellezaSystemHandle;
    friend class Unit_Usuario;
};

} // namespace bellezasys
