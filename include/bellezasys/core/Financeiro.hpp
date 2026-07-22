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

    /// data do atendimento que gerou a comissao, usada para filtrar por periodo
    DateTime data;
};

/// consolidado do caixa dentro de um intervalo de datas
struct RelatorioFinanceiro {
    DateTime inicio;
    DateTime fim;
    double entradas = 0.0;
    double saidas = 0.0;
    double saldo = 0.0;
    double totalComissoes = 0.0;
    int atendimentos = 0;
};

/// uma linha do relatorio por profissional dentro de um periodo
struct RelatorioProfissional {
    std::string profissionalId;
    int atendimentos = 0;
    double totalGerado = 0.0;
    double totalComissoes = 0.0;
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

    /// consolida entradas, saidas, saldo e comissoes do intervalo. o
    /// intervalo e fechado nas duas pontas ([inicio, fim]) e usa a data do
    /// atendimento, nao a data em que o pagamento foi digitado
    virtual RelatorioFinanceiro relatorioPorPeriodo(DateTime inicio, DateTime fim) const = 0;

    /// quebra o mesmo periodo por profissional, ordenado do que mais gerou
    /// para o que menos gerou. Begin() recalcula a consulta, End() so
    /// devolve o fim do resultado ja calculado (chame sempre Begin() antes)
    virtual std::vector<RelatorioProfissional>::const_iterator relatorioPorProfissionalBegin(DateTime inicio, DateTime fim) = 0;
    virtual std::vector<RelatorioProfissional>::const_iterator relatorioPorProfissionalEnd() = 0;

    virtual ~Financeiro() = default;
};

} // namespace bellezasys
