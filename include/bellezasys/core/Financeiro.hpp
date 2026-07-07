#pragma once

#include "bellezasys/core/Agendamento.hpp"
#include "bellezasys/core/Servico.hpp"
#include "bellezasys/core/Types.hpp"

#include <string>
#include <vector>

namespace bellezasys {

/// um lancamento no caixa do salao, gerado quando um atendimento e concluido
struct MovimentoCaixa {
    std::string id;
    std::string agendamentoId;
    std::string descricao;
    DateTime data;
    double entrada = 0.0;
    double saida = 0.0;
};

/// a comissao de um profissional sobre um atendimento concluido
struct Comissao {
    std::string profissionalId;
    std::string agendamentoId;
    double valor = 0.0;
};

/// interface do controle financeiro do salao: registra pagamentos e
/// calcula saldo de caixa e total de comissoes. os dados reais ficam em
/// FinanceiroBody (ver FinanceiroImpl.hpp)
class Financeiro {
public:
    /// registra o pagamento de um agendamento ja concluido, gerando um
    /// movimento de caixa e a comissao do profissional
    virtual void registrarPagamento(const Agendamento& agendamento, const Servico& servico) = 0;

    virtual double saldo() const = 0;
    virtual double totalComissoes() const = 0;
    virtual const std::vector<MovimentoCaixa>& movimentos() const = 0;
    virtual const std::vector<Comissao>& comissoes() const = 0;

    virtual ~Financeiro() = default;
};

} // namespace bellezasys
