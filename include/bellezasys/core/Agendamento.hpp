#pragma once

#include "bellezasys/core/Types.hpp"

#include <chrono>
#include <string>

namespace bellezasys {

/// interface de um agendamento: liga um cliente (Usuario), um Profissional
/// e um Servico em um horario (DateTime), acompanhando o status ao longo
/// do ciclo de vida (StatusAgendamento). os dados reais ficam em
/// AgendamentoBody (ver AgendamentoImpl.hpp)
class Agendamento {
public:
    virtual bool isValid() const = 0;
    virtual const std::string& id() const = 0;
    virtual const std::string& clienteId() const = 0;
    virtual const std::string& profissionalId() const = 0;
    virtual const std::string& servicoId() const = 0;
    virtual DateTime inicio() const = 0;
    virtual std::chrono::minutes duracao() const = 0;
    virtual double valor() const = 0;
    virtual StatusAgendamento status() const = 0;

    /// true se o agendamento ainda esta valendo (Agendado ou Remarcado)
    virtual bool estaAtivo() const = 0;

    virtual void remarcar(DateTime novoInicio) = 0;
    virtual void cancelar() = 0;
    virtual void concluir() = 0;

    virtual ~Agendamento() = default;
};

} // namespace bellezasys
