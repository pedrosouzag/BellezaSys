#pragma once

#include <deque>
#include <string>

namespace bellezasys {

/// interface de um modelo de linguagem usado pelo Chatbot. o nucleo nao
/// sabe se por tras existe um servidor Ollama, uma API remota ou um objeto
/// falso de teste: so pede uma completacao de texto e recebe texto de
/// volta. isso mantem bellezasys_core sem Qt e sem rede.
class LlmClient {
public:
    /// envia o prompt para o modelo e devolve a resposta crua (texto).
    /// deve lancar excecao se a comunicacao falhar
    virtual std::string completar(const std::string& prompt) = 0;

    /// indica se o modelo pode ser usado agora; quando false, o Chatbot
    /// cai no interpretador por palavra-chave
    virtual bool disponivel() const = 0;

    /// nome do modelo, so para exibir na interface
    virtual std::string nomeModelo() const = 0;

    virtual ~LlmClient() = default;
};

/// implementacao falsa de LlmClient usada nos testes unitarios. devolve as
/// respostas que foram enfileiradas com enfileirar(), na ordem, o que deixa
/// o comportamento do Chatbot totalmente deterministico.
class FakeLlmClient : public LlmClient {
public:
    FakeLlmClient() = default;

    /// adiciona uma resposta no fim da fila
    void enfileirar(std::string resposta);

    /// liga/desliga o cliente para exercitar o caminho de fallback
    void setDisponivel(bool disponivel);

    /// ultimo prompt recebido, para os testes conferirem o que foi montado
    const std::string& ultimoPrompt() const;

    /// quantas respostas ainda restam na fila
    std::size_t pendentes() const;

    std::string completar(const std::string& prompt) override;
    bool disponivel() const override;
    std::string nomeModelo() const override;

    ~FakeLlmClient() override = default;

private:
    std::deque<std::string> respostas_;
    std::string ultimoPrompt_;
    bool disponivel_ = true;
};

} // namespace bellezasys
