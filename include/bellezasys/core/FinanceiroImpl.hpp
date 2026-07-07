#pragma once

#include "bellezasys/core/Financeiro.hpp"
#include "bellezasys/core/HandleBody.hpp"

#include <string>
#include <vector>

namespace bellezasys {

/// classe Body responsavel por guardar os movimentos de caixa e as
/// comissoes registradas
class FinanceiroBody : public Body {
public:
    std::vector<MovimentoCaixa> movimentos;
    std::vector<Comissao> comissoes;

    friend class Unit_Financeiro;
};

/// classe Handle que implementa a interface Financeiro, delegando os
/// dados reais para um FinanceiroBody (pImpl_)
class FinanceiroHandle : public Financeiro, public Handle<FinanceiroBody> {
public:
    FinanceiroHandle();

    void registrarPagamento(const Agendamento& agendamento, const Servico& servico) override;
    double saldo() const override;
    double totalComissoes() const override;
    const std::vector<MovimentoCaixa>& movimentos() const override;
    const std::vector<Comissao>& comissoes() const override;

    ~FinanceiroHandle() override = default;

    friend class Unit_Financeiro;
};

} // namespace bellezasys
