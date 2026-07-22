#include "bellezasys/core/FinanceiroImpl.hpp"

#include <algorithm>
#include <chrono>
#include <iterator>
#include <numeric>
#include <stdexcept>

namespace bellezasys {

FinanceiroHandle::FinanceiroHandle() = default;

void FinanceiroHandle::limpar()
{
    pImpl_->movimentos.clear();
    pImpl_->comissoes.clear();
    pImpl_->relatorioPorProfissionalCache.clear();
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

    // a data do lancamento e a do atendimento, e nao o relogio do momento
    // em que o pagamento foi digitado: e isso que faz o relatorio por
    // periodo bater com a agenda, e o que mantem as datas corretas quando o
    // financeiro e reconstruido ao carregar o arquivo
    const DateTime data = agendamento.inicio();

    pImpl_->movimentos.push_back({"MOV-" + std::to_string(pImpl_->movimentos.size() + 1), agendamento.id(), "Recebimento de servico", data, agendamento.valor(), comissao});

    pImpl_->comissoes.push_back({agendamento.profissionalId(), agendamento.id(), comissao, data});
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

namespace {

/// true quando a data cai dentro do intervalo, que e fechado nas duas
/// pontas para o usuario nao precisar pensar em "ate 23:59:59"
bool dentroDoPeriodo(DateTime data, DateTime inicio, DateTime fim)
{
    return data >= inicio && data <= fim;
}

} // namespace

RelatorioFinanceiro FinanceiroHandle::relatorioPorPeriodo(DateTime inicio, DateTime fim) const
{
    if (inicio > fim) {
        throw std::invalid_argument("Inicio do periodo nao pode ser depois do fim.");
    }

    RelatorioFinanceiro relatorio;
    relatorio.inicio = inicio;
    relatorio.fim = fim;

    for (const MovimentoCaixa& movimento : pImpl_->movimentos) {
        if (!dentroDoPeriodo(movimento.data, inicio, fim)) {
            continue;
        }
        relatorio.entradas += movimento.entrada;
        relatorio.saidas += movimento.saida;
        ++relatorio.atendimentos;
    }
    relatorio.saldo = relatorio.entradas - relatorio.saidas;

    for (const Comissao& comissao : pImpl_->comissoes) {
        if (dentroDoPeriodo(comissao.data, inicio, fim)) {
            relatorio.totalComissoes += comissao.valor;
        }
    }

    return relatorio;
}

std::vector<RelatorioProfissional>::const_iterator FinanceiroHandle::relatorioPorProfissionalBegin(DateTime inicio, DateTime fim)
{
    if (inicio > fim) {
        throw std::invalid_argument("Inicio do periodo nao pode ser depois do fim.");
    }

    pImpl_->relatorioPorProfissionalCache.clear();

    // uma comissao por atendimento concluido, entao ela e a unidade de
    // contagem; o valor gerado vem do movimento correspondente
    for (const Comissao& comissao : pImpl_->comissoes) {
        if (!dentroDoPeriodo(comissao.data, inicio, fim)) {
            continue;
        }

        auto encontrado = std::find_if(pImpl_->relatorioPorProfissionalCache.begin(),
            pImpl_->relatorioPorProfissionalCache.end(),
            [&comissao](const RelatorioProfissional& linha) { return linha.profissionalId == comissao.profissionalId; });

        if (encontrado == pImpl_->relatorioPorProfissionalCache.end()) {
            pImpl_->relatorioPorProfissionalCache.push_back({comissao.profissionalId, 0, 0.0, 0.0});
            encontrado = std::prev(pImpl_->relatorioPorProfissionalCache.end());
        }

        ++encontrado->atendimentos;
        encontrado->totalComissoes += comissao.valor;

        for (const MovimentoCaixa& movimento : pImpl_->movimentos) {
            if (movimento.agendamentoId == comissao.agendamentoId) {
                encontrado->totalGerado += movimento.entrada;
                break;
            }
        }
    }

    // do que mais faturou para o que menos faturou; empate desempata pelo
    // id para a ordem nao variar entre execucoes
    std::sort(pImpl_->relatorioPorProfissionalCache.begin(), pImpl_->relatorioPorProfissionalCache.end(),
        [](const RelatorioProfissional& a, const RelatorioProfissional& b) {
            if (a.totalGerado != b.totalGerado) {
                return a.totalGerado > b.totalGerado;
            }
            return a.profissionalId < b.profissionalId;
        });

    return pImpl_->relatorioPorProfissionalCache.cbegin();
}

std::vector<RelatorioProfissional>::const_iterator FinanceiroHandle::relatorioPorProfissionalEnd()
{
    return pImpl_->relatorioPorProfissionalCache.cend();
}

} // namespace bellezasys
