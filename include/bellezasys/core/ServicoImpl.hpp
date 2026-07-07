#pragma once

#include "bellezasys/core/HandleBody.hpp"
#include "bellezasys/core/Servico.hpp"

#include <chrono>
#include <string>

namespace bellezasys {

/// classe Body responsavel por guardar os dados reais de um Servico
class ServicoBody : public Body {
public:
    ServicoBody() = default;

    /// construtor com validacao (ver validar())
    ServicoBody(std::string id, std::string nome, double preco, std::chrono::minutes duracao, double percentualComissao);

    /// confere campos obrigatorios, preco, duracao e faixa da comissao;
    /// lanca excecao se algo estiver invalido
    void validar() const;

    std::string id;
    std::string nome;
    double preco = 0.0;
    std::chrono::minutes duracao = std::chrono::minutes(0);
    double percentualComissao = 0.0;

    friend class Unit_Servico;
};

/// classe Handle que implementa a interface Servico, delegando os dados
/// reais para um ServicoBody (pImpl_)
class ServicoHandle : public Servico, public Handle<ServicoBody> {
public:
    ServicoHandle();
    ServicoHandle(std::string id, std::string nome, double preco, std::chrono::minutes duracao, double percentualComissao);

    bool isValid() const override;
    const std::string& id() const override;
    const std::string& nome() const override;
    double preco() const override;
    std::chrono::minutes duracao() const override;
    double percentualComissao() const override;

    void setNome(const std::string& nome) override;
    void setPreco(double preco) override;
    void setDuracao(std::chrono::minutes duracao) override;
    void setPercentualComissao(double percentualComissao) override;

    ~ServicoHandle() override = default;

    friend class Unit_Servico;
};

} // namespace bellezasys
