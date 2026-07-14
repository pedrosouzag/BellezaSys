#pragma once

#include "bellezasys/core/Agendamento.hpp"
#include "bellezasys/core/Financeiro.hpp"
#include "bellezasys/core/Profissional.hpp"
#include "bellezasys/core/Servico.hpp"
#include "bellezasys/core/Types.hpp"
#include "bellezasys/core/Usuario.hpp"

#include <chrono>
#include <string>
#include <vector>

namespace bellezasys {

/// orquestra o modelo de dominio do BellezaSys.
///
/// guarda todo Usuario/Servico/Profissional/Agendamento cadastrado no
/// salao e garante as regras de agendamento (expediente, conflito de
/// horario, papel do cliente). as instancias sao controladas por um
/// registro global, do mesmo jeito que Model faz com System/Flow nos
/// exemplos da disciplina. use a fabrica estatica createModel() para criar
/// e registrar um sistema, e deleteModel() para desregistrar e destruir.
///
/// nenhum metodo retorna uma colecao inteira por valor: toda consulta que
/// devolve varios itens expoe um par Begin()/End() de iterador, igual ao
/// systemsBegin()/systemsEnd() do Model.
class BellezaSystem {

private:
    /// registro global de todas as instancias de BellezaSystem ativas
    static std::vector<BellezaSystem*> models;

    /// registra um sistema no registro global
    static void addModel(BellezaSystem* model);

protected:
    virtual void add(Usuario* usuario) = 0;
    virtual void add(Servico* servico) = 0;
    virtual void add(Profissional* profissional) = 0;
    virtual void add(Agendamento* agendamento) = 0;

public:
    virtual Usuario* cadastrarUsuario(std::string id, std::string nome, std::string email, std::string senha, Papel papel) = 0;

    /// confere email e senha contra os usuarios cadastrados
    virtual bool login(const std::string& email, const std::string& senha) const = 0;

    virtual Servico* cadastrarServico(std::string id, std::string nome, double preco, std::chrono::minutes duracao, double percentualComissao) = 0;

    virtual Profissional* cadastrarProfissional(std::string id, std::string nome, std::string email, std::vector<std::string> servicosAtendidos, int expedienteInicioHora, int expedienteFimHora) = 0;

    /// cria um novo agendamento; lanca excecao se o cliente nao for do
    /// papel Cliente ou se o profissional nao estiver disponivel no horario
    virtual Agendamento* agendar(const std::string& clienteId, const std::string& profissionalId, const std::string& servicoId, DateTime inicio) = 0;
    virtual Agendamento* remarcarAgendamento(const std::string& agendamentoId, DateTime novoInicio) = 0;
    virtual Agendamento* cancelarAgendamento(const std::string& agendamentoId) = 0;
    virtual Agendamento* concluirAgendamento(const std::string& agendamentoId) = 0;

    /// consulta os profissionais que atendem o servico e estao livres no
    /// horario informado; profissionaisDisponiveisBegin() recalcula a
    /// consulta, profissionaisDisponiveisEnd() so devolve o fim do
    /// resultado ja calculado (chame sempre Begin() antes de End())
    virtual std::vector<Profissional*>::iterator profissionaisDisponiveisBegin(const std::string& servicoId, DateTime inicio) = 0;
    virtual std::vector<Profissional*>::iterator profissionaisDisponiveisEnd() = 0;

    virtual std::vector<Agendamento*>::iterator agendamentosDoProfissionalBegin(const std::string& profissionalId) = 0;
    virtual std::vector<Agendamento*>::iterator agendamentosDoProfissionalEnd() = 0;

    /// filtra a agenda de um profissional para o mesmo dia da data informada
    virtual std::vector<Agendamento*>::iterator agendaDoProfissionalNoDiaBegin(const std::string& profissionalId, DateTime dia) = 0;
    virtual std::vector<Agendamento*>::iterator agendaDoProfissionalNoDiaEnd() = 0;

    virtual std::vector<Agendamento*>::iterator agendamentosDoClienteBegin(const std::string& clienteId) = 0;
    virtual std::vector<Agendamento*>::iterator agendamentosDoClienteEnd() = 0;

    virtual std::vector<Usuario*>::iterator usuariosBegin() = 0;
    virtual std::vector<Usuario*>::iterator usuariosEnd() = 0;

    virtual std::vector<Servico*>::iterator servicosBegin() = 0;
    virtual std::vector<Servico*>::iterator servicosEnd() = 0;

    virtual std::vector<Profissional*>::iterator profissionaisBegin() = 0;
    virtual std::vector<Profissional*>::iterator profissionaisEnd() = 0;

    virtual std::vector<Agendamento*>::iterator agendamentosBegin() = 0;
    virtual std::vector<Agendamento*>::iterator agendamentosEnd() = 0;

    virtual const Financeiro& financeiro() const = 0;

    /// persistencia simples em arquivo texto para o prototipo
    virtual void salvarEmArquivo(const std::string& caminho) const = 0;
    virtual void carregarDeArquivo(const std::string& caminho) = 0;

    /// metodo de fabrica: cria, registra e retorna um novo BellezaSystemHandle
    static BellezaSystem* createModel();

    /// remove um sistema do registro global e o destroi (junto com todo
    /// Usuario/Servico/Profissional/Agendamento que ele possui)
    static void deleteModel(BellezaSystem* model);

    virtual ~BellezaSystem() = default;

    friend class BellezaSystemHandle;
    friend class Unit_BellezaSystem;
};

} // namespace bellezasys
