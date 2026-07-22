#include "bellezasys/core/Types.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace bellezasys {

// converte o papel do usuario para um texto legivel
std::string toString(Papel papel)
{
    switch (papel) {
    case Papel::Cliente:
        return "Cliente";
    case Papel::Funcionario:
        return "Funcionario";
    case Papel::Administrador:
        return "Administrador";
    }
    return "Desconhecido";
}

// converte a permissao para um texto legivel, usado nas mensagens de erro
std::string toString(Permissao permissao)
{
    switch (permissao) {
    case Permissao::GerenciarCadastros:
        return "gerenciar cadastros";
    case Permissao::GerenciarClientes:
        return "gerenciar clientes";
    case Permissao::AgendarParaTerceiros:
        return "agendar para outra pessoa";
    case Permissao::ConcluirAtendimento:
        return "concluir atendimento";
    case Permissao::VerFinanceiro:
        return "ver o financeiro";
    }
    return "acao desconhecida";
}

// converte o status do agendamento para um texto legivel
std::string toString(StatusAgendamento status)
{
    switch (status) {
    case StatusAgendamento::Agendado:
        return "Agendado";
    case StatusAgendamento::Remarcado:
        return "Remarcado";
    case StatusAgendamento::Cancelado:
        return "Cancelado";
    case StatusAgendamento::Concluido:
        return "Concluido";
    }
    return "Desconhecido";
}

// monta um DateTime a partir de ano, mes, dia, hora e minuto (horario local)
DateTime makeDateTime(int year, int month, int day, int hour, int minute)
{
    std::tm localTime = {};
    localTime.tm_year = year - 1900;
    localTime.tm_mon = month - 1;
    localTime.tm_mday = day;
    localTime.tm_hour = hour;
    localTime.tm_min = minute;
    localTime.tm_sec = 0;
    localTime.tm_isdst = -1; // deixa o sistema decidir sobre horario de verao

    const std::time_t raw = std::mktime(&localTime);
    if (raw == -1) {
        throw std::invalid_argument("Data e hora invalidas.");
    }

    return std::chrono::system_clock::from_time_t(raw);
}

// formata um DateTime no padrao "dd/mm/aaaa hh:mm"
std::string formatDateTime(DateTime value)
{
    const std::time_t raw = std::chrono::system_clock::to_time_t(value);
    std::tm localTime = {};

    // localtime_s (windows) e localtime_r (linux/mac) fazem a mesma coisa:
    // convertem o timestamp para hora local de forma thread-safe
#if defined(_WIN32)
    localtime_s(&localTime, &raw);
#else
    localtime_r(&raw, &localTime);
#endif

    std::ostringstream output;
    output << std::put_time(&localTime, "%d/%m/%Y %H:%M");
    return output.str();
}

// verifica se dois intervalos de tempo (inicio + duracao) se sobrepoem
bool intervalosColidem(DateTime aInicio, std::chrono::minutes aDuracao, DateTime bInicio, std::chrono::minutes bDuracao)
{
    const DateTime aFim = aInicio + aDuracao;
    const DateTime bFim = bInicio + bDuracao;
    // dois intervalos colidem se um comeca antes do outro terminar, nos
    // dois sentidos
    return aInicio < bFim && bInicio < aFim;
}

// retorna a hora (0-23) de um DateTime no horario local
int localHour(DateTime value)
{
    const std::time_t raw = std::chrono::system_clock::to_time_t(value);
    std::tm localTime = {};

#if defined(_WIN32)
    localtime_s(&localTime, &raw);
#else
    localtime_r(&raw, &localTime);
#endif

    return localTime.tm_hour;
}

// retorna o minuto (0-59) de um DateTime no horario local
int localMinute(DateTime value)
{
    const std::time_t raw = std::chrono::system_clock::to_time_t(value);
    std::tm localTime = {};

#if defined(_WIN32)
    localtime_s(&localTime, &raw);
#else
    localtime_r(&raw, &localTime);
#endif

    return localTime.tm_min;
}

} // namespace bellezasys
