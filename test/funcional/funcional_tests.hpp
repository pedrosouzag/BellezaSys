#ifndef FUNCIONAL_TESTS_HPP
#define FUNCIONAL_TESTS_HPP

#include <iostream>
#include <cassert>
#include <cmath>

using namespace std;

/// testes de integracao que validam cenarios completos de uso do BellezaSys.
///
/// cada teste monta um BellezaSystem com clientes, servicos e profissionais
/// reais, exercita a API publica de ponta a ponta (agendar, remarcar,
/// cancelar, concluir) e confere o estado final do sistema

/// teste funcional da jornada completa do cliente.
///
/// cenario (BDD):
/// - GIVEN um cliente, um servico e um profissional cadastrados no salao
/// - WHEN o cliente agenda um horario, remarca, e o administrador conclui
///   o atendimento
/// - THEN o agendamento fica Concluido e o caixa/comissao refletem o
///   preco do servico e o percentual de comissao
void jornadaClienteFuncionalTest();

/// teste funcional de conflito de agenda.
///
/// cenario (BDD):
/// - GIVEN um profissional com um agendamento ativo as 10:00
/// - WHEN outro agendamento e pedido pro mesmo profissional num horario
///   que se sobrepoe
/// - THEN o sistema recusa (logic_error), mas o mesmo profissional num
///   horario livre, ou outro profissional disponivel no mesmo horario,
///   funcionam normalmente
void conflitoDeAgendaFuncionalTest();

/// teste funcional de um dia completo do salao, com varios clientes,
/// profissionais e servicos.
///
/// cenario (BDD):
/// - GIVEN 2 clientes, 2 profissionais e 2 servicos cadastrados
/// - WHEN cada cliente agenda um horario, um agendamento e cancelado e o
///   outro e concluido
/// - THEN cada cliente ve so o proprio agendamento, cada profissional ve
///   so a propria agenda, e o caixa/comissao batem apenas com o
///   agendamento concluido
void cenarioCompletoSalaoFuncionalTest();

#endif
