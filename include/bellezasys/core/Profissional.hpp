#pragma once

#include "bellezasys/core/Types.hpp"

#include <chrono>
#include <string>
#include <vector>

namespace bellezasys {

/// interface de um profissional que presta servico no salao (cabeleireiro,
/// manicure etc). os dados reais ficam em ProfissionalBody (ver
/// ProfissionalImpl.hpp)
class Profissional {
public:
    virtual bool isValid() const = 0;
    virtual const std::string& id() const = 0;
    virtual const std::string& nome() const = 0;
    virtual const std::string& email() const = 0;
    virtual const std::vector<std::string>& servicosAtendidos() const = 0;
    virtual int expedienteInicioHora() const = 0;
    virtual int expedienteFimHora() const = 0;

    /// confere se o profissional atende o servico informado
    virtual bool atendeServico(const std::string& servicoId) const = 0;

    /// confere se o intervalo (inicio + duracao) cabe dentro do expediente
    virtual bool estaNoExpediente(DateTime inicio, std::chrono::minutes duracao) const = 0;

    virtual void adicionarServico(const std::string& servicoId) = 0;
    virtual void removerServico(const std::string& servicoId) = 0;

    virtual ~Profissional() = default;
};

} // namespace bellezasys
