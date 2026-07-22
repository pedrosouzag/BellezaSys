#ifndef UNIT_CHATBOT_HPP
#define UNIT_CHATBOT_HPP

namespace bellezasys {

/// suite de testes unitarios da classe ChatbotHandle
class Unit_Chatbot {

public:

    static bool unit_Chatbot_defaultConstructor();
    static bool unit_Chatbot_Constructor();
    static bool unit_Chatbot_Copyconstructor();
    static bool unit_Chatbot_destructor();
    static bool unit_Chatbot_assignmentOperator();

    static bool unit_Chatbot_normalizarTexto();
    static bool unit_Chatbot_extrairCampoJson();
    static bool unit_Chatbot_parseDataIso();

    static bool unit_Chatbot_interpretarPorPalavraChave();
    static bool unit_Chatbot_interpretarRespostaModelo();
    static bool unit_Chatbot_interpretarComModeloIndisponivel();
    static bool unit_Chatbot_interpretarComFalhaDoModelo();
    static bool unit_Chatbot_montarPrompt();

    static bool unit_Chatbot_responderListarServicos();
    static bool unit_Chatbot_responderDisponibilidade();
    static bool unit_Chatbot_disponibilidadeMarcaPreferido();
    static bool unit_Chatbot_responderMeusAgendamentos();
    static bool unit_Chatbot_responderCancelar();
    static bool unit_Chatbot_historico();

    /// confere a gestao de memoria dos objetos Handle e Body de Chatbot
    static bool unit_Chatbot_handleBodyTest();

    static bool run_unit_tests_Chatbot();
};

} // namespace bellezasys

#endif
