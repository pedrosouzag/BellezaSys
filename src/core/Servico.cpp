#include "bellezasys/core/ServicoImpl.hpp"

#include <stdexcept>
#include <utility>

namespace bellezasys {

// guarda os dados e valida logo em seguida
ServicoBody::ServicoBody(std::string id, std::string nome, double preco, std::chrono::minutes duracao, double percentualComissao)
    : id(std::move(id)), nome(std::move(nome)), preco(preco), duracao(duracao), percentualComissao(percentualComissao)
{
    validar();
}

void ServicoBody::validar() const
{
    if (id.empty() || nome.empty()) {
        throw std::invalid_argument("Servico precisa de id e nome.");
    }
    if (preco < 0.0) {
        throw std::invalid_argument("Preco nao pode ser negativo.");
    }
    if (duracao.count() <= 0) {
        throw std::invalid_argument("Duracao precisa ser positiva.");
    }
    // comissao e um percentual, entao precisa estar entre 0 e 1
    if (percentualComissao < 0.0 || percentualComissao > 1.0) {
        throw std::invalid_argument("Comissao deve estar entre 0 e 1.");
    }
}

ServicoHandle::ServicoHandle() = default;

ServicoHandle::ServicoHandle(std::string id, std::string nome, double preco, std::chrono::minutes duracao, double percentualComissao)
    : Handle<ServicoBody>(new ServicoBody(std::move(id), std::move(nome), preco, duracao, percentualComissao))
{
}

bool ServicoHandle::isValid() const { return !pImpl_->id.empty(); }
const std::string& ServicoHandle::id() const { return pImpl_->id; }
const std::string& ServicoHandle::nome() const { return pImpl_->nome; }
double ServicoHandle::preco() const { return pImpl_->preco; }
std::chrono::minutes ServicoHandle::duracao() const { return pImpl_->duracao; }
double ServicoHandle::percentualComissao() const { return pImpl_->percentualComissao; }

void ServicoHandle::setNome(const std::string& nome)
{
    if (nome.empty()) {
        throw std::invalid_argument("Nome do servico nao pode ser vazio.");
    }
    pImpl_->nome = nome;
}

void ServicoHandle::setPreco(double preco)
{
    if (preco < 0.0) {
        throw std::invalid_argument("Preco nao pode ser negativo.");
    }
    pImpl_->preco = preco;
}

void ServicoHandle::setDuracao(std::chrono::minutes duracao)
{
    if (duracao.count() <= 0) {
        throw std::invalid_argument("Duracao precisa ser positiva.");
    }
    pImpl_->duracao = duracao;
}

void ServicoHandle::setPercentualComissao(double percentualComissao)
{
    if (percentualComissao < 0.0 || percentualComissao > 1.0) {
        throw std::invalid_argument("Comissao deve estar entre 0 e 1.");
    }
    pImpl_->percentualComissao = percentualComissao;
}

} // namespace bellezasys
