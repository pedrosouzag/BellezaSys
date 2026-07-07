#include "funcional_tests.hpp"

#include "bellezasys/core/AgendaService.hpp"

#include <chrono>
#include <iterator>
#include <stdexcept>

using namespace bellezasys;

// GIVEN um cliente, um servico e um profissional cadastrados no salao.
// WHEN o cliente agenda, remarca, e o administrador conclui o atendimento.
// THEN o agendamento fica Concluido e o caixa/comissao batem com o preco
// do servico (80.0) e a comissao de 35%.
void jornadaClienteFuncionalTest() {

    BellezaSystem* salao = BellezaSystem::createModel();

    salao->cadastrarUsuario("CLI-1", "Marina", "marina@email.com", "123", Papel::Cliente);
    salao->cadastrarServico("SER-CORTE", "Corte", 80.0, std::chrono::minutes(45), 0.35);
    salao->cadastrarProfissional("PRO-1", "Ana", "ana@belleza.com", {"SER-CORTE"}, 9, 18);

    // GIVEN o cliente autenticado
    assert(salao->login("marina@email.com", "123"));

    // WHEN o cliente agenda um horario disponivel
    const DateTime horario = makeDateTime(2026, 7, 7, 10, 0);
    Agendamento* agendamento = salao->agendar("CLI-1", "PRO-1", "SER-CORTE", horario);
    assert(agendamento->status() == StatusAgendamento::Agendado);

    // WHEN o cliente remarca para outro horario livre
    const DateTime novoHorario = makeDateTime(2026, 7, 7, 15, 0);
    salao->remarcarAgendamento(agendamento->id(), novoHorario);
    assert(agendamento->status() == StatusAgendamento::Remarcado);
    assert(agendamento->inicio() == novoHorario);

    // WHEN o atendimento e concluido
    salao->concluirAgendamento(agendamento->id());

    // THEN o agendamento esta concluido e o financeiro foi atualizado
    assert(agendamento->status() == StatusAgendamento::Concluido);
    assert(salao->financeiro().saldo() == 80.0 - 80.0 * 0.35);
    assert(salao->financeiro().totalComissoes() == 80.0 * 0.35);

    cout << "Jornada do cliente: teste funcional passou!" << endl;

    BellezaSystem::deleteModel(salao);
}

// GIVEN um profissional com um agendamento ativo as 10:00 (45 min).
// WHEN outro agendamento que se sobrepoe a esse horario e pedido pro
// mesmo profissional.
// THEN e recusado; o mesmo profissional num horario livre, ou outro
// profissional disponivel no mesmo horario, funcionam normalmente.
void conflitoDeAgendaFuncionalTest() {

    BellezaSystem* salao = BellezaSystem::createModel();

    salao->cadastrarUsuario("CLI-1", "Marina", "marina@email.com", "123", Papel::Cliente);
    salao->cadastrarUsuario("CLI-2", "Joao", "joao@email.com", "123", Papel::Cliente);
    salao->cadastrarServico("SER-CORTE", "Corte", 80.0, std::chrono::minutes(45), 0.35);
    salao->cadastrarProfissional("PRO-1", "Ana", "ana@belleza.com", {"SER-CORTE"}, 9, 18);
    salao->cadastrarProfissional("PRO-2", "Bia", "bia@belleza.com", {"SER-CORTE"}, 9, 18);

    const DateTime horario = makeDateTime(2026, 7, 7, 10, 0);
    salao->agendar("CLI-1", "PRO-1", "SER-CORTE", horario);

    // WHEN outro cliente tenta o mesmo profissional em horario conflitante
    // THEN a agenda recusa
    bool conflitoDetectado = false;
    try {
        salao->agendar("CLI-2", "PRO-1", "SER-CORTE", makeDateTime(2026, 7, 7, 10, 15));
    } catch (const std::logic_error&) {
        conflitoDetectado = true;
    }
    assert(conflitoDetectado);

    // WHEN o mesmo profissional e escolhido num horario livre
    // THEN o agendamento e aceito normalmente
    Agendamento* emOutroHorario = salao->agendar("CLI-2", "PRO-1", "SER-CORTE", makeDateTime(2026, 7, 7, 14, 0));
    assert(emOutroHorario != nullptr);

    // WHEN outro profissional disponivel e escolhido no mesmo horario do conflito
    // THEN o agendamento tambem e aceito
    Agendamento* comOutroProfissional = salao->agendar("CLI-2", "PRO-2", "SER-CORTE", horario);
    assert(comOutroProfissional != nullptr);

    assert(std::distance(salao->agendamentosBegin(), salao->agendamentosEnd()) == 3);

    cout << "Conflito de agenda: teste funcional passou!" << endl;

    BellezaSystem::deleteModel(salao);
}

