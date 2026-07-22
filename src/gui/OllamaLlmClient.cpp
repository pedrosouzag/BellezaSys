#include "OllamaLlmClient.hpp"

#include <QByteArray>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>

#include <stdexcept>

namespace {

/// tempo maximo esperando o modelo responder. a primeira geracao costuma
/// demorar mais porque o Ollama ainda esta carregando os pesos na memoria
constexpr int kTimeoutGeracaoMs = 60000;

/// a checagem do servidor e barata, entao pode falhar rapido
constexpr int kTimeoutChecagemMs = 2000;

} // namespace

OllamaLlmClient::OllamaLlmClient(QString modelo, QString host)
    : manager_(new QNetworkAccessManager())
    , modelo_(std::move(modelo))
    , host_(std::move(host))
{
    verificarServidor();
}

OllamaLlmClient::~OllamaLlmClient()
{
    delete manager_;
}

bool OllamaLlmClient::disponivel() const { return disponivel_; }

QString OllamaLlmClient::ultimoErro() const { return ultimoErro_; }

std::string OllamaLlmClient::nomeModelo() const
{
    return QStringLiteral("Ollama %1").arg(modelo_).toStdString();
}

bool OllamaLlmClient::verificarServidor()
{
    QNetworkRequest request{ QUrl(host_ + QStringLiteral("/api/tags")) };

    QEventLoop loop;
    QNetworkReply* reply = manager_->get(request);
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start(kTimeoutChecagemMs);
    loop.exec();

    if (!reply->isFinished()) {
        reply->abort();
        reply->deleteLater();
        disponivel_ = false;
        ultimoErro_ = QStringLiteral("Servidor Ollama nao respondeu em %1 ms.").arg(kTimeoutChecagemMs);
        return false;
    }

    if (reply->error() != QNetworkReply::NoError) {
        ultimoErro_ = QStringLiteral("Nao consegui falar com o Ollama em %1: %2")
                          .arg(host_, reply->errorString());
        reply->deleteLater();
        disponivel_ = false;
        return false;
    }

    const QByteArray corpo = reply->readAll();
    reply->deleteLater();

    // confere se o modelo configurado esta entre os que ja foram baixados
    const QJsonObject raiz = QJsonDocument::fromJson(corpo).object();
    const QJsonArray modelos = raiz.value(QStringLiteral("models")).toArray();
    for (const QJsonValue& valor : modelos) {
        const QString nome = valor.toObject().value(QStringLiteral("name")).toString();
        // o Ollama devolve "llama3.2:3b"; aceita tambem quem configurou sem a tag
        if (nome == modelo_ || nome.section(QLatin1Char(':'), 0, 0) == modelo_) {
            disponivel_ = true;
            ultimoErro_.clear();
            return true;
        }
    }

    disponivel_ = false;
    ultimoErro_ = QStringLiteral("Servidor no ar, mas o modelo \"%1\" nao foi baixado. Rode: ollama pull %1")
                      .arg(modelo_);
    return false;
}

QByteArray OllamaLlmClient::postarBloqueando(const QByteArray& corpo, int timeoutMs, QString& erro) const
{
    QNetworkRequest request{ QUrl(host_ + QStringLiteral("/api/generate")) };
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

    QEventLoop loop;
    QNetworkReply* reply = manager_->post(request, corpo);
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start(timeoutMs);
    loop.exec();

    if (!reply->isFinished()) {
        reply->abort();
        reply->deleteLater();
        erro = QStringLiteral("O modelo nao respondeu em %1 ms.").arg(timeoutMs);
        return {};
    }

    if (reply->error() != QNetworkReply::NoError) {
        erro = reply->errorString();
        reply->deleteLater();
        return {};
    }

    const QByteArray resposta = reply->readAll();
    reply->deleteLater();
    erro.clear();
    return resposta;
}

std::string OllamaLlmClient::completar(const std::string& prompt)
{
    QJsonObject corpo;
    corpo[QStringLiteral("model")] = modelo_;
    corpo[QStringLiteral("prompt")] = QString::fromStdString(prompt);
    corpo[QStringLiteral("stream")] = false;
    // pede ao Ollama para restringir a saida a JSON valido, que e o formato
    // que ChatbotBody::interpretarRespostaModelo() espera
    corpo[QStringLiteral("format")] = QStringLiteral("json");

    QJsonObject opcoes;
    // temperatura baixa deixa a classificacao mais estavel entre execucoes
    opcoes[QStringLiteral("temperature")] = 0.1;
    opcoes[QStringLiteral("num_predict")] = 200;
    corpo[QStringLiteral("options")] = opcoes;

    QString erro;
    const QByteArray resposta = postarBloqueando(QJsonDocument(corpo).toJson(QJsonDocument::Compact),
        kTimeoutGeracaoMs, erro);

    if (!erro.isEmpty()) {
        ultimoErro_ = erro;
        disponivel_ = false; // proxima mensagem ja cai no modo palavras-chave
        throw std::runtime_error(erro.toStdString());
    }

    const QJsonObject raiz = QJsonDocument::fromJson(resposta).object();
    const QString texto = raiz.value(QStringLiteral("response")).toString();
    if (texto.isEmpty()) {
        ultimoErro_ = QStringLiteral("Resposta vazia do modelo.");
        throw std::runtime_error(ultimoErro_.toStdString());
    }

    ultimoErro_.clear();
    return texto.toStdString();
}
