#pragma once

#include <chrono>
#include <string>

namespace bellezasys {

/// interface de um servico oferecido pelo salao (corte, manicure etc). os
/// dados reais ficam em ServicoBody (ver ServicoImpl.hpp)
class Servico {
public:
    virtual bool isValid() const = 0;
    virtual const std::string& id() const = 0;
    virtual const std::string& nome() const = 0;
    virtual double preco() const = 0;
    virtual std::chrono::minutes duracao() const = 0;

    /// percentual (0 a 1) que o profissional recebe de comissao sobre o preco
    virtual double percentualComissao() const = 0;

    virtual void setNome(const std::string& nome) = 0;
    virtual void setPreco(double preco) = 0;
    virtual void setDuracao(std::chrono::minutes duracao) = 0;
    virtual void setPercentualComissao(double percentualComissao) = 0;

    virtual ~Servico() = default;
};

} // namespace bellezasys
