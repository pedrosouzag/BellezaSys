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
