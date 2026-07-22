#include "unit_Chatbot.hpp"
#include "bellezasys/core/ChatbotImpl.hpp"

#include <cassert>
#include <chrono>
#include <string>

namespace bellezasys {

namespace {

/// data fixa usada em todos os testes: 07/07/2026 as 10:00. deixar a
/// referencia explicita e o que permite conferir as respostas do assistente
/// sem depender do relogio da maquina
DateTime referenciaFixa() { return makeDateTime(2026, 7, 7, 10, 0); }

/// monta um salao minimo: um servico, um profissional que o atende e um
/// cliente. quem chama e dono do ponteiro e deve usar deleteModel()
BellezaSystem* montarSalao()
{
    BellezaSystem* system = BellezaSystem::createModel();
    system->cadastrarServico("SER-CORTE", "Corte Feminino", 80.0, std::chrono::minutes(45), 0.35);
    system->cadastrarServico("SER-UNHA", "Manicure", 50.0, std::chrono::minutes(30), 0.40);
    system->cadastrarProfissional("PRO-1", "Ana", "ana@belleza.com", { "SER-CORTE" }, 9, 18);
    system->cadastrarUsuario("CLI-1", "Marina", "marina@email.com", "123", Papel::Cliente);
    return system;
}

/// true quando o texto contem o trecho informado
bool contem(const std::string& texto, const std::string& trecho)
{
    return texto.find(trecho) != std::string::npos;
}

} // namespace

// confere que o construtor padrao deixa o body sem sistema nem modelo
bool Unit_Chatbot::unit_Chatbot_defaultConstructor()
{
    ChatbotHandle chatbot;

    assert(chatbot.pImpl_->system == nullptr);
    assert(chatbot.pImpl_->llm == nullptr);
    assert(chatbot.pImpl_->historico.empty());

    return true;
}

// confere que o construtor parametrizado guarda sistema e modelo
bool Unit_Chatbot::unit_Chatbot_Constructor()
{
    BellezaSystem* system = montarSalao();
    FakeLlmClient llm;

    ChatbotHandle chatbot(system, &llm);

    assert(chatbot.pImpl_->system == system);
    assert(chatbot.pImpl_->llm == &llm);

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que o construtor de copia compartilha o mesmo body (nao duplica)
bool Unit_Chatbot::unit_Chatbot_Copyconstructor()
{
    BellezaSystem* system = montarSalao();
    FakeLlmClient llm;

    ChatbotHandle chatbot1(system, &llm);
    ChatbotHandle chatbot2(chatbot1);

    assert(chatbot2.pImpl_ == chatbot1.pImpl_);
    assert(chatbot2.pImpl_->system == system);

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que o destrutor nao quebra a aplicacao
bool Unit_Chatbot::unit_Chatbot_destructor()
{
    BellezaSystem* system = montarSalao();
    FakeLlmClient llm;

    ChatbotHandle* chatbot = new ChatbotHandle(system, &llm);
    delete chatbot;

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que o operador de atribuicao passa a apontar pro mesmo body
bool Unit_Chatbot::unit_Chatbot_assignmentOperator()
{
    BellezaSystem* system = montarSalao();
    FakeLlmClient llm;

    ChatbotHandle chatbot1(system, &llm);
    ChatbotHandle chatbot2;
    chatbot2 = chatbot1;

    assert(chatbot2.pImpl_ == chatbot1.pImpl_);

    BellezaSystem::deleteModel(system);
    return true;
}

// confere a normalizacao usada para casar o texto do cliente com o catalogo
bool Unit_Chatbot::unit_Chatbot_normalizarTexto()
{
    assert(normalizarTexto("Corte Feminino") == "corte feminino");
    // acentos em UTF-8 viram a letra simples
    assert(normalizarTexto("Manicure e Pedicure\xC3\xA7\xC3\xA3o") == "manicure e pedicurecao");
    // pontuacao some e espacos repetidos colapsam
    assert(normalizarTexto("  Quanto   custa?? ") == "quanto custa");
    assert(normalizarTexto("!!!") == "");

    return true;
}

// confere a leitura dos campos do JSON devolvido pelo modelo
bool Unit_Chatbot::unit_Chatbot_extrairCampoJson()
{
    const std::string json = R"({"intencao":"agendar","servico":"Corte Feminino","data":"","agendamento_id":"AGD-1"})";

    assert(extrairCampoJson(json, "intencao") == "agendar");
    assert(extrairCampoJson(json, "servico") == "Corte Feminino");
    assert(extrairCampoJson(json, "data") == "");
    assert(extrairCampoJson(json, "agendamento_id") == "AGD-1");
    // campo que nao existe devolve vazio em vez de lancar excecao
    assert(extrairCampoJson(json, "inexistente") == "");
    assert(extrairCampoJson("nao e json", "intencao") == "");

    return true;
}

// confere a conversao de data ISO e a rejeicao de formatos invalidos
bool Unit_Chatbot::unit_Chatbot_parseDataIso()
{
    DateTime data;

    assert(parseDataIso("2026-07-07T10:00", data));
    assert(data == makeDateTime(2026, 7, 7, 10, 0));

    // espaco no lugar do "T" tambem e aceito
    assert(parseDataIso("2026-12-31 23:59", data));
    assert(data == makeDateTime(2026, 12, 31, 23, 59));

    assert(!parseDataIso("", data));
    assert(!parseDataIso("07/07/2026 10:00", data));
    assert(!parseDataIso("2026-13-07T10:00", data)); // mes invalido
    assert(!parseDataIso("2026-07-07T25:00", data)); // hora invalida
    assert(!parseDataIso("20xx-07-07T10:00", data)); // nao numerico

    return true;
}

// confere o interpretador de reserva, que roda sem modelo de linguagem
bool Unit_Chatbot::unit_Chatbot_interpretarPorPalavraChave()
{
    BellezaSystem* system = montarSalao();
    ChatbotHandle chatbot(system, nullptr);

    InterpretacaoChat i1 = chatbot.interpretar("bom dia");
    assert(i1.intencao == IntencaoChat::Saudacao);
    assert(!i1.veioDoModelo);

    assert(chatbot.interpretar("quais servicos voces tem?").intencao == IntencaoChat::ListarServicos);
    assert(chatbot.interpretar("quero marcar um horario").intencao == IntencaoChat::ConsultarDisponibilidade);
    assert(chatbot.interpretar("meus agendamentos").intencao == IntencaoChat::MeusAgendamentos);
    assert(chatbot.interpretar("quero cancelar").intencao == IntencaoChat::Cancelar);
    assert(chatbot.interpretar("xyzabc").intencao == IntencaoChat::Desconhecida);

    // o nome do servico e reconhecido dentro da frase, mesmo sem acento
    InterpretacaoChat i2 = chatbot.interpretar("tem horario pra corte feminino?");
    assert(i2.intencao == IntencaoChat::ConsultarDisponibilidade);
    assert(i2.servicoTexto == "Corte Feminino");

    BellezaSystem::deleteModel(system);
    return true;
}

// confere a traducao do JSON do modelo em uma InterpretacaoChat
bool Unit_Chatbot::unit_Chatbot_interpretarRespostaModelo()
{
    BellezaSystem* system = montarSalao();
    FakeLlmClient llm;
    ChatbotHandle chatbot(system, &llm);

    llm.enfileirar(R"({"intencao":"consultar_disponibilidade","servico":"Corte Feminino","data":"2026-07-07T14:00","agendamento_id":""})");

    InterpretacaoChat interpretacao = chatbot.interpretar("tem vaga hoje a tarde?");
    assert(interpretacao.intencao == IntencaoChat::ConsultarDisponibilidade);
    assert(interpretacao.servicoTexto == "Corte Feminino");
    assert(interpretacao.dataTexto == "2026-07-07T14:00");
    assert(interpretacao.veioDoModelo);
    assert(llm.pendentes() == 0);

    // quando o modelo devolve um rotulo fora da lista, as palavras-chave
    // assumem, mas os parametros extraidos pelo modelo sao aproveitados
    llm.enfileirar(R"({"intencao":"blablabla","servico":"","data":"2026-07-08T09:00","agendamento_id":""})");
    InterpretacaoChat reserva = chatbot.interpretar("quero marcar um horario");
    assert(reserva.intencao == IntencaoChat::ConsultarDisponibilidade);
    assert(reserva.dataTexto == "2026-07-08T09:00");

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que um modelo desligado nao e chamado e o assistente segue vivo
bool Unit_Chatbot::unit_Chatbot_interpretarComModeloIndisponivel()
{
    BellezaSystem* system = montarSalao();
    FakeLlmClient llm;
    llm.setDisponivel(false);
    ChatbotHandle chatbot(system, &llm);

    assert(!chatbot.usandoModelo());
    assert(chatbot.descricaoMotor() == "palavras-chave (sem modelo)");

    // nenhuma resposta foi enfileirada: se o modelo fosse chamado, o
    // FakeLlmClient lancaria excecao
    InterpretacaoChat interpretacao = chatbot.interpretar("quais servicos voces tem?");
    assert(interpretacao.intencao == IntencaoChat::ListarServicos);
    assert(!interpretacao.veioDoModelo);
    assert(llm.ultimoPrompt().empty());

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que uma falha do modelo cai no interpretador por palavra-chave
bool Unit_Chatbot::unit_Chatbot_interpretarComFalhaDoModelo()
{
    BellezaSystem* system = montarSalao();
    FakeLlmClient llm; // disponivel, mas com a fila vazia: completar() lanca
    ChatbotHandle chatbot(system, &llm);

    assert(chatbot.usandoModelo());

    InterpretacaoChat interpretacao = chatbot.interpretar("meus agendamentos");
    assert(interpretacao.intencao == IntencaoChat::MeusAgendamentos);
    assert(!interpretacao.veioDoModelo);

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que o prompt leva o catalogo real e a data de referencia
bool Unit_Chatbot::unit_Chatbot_montarPrompt()
{
    BellezaSystem* system = montarSalao();
    FakeLlmClient llm;
    ChatbotHandle chatbot(system, &llm);
    chatbot.setReferencia(referenciaFixa());

    llm.enfileirar(R"({"intencao":"ajuda","servico":"","data":"","agendamento_id":""})");
    chatbot.interpretar("me ajuda");

    const std::string prompt = llm.ultimoPrompt();
    assert(contem(prompt, "Corte Feminino"));
    assert(contem(prompt, "Manicure"));
    assert(contem(prompt, "2026-07-07T10:00"));
    assert(contem(prompt, "me ajuda"));
    // os dias relativos vao calculados no prompt: modelos pequenos erram
    // quando precisam somar um dia sozinhos
    assert(contem(prompt, "\"amanha\" = 2026-07-08"));
    assert(contem(prompt, "\"depois de amanha\" = 2026-07-09"));
    // os exemplos few-shot citam servicos do proprio catalogo, e nao nomes
    // fixos que poderiam nao existir neste salao
    assert(contem(prompt, "tem vaga hoje de manha pra Corte Feminino"));
    assert(contem(prompt, "quanto custa Manicure"));
    // o prompt precisa proibir o modelo de decidir disponibilidade
    assert(contem(prompt, "Nunca afirme que existe horario livre"));

    BellezaSystem::deleteModel(system);
    return true;
}

// confere a resposta com o catalogo de servicos
bool Unit_Chatbot::unit_Chatbot_responderListarServicos()
{
    BellezaSystem* system = montarSalao();
    ChatbotHandle chatbot(system, nullptr);
    chatbot.setReferencia(referenciaFixa());

    const std::string resposta = chatbot.responder("quais servicos voces tem?", "CLI-1");

    assert(contem(resposta, "Corte Feminino"));
    assert(contem(resposta, "R$ 80.00"));
    assert(contem(resposta, "45 min"));
    assert(contem(resposta, "Manicure"));

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que a disponibilidade vem do core, e nao do modelo de linguagem
bool Unit_Chatbot::unit_Chatbot_responderDisponibilidade()
{
    BellezaSystem* system = montarSalao();
    FakeLlmClient llm;
    ChatbotHandle chatbot(system, &llm);
    chatbot.setReferencia(referenciaFixa());

    // dentro do expediente da Ana (9h as 18h): ela aparece como livre
    llm.enfileirar(R"({"intencao":"consultar_disponibilidade","servico":"Corte Feminino","data":"2026-07-07T14:00","agendamento_id":""})");
    const std::string livre = chatbot.responder("tem vaga as 14h?", "CLI-1");
    assert(contem(livre, "Ana"));
    assert(contem(livre, "07/07/2026 14:00"));

    // fora do expediente: o assistente nao pode prometer horario
    llm.enfileirar(R"({"intencao":"consultar_disponibilidade","servico":"Corte Feminino","data":"2026-07-07T23:00","agendamento_id":""})");
    const std::string fechado = chatbot.responder("e as 23h?", "CLI-1");
    assert(contem(fechado, "Nao encontrei profissional livre"));
    assert(!contem(fechado, "Ana"));

    // servico que ninguem atende tambem nao inventa profissional
    llm.enfileirar(R"({"intencao":"consultar_disponibilidade","servico":"Manicure","data":"2026-07-07T14:00","agendamento_id":""})");
    const std::string semProfissional = chatbot.responder("e manicure?", "CLI-1");
    assert(contem(semProfissional, "Nao encontrei profissional livre"));

    // sem servico identificado, o assistente pergunta em vez de chutar
    llm.enfileirar(R"({"intencao":"consultar_disponibilidade","servico":"","data":"","agendamento_id":""})");
    const std::string semServico = chatbot.responder("tem horario?", "CLI-1");
    assert(contem(semServico, "Qual servico"));

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que a lista de disponibilidade marca o profissional preferido
bool Unit_Chatbot::unit_Chatbot_disponibilidadeMarcaPreferido()
{
    BellezaSystem* system = montarSalao();
    FakeLlmClient llm;
    ChatbotHandle chatbot(system, &llm);
    chatbot.setReferencia(referenciaFixa());

    const std::string json = R"({"intencao":"consultar_disponibilidade","servico":"Corte Feminino","data":"2026-07-07T14:00","agendamento_id":""})";

    // sem preferencia definida, nenhuma marcacao aparece
    llm.enfileirar(json);
    const std::string semPreferencia = chatbot.responder("tem vaga as 14h?", "CLI-1");
    assert(contem(semPreferencia, "Ana"));
    assert(!contem(semPreferencia, "preferido"));

    system->definirPreferencias("CLI-1", "PRO-1", "");

    llm.enfileirar(json);
    const std::string comPreferencia = chatbot.responder("tem vaga as 14h?", "CLI-1");
    assert(contem(comPreferencia, "Ana (seu profissional preferido)"));

    // a marcacao e do cliente logado: sem login, ninguem e preferido
    llm.enfileirar(json);
    const std::string semLogin = chatbot.responder("tem vaga as 14h?", "");
    assert(contem(semLogin, "Ana"));
    assert(!contem(semLogin, "preferido"));

    BellezaSystem::deleteModel(system);
    return true;
}

// confere a listagem dos agendamentos do proprio cliente
bool Unit_Chatbot::unit_Chatbot_responderMeusAgendamentos()
{
    BellezaSystem* system = montarSalao();
    ChatbotHandle chatbot(system, nullptr);
    chatbot.setReferencia(referenciaFixa());

    const std::string vazio = chatbot.responder("meus agendamentos", "CLI-1");
    assert(contem(vazio, "ainda nao tem nenhum agendamento"));

    Agendamento* agendamento = system->agendar("CLI-1", "PRO-1", "SER-CORTE", makeDateTime(2026, 7, 8, 11, 0));
    const std::string comItem = chatbot.responder("meus agendamentos", "CLI-1");
    assert(contem(comItem, agendamento->id()));
    assert(contem(comItem, "08/07/2026 11:00"));

    // sem cliente logado o assistente nao expoe dado de ninguem
    const std::string semLogin = chatbot.responder("meus agendamentos", "");
    assert(contem(semLogin, "Entre na sua conta"));

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que o assistente localiza o agendamento mas nao cancela sozinho
bool Unit_Chatbot::unit_Chatbot_responderCancelar()
{
    BellezaSystem* system = montarSalao();
    FakeLlmClient llm;
    ChatbotHandle chatbot(system, &llm);
    chatbot.setReferencia(referenciaFixa());

    Agendamento* agendamento = system->agendar("CLI-1", "PRO-1", "SER-CORTE", makeDateTime(2026, 7, 8, 11, 0));

    llm.enfileirar(R"({"intencao":"cancelar","servico":"","data":"","agendamento_id":")" + agendamento->id() + R"("})");
    const std::string resposta = chatbot.responder("quero cancelar esse agendamento", "CLI-1");
    assert(contem(resposta, agendamento->id()));
    assert(contem(resposta, "clique em Cancelar para confirmar"));
    // o agendamento continua ativo: quem cancela e o cliente, pela tela
    assert(agendamento->estaAtivo());

    // id que nao pertence ao cliente nao e confirmado
    llm.enfileirar(R"({"intencao":"cancelar","servico":"","data":"","agendamento_id":"AGD-INEXISTENTE"})");
    const std::string naoAchou = chatbot.responder("cancela o AGD-INEXISTENTE", "CLI-1");
    assert(contem(naoAchou, "Nao achei o agendamento"));

    BellezaSystem::deleteModel(system);
    return true;
}

// confere que as duas pontas da conversa entram no historico, na ordem
bool Unit_Chatbot::unit_Chatbot_historico()
{
    BellezaSystem* system = montarSalao();
    ChatbotHandle chatbot(system, nullptr);
    chatbot.setReferencia(referenciaFixa());

    assert(chatbot.historicoBegin() == chatbot.historicoEnd());

    chatbot.responder("bom dia", "CLI-1");

    auto it = chatbot.historicoBegin();
    assert(it->autor == AutorMensagem::Cliente);
    assert(it->texto == "bom dia");
    assert(it->instante == referenciaFixa());
    ++it;
    assert(it->autor == AutorMensagem::Assistente);
    assert(contem(it->texto, "assistente do BellezaSys"));
    ++it;
    assert(it == chatbot.historicoEnd());

    chatbot.limparHistorico();
    assert(chatbot.historicoBegin() == chatbot.historicoEnd());

    BellezaSystem::deleteModel(system);
    return true;
}

// confere a gestao de memoria: um body por handle criado com new, e
// compartilhamento de body nas copias
bool Unit_Chatbot::unit_Chatbot_handleBodyTest()
{
    BellezaSystem* system = montarSalao();
    FakeLlmClient llm;

    const int bodiesAntes = numBodyCreated;
    const int deletadosAntes = numBodyDeleted;

    {
        ChatbotHandle chatbot(system, &llm);
        assert(numBodyCreated == bodiesAntes + 1);
        assert(chatbot.pImpl_->refCount() == 1);

        ChatbotHandle copia(chatbot);
        // a copia compartilha o body, entao nenhum body novo e criado
        assert(numBodyCreated == bodiesAntes + 1);
        assert(chatbot.pImpl_->refCount() == 2);
    }

    // os dois handles sairam de escopo: o body se autodestruiu
    assert(numBodyDeleted == deletadosAntes + 1);

    BellezaSystem::deleteModel(system);
    return true;
}

bool Unit_Chatbot::run_unit_tests_Chatbot()
{
    assert(unit_Chatbot_defaultConstructor());
    assert(unit_Chatbot_Constructor());
    assert(unit_Chatbot_Copyconstructor());
    assert(unit_Chatbot_destructor());
    assert(unit_Chatbot_assignmentOperator());

    assert(unit_Chatbot_normalizarTexto());
    assert(unit_Chatbot_extrairCampoJson());
    assert(unit_Chatbot_parseDataIso());

    assert(unit_Chatbot_interpretarPorPalavraChave());
    assert(unit_Chatbot_interpretarRespostaModelo());
    assert(unit_Chatbot_interpretarComModeloIndisponivel());
    assert(unit_Chatbot_interpretarComFalhaDoModelo());
    assert(unit_Chatbot_montarPrompt());

    assert(unit_Chatbot_responderListarServicos());
    assert(unit_Chatbot_responderDisponibilidade());
    assert(unit_Chatbot_disponibilidadeMarcaPreferido());
    assert(unit_Chatbot_responderMeusAgendamentos());
    assert(unit_Chatbot_responderCancelar());
    assert(unit_Chatbot_historico());

    assert(unit_Chatbot_handleBodyTest());

    return true;
}

} // namespace bellezasys