// GIVEN 2 clientes, 2 profissionais e 2 servicos.
// WHEN cada cliente agenda um horario, um agendamento e cancelado e o
// outro e concluido.
// THEN cada cliente/profissional ve so os proprios agendamentos, e o
// caixa/comissao refletem apenas o agendamento concluido.
void cenarioCompletoSalaoFuncionalTest() {

    BellezaSystem* salao = BellezaSystem::createModel();

    salao->cadastrarUsuario("CLI-1", "Marina", "marina@email.com", "123", Papel::Cliente);
    salao->cadastrarUsuario("CLI-2", "Joao", "joao@email.com", "123", Papel::Cliente);

    salao->cadastrarServico("SER-CORTE", "Corte", 80.0, std::chrono::minutes(45), 0.35);
    salao->cadastrarServico("SER-MANICURE", "Manicure", 45.0, std::chrono::minutes(40), 0.30);

    salao->cadastrarProfissional("PRO-ANA", "Ana", "ana@belleza.com", {"SER-CORTE"}, 9, 18);
    salao->cadastrarProfissional("PRO-BIA", "Bia", "bia@belleza.com", {"SER-MANICURE"}, 10, 19);

    Agendamento* agendamentoCliente1 = salao->agendar("CLI-1", "PRO-ANA", "SER-CORTE", makeDateTime(2026, 7, 7, 10, 0));
    Agendamento* agendamentoCliente2 = salao->agendar("CLI-2", "PRO-BIA", "SER-MANICURE", makeDateTime(2026, 7, 7, 11, 0));

    // WHEN o agendamento do cliente 1 e cancelado
    salao->cancelarAgendamento(agendamentoCliente1->id());
    assert(agendamentoCliente1->status() == StatusAgendamento::Cancelado);

    // WHEN o agendamento do cliente 2 e concluido
    salao->concluirAgendamento(agendamentoCliente2->id());
    assert(agendamentoCliente2->status() == StatusAgendamento::Concluido);

    // THEN cada cliente ve somente o proprio agendamento
    auto inicioCli1 = salao->agendamentosDoClienteBegin("CLI-1");
    assert(std::distance(inicioCli1, salao->agendamentosDoClienteEnd()) == 1);
    auto inicioCli2 = salao->agendamentosDoClienteBegin("CLI-2");
    assert(std::distance(inicioCli2, salao->agendamentosDoClienteEnd()) == 1);

    // THEN cada profissional ve somente os proprios agendamentos
    auto inicioAna = salao->agendamentosDoProfissionalBegin("PRO-ANA");
    assert(std::distance(inicioAna, salao->agendamentosDoProfissionalEnd()) == 1);
    auto inicioBia = salao->agendamentosDoProfissionalBegin("PRO-BIA");
    assert(std::distance(inicioBia, salao->agendamentosDoProfissionalEnd()) == 1);

    // THEN o financeiro reflete apenas o agendamento concluido (manicure, 45.0, 30%)
    assert(salao->financeiro().saldo() == 45.0 - 45.0 * 0.30);
    assert(salao->financeiro().totalComissoes() == 45.0 * 0.30);

    cout << "Cenario completo do salao: teste funcional passou!" << endl;

    BellezaSystem::deleteModel(salao);
}
