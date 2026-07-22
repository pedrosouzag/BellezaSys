#pragma once

#include "bellezasys/core/LlmClient.hpp"

#include <QString>

#include <string>

class QNetworkAccessManager;

/// implementacao de bellezasys::LlmClient que fala com um servidor Ollama
/// rodando na propria maquina (por padrao http://localhost:11434).
///
/// fica em src/gui de proposito: assim a biblioteca bellezasys_core
/// continua sem nenhuma dependencia de Qt nem de rede, e os testes
/// unitarios do nucleo seguem deterministicos usando FakeLlmClient.
///
/// para usar, instale o Ollama e baixe um modelo pequeno:
///
///     ollama pull llama3.2:3b
///     ollama serve
class OllamaLlmClient : public bellezasys::LlmClient {
public:
    explicit OllamaLlmClient(QString modelo = QStringLiteral("llama3.2:3b"),
        QString host = QStringLiteral("http://localhost:11434"));
    ~OllamaLlmClient() override;

    /// consulta /api/tags para saber se o servidor esta no ar e se o modelo
    /// configurado foi baixado. o resultado fica em cache; chame
    /// verificarServidor() para checar de novo
    bool disponivel() const override;

    /// refaz a checagem do servidor e devolve o resultado
    bool verificarServidor();

    /// ultima mensagem de erro da checagem ou da geracao, para exibir na
    /// interface quando o assistente cair para o modo palavras-chave
    QString ultimoErro() const;

    /// envia o prompt para /api/generate e devolve o campo "response".
    /// lanca std::runtime_error quando a chamada falha ou estoura o tempo
    std::string completar(const std::string& prompt) override;

    std::string nomeModelo() const override;

private:
    /// executa a requisicao bloqueando com um QEventLoop proprio e um
    /// timeout, para caber no fluxo sincrono do Chatbot
    QByteArray postarBloqueando(const QByteArray& corpo, int timeoutMs, QString& erro) const;

    QNetworkAccessManager* manager_ = nullptr;
    QString modelo_;
    QString host_;
    mutable QString ultimoErro_;
    mutable bool disponivel_ = false;
};
