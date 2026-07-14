#include "bellezasys/core/FinanceiroImpl.hpp"

#include <chrono>
#include <numeric>
#include <stdexcept>

namespace bellezasys {

FinanceiroHandle::FinanceiroHandle() = default;

void FinanceiroHandle::limpar()
{
    pImpl_->movimentos.clear();
    pImpl_->comissoes.clear();
}

// gera um movimento de caixa (entrada = valor do servico, saida = comissao
// do profissional) e registra a comissao separadamente
void FinanceiroHandle::registrarPagamento(const Agendamento& agendamento, const Servico& servico)
{
    if (!agendamento.isValid() || !servico.isValid()) {
        throw std::invalid_argument("Pagamento precisa de agendamento e servico validos.");
    }
    if (agendamento.status() != StatusAgendamento::Concluido) {
        throw std::logic_error("Pagamento so pode ser registrado apos conclusao do agendamento.");
    }

    const double comissao = agendamento.valor() * servico.percentualComissao();
    pImpl_->movimentos.push_back({"MOV-" + std::to_string(pImpl_->movimentos.size() + 1), agendamento.id(), "Recebimento de servico", std::chrono::system_clock::now(), agendamento.valor(), comissao});

    pImpl_->comissoes.push_back({agendamento.profissionalId(), agendamento.id(), comissao});
}

// soma todas as entradas e subtrai todas as saidas registradas
double FinanceiroHandle::saldo() const
{
    return std::accumulate(pImpl_->movimentos.begin(), pImpl_->movimentos.end(), 0.0, [](double total, const MovimentoCaixa& movimento) { return total + movimento.entrada - movimento.saida; });
}

// soma o valor de todas as comissoes registradas
double FinanceiroHandle::totalComissoes() const
{
    return std::accumulate(pImpl_->comissoes.begin(), pImpl_->comissoes.end(), 0.0, [](double total, const Comissao& comissao) { return total + comissao.valor; });
}

const std::vector<MovimentoCaixa>& FinanceiroHandle::movimentos() const { return pImpl_->movimentos; }
const std::vector<Comissao>& FinanceiroHandle::comissoes() const { return pImpl_->comissoes; }

} // namespace bellezasys
