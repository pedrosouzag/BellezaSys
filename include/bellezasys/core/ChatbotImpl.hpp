#pragma once

#include "bellezasys/core/BellezaSystem.hpp"
#include "bellezasys/core/Chatbot.hpp"
#include "bellezasys/core/HandleBody.hpp"
#include "bellezasys/core/LlmClient.hpp"

#include <string>
#include <vector>

namespace bellezasys {

/// deixa o texto em minusculas, tira acentos e pontuacao e colapsa espacos.
/// usado para casar o que o cliente escreveu com o nome dos servicos
/// cadastrados ("Corte Feminino" casa com "corte feminino")
std::string normalizarTexto(const std::string& texto);

/// le o valor de um campo string de um JSON simples e plano, do tipo
/// {"intencao":"agendar","servico":"corte"}. devolve string vazia quando o
/// campo nao existe. nao e um parser completo de JSON: cobre so o formato
/// que pedimos ao modelo, para nao trazer dependencia externa pro core
std::string extrairCampoJson(const std::string& json, const std::string& campo);

/// converte uma data no formato ISO "AAAA-MM-DDTHH:MM" para DateTime.
/// devolve false quando o texto nao esta nesse formato
bool parseDataIso(const std::string& texto, DateTime& saida);

/// classe Body responsavel por guardar os dados reais de um Chatbot
class ChatbotBody : public Body {
public:
    ChatbotBody() = default;

    /// system e obrigatorio (o assistente consulta a agenda real por ele);
    /// llm e opcional: sem ele, o assistente roda so por palavra-chave
    ChatbotBody(BellezaSystem* system, LlmClient* llm);

    /// monta o prompt de classificacao com o catalogo real de servicos
    std::string montarPrompt(const std::string& mensagem) const;

    /// interpretacao por palavra-chave, usada quando nao ha modelo
    /// disponivel ou quando a chamada ao modelo falha
    InterpretacaoChat interpretarPorPalavraChave(const std::string& mensagem) const;

    /// traduz o JSON devolvido pelo modelo em uma InterpretacaoChat
    InterpretacaoChat interpretarRespostaModelo(const std::string& json) const;

    /// procura um servico cadastrado cujo nome case com o texto informado;
    /// devolve nullptr quando nao encontra
    Servico* resolverServico(const std::string& texto) const;

    // executores: cada um responde uma intencao consultando o core
    std::string executarListarServicos() const;
    std::string executarConsultarDisponibilidade(const InterpretacaoChat& interpretacao, const std::string& clienteId) const;
    std::string executarMeusAgendamentos(const std::string& clienteId) const;
    std::string executarCancelar(const InterpretacaoChat& interpretacao, const std::string& clienteId) const;
    std::string textoDeAjuda() const;

    void registrar(AutorMensagem autor, const std::string& texto);

    BellezaSystem* system = nullptr;
    LlmClient* llm = nullptr;
    std::vector<MensagemChat> historico;

    /// data usada como referencia quando o cliente nao diz o dia
    DateTime referencia = DateTime{};

    friend class Unit_Chatbot;
};

/// classe Handle que implementa a interface Chatbot, delegando os dados
/// reais para um ChatbotBody (pImpl_)
class ChatbotHandle : public Chatbot, public Handle<ChatbotBody> {
public:
    ChatbotHandle();
    ChatbotHandle(BellezaSystem* system, LlmClient* llm);

    std::string responder(const std::string& mensagem, const std::string& clienteId) override;
    InterpretacaoChat interpretar(const std::string& mensagem) override;

    std::vector<MensagemChat>::const_iterator historicoBegin() const override;
    std::vector<MensagemChat>::const_iterator historicoEnd() const override;
    void limparHistorico() override;

    bool usandoModelo() const override;
    std::string descricaoMotor() const override;

    /// define a data de referencia usada quando o cliente nao informa o dia
    /// (por padrao, o relogio do sistema). deixar isso explicito e o que
    /// permite testar o assistente de forma deterministica
    void setReferencia(DateTime referencia);

    ~ChatbotHandle() override = default;

    friend class Unit_Chatbot;
};

} // namespace bellezasys
