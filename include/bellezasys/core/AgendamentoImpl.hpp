#pragma once

#include "bellezasys/core/Agendamento.hpp"
#include "bellezasys/core/HandleBody.hpp"

#include <chrono>
#include <string>

namespace bellezasys {

/// classe Body responsavel por guardar os dados reais de um Agendamento
class AgendamentoBody : public Body {
public:
    AgendamentoBody() = default;

    /// construtor com validacao: exige ids preenchidos, duracao positiva
    /// e valor nao negativo
    AgendamentoBody(std::string id, std::string clienteId, std::string profissionalId, std::string servicoId, DateTime inicio, std::chrono::minutes duracao, double valor);

    std::string id;
    std::string clienteId;
    std::string profissionalId;
    std::string servicoId;
    DateTime inicio = DateTime{};
    std::chrono::minutes duracao = std::chrono::minutes(0);
    double valor = 0.0;
    StatusAgendamento status = StatusAgendamento::Agendado;

    friend class Unit_Agendamento;
};

/// classe Handle que implementa a interface Agendamento, delegando os
/// dados reais para um AgendamentoBody (pImpl_)
class AgendamentoHandle : public Agendamento, public Handle<AgendamentoBody> {
public:
    AgendamentoHandle();
    AgendamentoHandle(std::string id, std::string clienteId, std::string profissionalId, std::string servicoId, DateTime inicio, std::chrono::minutes duracao, double valor);

    bool isValid() const override;
    const std::string& id() const override;
    const std::string& clienteId() const override;
    const std::string& profissionalId() const override;
    const std::string& servicoId() const override;
    DateTime inicio() const override;
    std::chrono::minutes duracao() const override;
    double valor() const override;
    StatusAgendamento status() const override;

    bool estaAtivo() const override;
    void remarcar(DateTime novoInicio) override;
    void cancelar() override;
    void concluir() override;

    ~AgendamentoHandle() override = default;

    friend class Unit_Agendamento;
};

} // namespace bellezasys
