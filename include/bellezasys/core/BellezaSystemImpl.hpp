#pragma once

#include "bellezasys/core/AgendamentoImpl.hpp"
#include "bellezasys/core/BellezaSystem.hpp"
#include "bellezasys/core/FinanceiroImpl.hpp"
#include "bellezasys/core/HandleBody.hpp"
#include "bellezasys/core/ProfissionalImpl.hpp"
#include "bellezasys/core/ServicoImpl.hpp"
#include "bellezasys/core/UsuarioImpl.hpp"

#include <string>
#include <vector>

namespace bellezasys {

/// classe Body responsavel por guardar os dados do BellezaSystem.
///
/// e dona (por ponteiro, alocados no heap) de todo Usuario/Servico/
/// Profissional/Agendamento cadastrado no salao. os quatro cadastros ficam
/// em vetores (nao em mapas) pra permitir expor iteradores diretos sobre
/// eles, igual o ModelBody faz com systems/flows.
class BellezaSystemBody : public Body {
public:
    BellezaSystemBody() = default;

    void add(Usuario* usuario);
    void add(Servico* servico);
    void add(Profissional* profissional);
    void add(Agendamento* agendamento);

    /// buscam uma entidade pelo id (busca linear) e lancam excecao se ela nao existir
    Usuario* usuarioObrigatorio(const std::string& id) const;
    Servico* servicoObrigatorio(const std::string& id) const;
    Profissional* profissionalObrigatorio(const std::string& id) const;
    Agendamento* agendamentoObrigatorio(const std::string& id) const;

    /// confere se o profissional atende o servico, esta dentro do
    /// expediente e nao tem conflito de horario com outro agendamento ativo
    bool profissionalDisponivel(const std::string& profissionalId, const std::string& servicoId, DateTime inicio, const std::string& ignorarAgendamentoId = "") const;

    std::vector<Usuario*> usuarios;
    std::vector<Servico*> servicos;
    std::vector<Profissional*> profissionais;
    std::vector<Agendamento*> agendamentos;
    FinanceiroHandle financeiro;
    int proximoAgendamento = 1;

    /// resultados das ultimas consultas filtradas, usados pelos pares
    /// Begin()/End() do Handle (ver comentario na interface BellezaSystem)
    std::vector<Profissional*> profissionaisDisponiveisCache;
    std::vector<Agendamento*> agendamentosDoProfissionalCache;
    std::vector<Agendamento*> agendamentosDoClienteCache;

    /// deleta todas as entidades que o sistema possui
    virtual ~BellezaSystemBody();

    friend class BellezaSystem;
    friend class BellezaSystemHandle;
    friend class Unit_BellezaSystem;
};

/// classe Handle que implementa a interface BellezaSystem, delegando os
/// dados reais para um BellezaSystemBody (pImpl_)
class BellezaSystemHandle : public BellezaSystem, public Handle<BellezaSystemBody> {

protected:
    void add(Usuario* usuario) override;
    void add(Servico* servico) override;
    void add(Profissional* profissional) override;
    void add(Agendamento* agendamento) override;

public:
    BellezaSystemHandle();

    Usuario* cadastrarUsuario(std::string id, std::string nome, std::string email, std::string senha, Papel papel) override;
    bool login(const std::string& email, const std::string& senha) const override;

    Servico* cadastrarServico(std::string id, std::string nome, double preco, std::chrono::minutes duracao, double percentualComissao) override;

    Profissional* cadastrarProfissional(std::string id, std::string nome, std::string email, std::vector<std::string> servicosAtendidos, int expedienteInicioHora, int expedienteFimHora) override;

    Agendamento* agendar(const std::string& clienteId, const std::string& profissionalId, const std::string& servicoId, DateTime inicio) override;
    Agendamento* remarcarAgendamento(const std::string& agendamentoId, DateTime novoInicio) override;
    Agendamento* cancelarAgendamento(const std::string& agendamentoId) override;
    Agendamento* concluirAgendamento(const std::string& agendamentoId) override;

    std::vector<Profissional*>::iterator profissionaisDisponiveisBegin(const std::string& servicoId, DateTime inicio) override;
    std::vector<Profissional*>::iterator profissionaisDisponiveisEnd() override;

    std::vector<Agendamento*>::iterator agendamentosDoProfissionalBegin(const std::string& profissionalId) override;
    std::vector<Agendamento*>::iterator agendamentosDoProfissionalEnd() override;

    std::vector<Agendamento*>::iterator agendamentosDoClienteBegin(const std::string& clienteId) override;
    std::vector<Agendamento*>::iterator agendamentosDoClienteEnd() override;

    std::vector<Usuario*>::iterator usuariosBegin() override;
    std::vector<Usuario*>::iterator usuariosEnd() override;

    std::vector<Servico*>::iterator servicosBegin() override;
    std::vector<Servico*>::iterator servicosEnd() override;

    std::vector<Profissional*>::iterator profissionaisBegin() override;
    std::vector<Profissional*>::iterator profissionaisEnd() override;

    std::vector<Agendamento*>::iterator agendamentosBegin() override;
    std::vector<Agendamento*>::iterator agendamentosEnd() override;

    const Financeiro& financeiro() const override;

    ~BellezaSystemHandle() override = default;

    friend class BellezaSystem;
    friend class Unit_BellezaSystem;
};

} // namespace bellezasys
