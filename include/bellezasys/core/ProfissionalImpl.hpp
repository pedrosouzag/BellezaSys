#pragma once

#include "bellezasys/core/HandleBody.hpp"
#include "bellezasys/core/Profissional.hpp"

#include <string>
#include <vector>

namespace bellezasys {

/// classe Body responsavel por guardar os dados reais de um Profissional
class ProfissionalBody : public Body {
public:
    ProfissionalBody() = default;

    /// construtor com validacao (ver validar())
    ProfissionalBody(std::string id, std::string nome, std::string email, std::vector<std::string> servicosAtendidos, int expedienteInicioHora, int expedienteFimHora);

    /// confere campos obrigatorios e o intervalo de expediente; lanca
    /// excecao se algo estiver invalido
    void validar() const;

    std::string id;
    std::string nome;
    std::string email;
    std::vector<std::string> servicosAtendidos;
    int expedienteInicioHora = 0;
    int expedienteFimHora = 0;

    friend class Unit_Profissional;
};

/// classe Handle que implementa a interface Profissional, delegando os
/// dados reais para um ProfissionalBody (pImpl_)
class ProfissionalHandle : public Profissional, public Handle<ProfissionalBody> {
public:
    ProfissionalHandle();
    ProfissionalHandle(std::string id, std::string nome, std::string email, std::vector<std::string> servicosAtendidos, int expedienteInicioHora, int expedienteFimHora);

    bool isValid() const override;
    const std::string& id() const override;
    const std::string& nome() const override;
    const std::string& email() const override;
    const std::vector<std::string>& servicosAtendidos() const override;
    int expedienteInicioHora() const override;
    int expedienteFimHora() const override;

    bool atendeServico(const std::string& servicoId) const override;
    bool estaNoExpediente(DateTime inicio, std::chrono::minutes duracao) const override;
    void adicionarServico(const std::string& servicoId) override;
    void removerServico(const std::string& servicoId) override;

    ~ProfissionalHandle() override = default;

    friend class Unit_Profissional;
};

} // namespace bellezasys
