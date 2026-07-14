#include "bellezasys/core/AgendamentoImpl.hpp"

#include <stdexcept>
#include <utility>

namespace bellezasys {

// guarda os dados e valida ids, duracao e valor
AgendamentoBody::AgendamentoBody(std::string id, std::string clienteId, std::string profissionalId, std::string servicoId, DateTime inicio, std::chrono::minutes duracao, double valor)
    : id(std::move(id)), clienteId(std::move(clienteId)), profissionalId(std::move(profissionalId)), servicoId(std::move(servicoId)), inicio(inicio), duracao(duracao), valor(valor), status(StatusAgendamento::Agendado)
{
    if (this->id.empty() || this->clienteId.empty() || this->profissionalId.empty() || this->servicoId.empty()) {
        throw std::invalid_argument("Agendamento precisa de ids validos.");
    }
    if (this->duracao.count() <= 0) {
        throw std::invalid_argument("Duracao precisa ser positiva.");
    }
    if (this->valor < 0.0) {
        throw std::invalid_argument("Valor nao pode ser negativo.");
    }
}

AgendamentoHandle::AgendamentoHandle() = default;

AgendamentoHandle::AgendamentoHandle(std::string id, std::string clienteId, std::string profissionalId, std::string servicoId, DateTime inicio, std::chrono::minutes duracao, double valor)
    : Handle<AgendamentoBody>(new AgendamentoBody(std::move(id), std::move(clienteId), std::move(profissionalId), std::move(servicoId), inicio, duracao, valor))
{
}

bool AgendamentoHandle::isValid() const { return !pImpl_->id.empty(); }
const std::string& AgendamentoHandle::id() const { return pImpl_->id; }
const std::string& AgendamentoHandle::clienteId() const { return pImpl_->clienteId; }
const std::string& AgendamentoHandle::profissionalId() const { return pImpl_->profissionalId; }
const std::string& AgendamentoHandle::servicoId() const { return pImpl_->servicoId; }
DateTime AgendamentoHandle::inicio() const { return pImpl_->inicio; }
std::chrono::minutes AgendamentoHandle::duracao() const { return pImpl_->duracao; }
double AgendamentoHandle::valor() const { return pImpl_->valor; }
StatusAgendamento AgendamentoHandle::status() const { return pImpl_->status; }

// um agendamento so esta "ativo" enquanto ainda pode ser remarcado,
// cancelado ou concluido (Agendado ou Remarcado)
bool AgendamentoHandle::estaAtivo() const
{
    return isValid() && (pImpl_->status == StatusAgendamento::Agendado || pImpl_->status == StatusAgendamento::Remarcado);
}

void AgendamentoHandle::remarcar(DateTime novoInicio)
{
    if (!estaAtivo()) {
        throw std::logic_error("Apenas agendamentos ativos podem ser remarcados.");
    }
    pImpl_->inicio = novoInicio;
    pImpl_->status = StatusAgendamento::Remarcado;
}

void AgendamentoHandle::cancelar()
{
    if (!estaAtivo()) {
        throw std::logic_error("Apenas agendamentos ativos podem ser cancelados.");
    }
    pImpl_->status = StatusAgendamento::Cancelado;
}

void AgendamentoHandle::concluir()
{
    if (!estaAtivo()) {
        throw std::logic_error("Apenas agendamentos ativos podem ser concluidos.");
    }
    pImpl_->status = StatusAgendamento::Concluido;
}

} // namespace bellezasys
