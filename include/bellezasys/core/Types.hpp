#pragma once

#include <chrono>
#include <string>

namespace bellezasys {

/// alias para representar um instante no tempo (data e hora) em todo o
/// sistema, usando o relogio do sistema operacional
using DateTime = std::chrono::system_clock::time_point;

/// papel de um usuario dentro do salao: define o que ele pode fazer
enum class Papel {
    Cliente,
    Funcionario,
    Administrador
};

/// acao controlada por perfil. o papel do usuario da sessao decide quais
/// destas ele pode executar (ver BellezaSystem::temPermissao)
enum class Permissao {
    /// criar servicos, profissionais e usuarios
    GerenciarCadastros,
    /// mexer no cadastro e nas preferencias de outros clientes
    GerenciarClientes,
    /// criar, remarcar ou cancelar agendamento de outra pessoa
    AgendarParaTerceiros,
    /// marcar um atendimento como concluido (gera caixa e comissao)
    ConcluirAtendimento,
    /// consultar caixa, comissoes e relatorios
    VerFinanceiro
};

/// converte a permissao para um texto legivel
std::string toString(Permissao permissao);

/// situacao atual de um agendamento ao longo do seu ciclo de vida
enum class StatusAgendamento {
    Agendado,
    Remarcado,
    Cancelado,
    Concluido
};

/// converte o papel do usuario para um texto legivel
std::string toString(Papel papel);

/// converte o status do agendamento para um texto legivel
std::string toString(StatusAgendamento status);

/// monta um DateTime a partir de ano, mes, dia, hora e minuto (horario local)
DateTime makeDateTime(int year, int month, int day, int hour, int minute);

/// formata um DateTime no padrao "dd/mm/aaaa hh:mm"
std::string formatDateTime(DateTime value);

/// verifica se dois intervalos de tempo (inicio + duracao) se sobrepoem;
/// usado para detectar conflito de horario entre agendamentos
bool intervalosColidem(DateTime aInicio, std::chrono::minutes aDuracao, DateTime bInicio, std::chrono::minutes bDuracao);

/// retorna a hora (0-23) de um DateTime no horario local
int localHour(DateTime value);

/// retorna o minuto (0-59) de um DateTime no horario local
int localMinute(DateTime value);

} // namespace bellezasys
