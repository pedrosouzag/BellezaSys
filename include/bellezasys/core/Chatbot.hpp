#pragma once

#include "bellezasys/core/Types.hpp"

#include <string>
#include <vector>

namespace bellezasys {

/// quem escreveu uma mensagem da conversa
enum class AutorMensagem {
    Cliente,
    Assistente
};

/// uma linha da conversa entre o cliente e o assistente
struct MensagemChat {
    AutorMensagem autor = AutorMensagem::Cliente;
    std::string texto;
    DateTime instante;
};

/// intencao que o assistente identificou na frase do cliente
enum class IntencaoChat {
    Saudacao,
    ListarServicos,
    ConsultarDisponibilidade,
    MeusAgendamentos,
    Cancelar,
    Ajuda,
    Desconhecida
};

/// resultado da interpretacao de uma frase: a intencao e os parametros que
/// foram extraidos dela (ainda em texto cru, do jeito que o modelo devolveu)
struct InterpretacaoChat {
    IntencaoChat intencao = IntencaoChat::Desconhecida;
    std::string servicoTexto;
    std::string dataTexto;
    std::string agendamentoId;

    /// true quando quem interpretou foi o modelo de linguagem; false quando
    /// o Chatbot caiu no interpretador por palavra-chave
    bool veioDoModelo = false;
};

/// converte a intencao para um texto legivel
std::string toString(IntencaoChat intencao);

/// interface do assistente virtual do salao. traduz frases em linguagem
/// natural para consultas no BellezaSystem.
///
/// o modelo de linguagem NUNCA decide se existe horario livre: ele so
/// classifica a frase do cliente numa IntencaoChat e extrai parametros. quem
/// responde sobre agenda, servicos e disponibilidade e sempre o core,
/// consultando os dados reais. isso evita que o modelo invente horario.
///
/// os dados reais ficam em ChatbotBody (ver ChatbotImpl.hpp)
class Chatbot {
public:
    /// processa uma frase do cliente e devolve a resposta do assistente,
    /// registrando as duas pontas no historico
    virtual std::string responder(const std::string& mensagem, const std::string& clienteId) = 0;

    /// interpreta a frase sem executar nada nem mexer no historico; exposto
    /// para os testes e para depuracao
    virtual InterpretacaoChat interpretar(const std::string& mensagem) = 0;

    virtual std::vector<MensagemChat>::const_iterator historicoBegin() const = 0;
    virtual std::vector<MensagemChat>::const_iterator historicoEnd() const = 0;
    virtual void limparHistorico() = 0;

    /// true quando existe um LlmClient ligado e disponivel; false quando o
    /// assistente esta rodando so com palavras-chave
    virtual bool usandoModelo() const = 0;

    /// descreve o motor em uso, para mostrar na interface
    virtual std::string descricaoMotor() const = 0;

    virtual ~Chatbot() = default;
};

} // namespace bellezasys
