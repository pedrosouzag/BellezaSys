#include "bellezasys/core/ChatbotImpl.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace bellezasys {

namespace {

/// tabela de acentos em UTF-8: cada acento ocupa dois bytes, entao a
/// normalizacao troca o par pelo caractere simples correspondente
struct Acento {
    const char* utf8;
    char simples;
};

const std::array<Acento, 22> kAcentos = { {
    { "\xC3\xA1", 'a' }, { "\xC3\xA0", 'a' }, { "\xC3\xA2", 'a' }, { "\xC3\xA3", 'a' }, { "\xC3\xA4", 'a' },
    { "\xC3\xA9", 'e' }, { "\xC3\xA8", 'e' }, { "\xC3\xAA", 'e' }, { "\xC3\xAB", 'e' },
    { "\xC3\xAD", 'i' }, { "\xC3\xAC", 'i' }, { "\xC3\xAE", 'i' },
    { "\xC3\xB3", 'o' }, { "\xC3\xB2", 'o' }, { "\xC3\xB4", 'o' }, { "\xC3\xB5", 'o' }, { "\xC3\xB6", 'o' },
    { "\xC3\xBA", 'u' }, { "\xC3\xB9", 'u' }, { "\xC3\xBB", 'u' }, { "\xC3\xBC", 'u' },
    { "\xC3\xA7", 'c' },
} };

/// true quando o texto normalizado contem o termo informado
bool contem(const std::string& texto, const std::string& termo)
{
    return texto.find(termo) != std::string::npos;
}

/// formata um valor monetario no padrao "R$ 80.00"
std::string formatarMoeda(double valor)
{
    std::ostringstream out;
    out << "R$ " << std::fixed << std::setprecision(2) << valor;
    return out.str();
}

/// formata um DateTime no padrao ISO "AAAA-MM-DDTHH:MM", que e o mesmo
/// formato que pedimos ao modelo no campo "data"
std::string formatarIso(DateTime valor)
{
    const std::time_t bruto = std::chrono::system_clock::to_time_t(valor);
    std::tm local {};
#if defined(_WIN32)
    localtime_s(&local, &bruto);
#else
    localtime_r(&bruto, &local);
#endif

    std::ostringstream out;
    out << std::put_time(&local, "%Y-%m-%dT%H:%M");
    return out.str();
}

/// so a parte da data, "AAAA-MM-DD", usada nas regras de dia relativo
std::string formatarDiaIso(DateTime valor)
{
    return formatarIso(valor).substr(0, 10);
}

} // namespace

std::string toString(IntencaoChat intencao)
{
    switch (intencao) {
    case IntencaoChat::Saudacao:
        return "saudacao";
    case IntencaoChat::ListarServicos:
        return "listar_servicos";
    case IntencaoChat::ConsultarDisponibilidade:
        return "consultar_disponibilidade";
    case IntencaoChat::MeusAgendamentos:
        return "meus_agendamentos";
    case IntencaoChat::Cancelar:
        return "cancelar";
    case IntencaoChat::Ajuda:
        return "ajuda";
    case IntencaoChat::Desconhecida:
    default:
        return "desconhecida";
    }
}

std::string normalizarTexto(const std::string& texto)
{
    std::string saida;
    saida.reserve(texto.size());

    for (std::size_t i = 0; i < texto.size(); ++i) {
        // acentos ocupam dois bytes em UTF-8; tenta casar o par primeiro
        bool trocou = false;
        if (i + 1 < texto.size()) {
            for (const Acento& acento : kAcentos) {
                if (texto[i] == acento.utf8[0] && texto[i + 1] == acento.utf8[1]) {
                    saida.push_back(acento.simples);
                    ++i;
                    trocou = true;
                    break;
                }
            }
        }
        if (trocou) {
            continue;
        }

        unsigned char c = static_cast<unsigned char>(texto[i]);
        if (std::isalnum(c) != 0) {
            saida.push_back(static_cast<char>(std::tolower(c)));
        } else if (std::isspace(c) != 0 || c == '-' || c == '_') {
            // colapsa qualquer separador em um unico espaco
            if (!saida.empty() && saida.back() != ' ') {
                saida.push_back(' ');
            }
        }
        // pontuacao e bytes nao-ASCII restantes sao descartados
    }

    while (!saida.empty() && saida.back() == ' ') {
        saida.pop_back();
    }
    return saida;
}

std::string extrairCampoJson(const std::string& json, const std::string& campo)
{
    const std::string chave = "\"" + campo + "\"";
    std::size_t posChave = json.find(chave);
    if (posChave == std::string::npos) {
        return {};
    }

    std::size_t posDoisPontos = json.find(':', posChave + chave.size());
    if (posDoisPontos == std::string::npos) {
        return {};
    }

    std::size_t posAbre = json.find('"', posDoisPontos + 1);
    if (posAbre == std::string::npos) {
        return {};
    }

    std::string valor;
    for (std::size_t i = posAbre + 1; i < json.size(); ++i) {
        if (json[i] == '\\' && i + 1 < json.size()) {
            // mantem o caractere escapado sem interpretar a sequencia
            valor.push_back(json[i + 1]);
            ++i;
            continue;
        }
        if (json[i] == '"') {
            return valor;
        }
        valor.push_back(json[i]);
    }
    return {};
}

bool parseDataIso(const std::string& texto, DateTime& saida)
{
    // formato esperado: AAAA-MM-DDTHH:MM (o "T" tambem aceita espaco)
    if (texto.size() < 16) {
        return false;
    }
    if (texto[4] != '-' || texto[7] != '-' || texto[13] != ':') {
        return false;
    }
    if (texto[10] != 'T' && texto[10] != ' ') {
        return false;
    }

    for (std::size_t i : { 0u, 1u, 2u, 3u, 5u, 6u, 8u, 9u, 11u, 12u, 14u, 15u }) {
        if (std::isdigit(static_cast<unsigned char>(texto[i])) == 0) {
            return false;
        }
    }

    const int ano = std::atoi(texto.substr(0, 4).c_str());
    const int mes = std::atoi(texto.substr(5, 2).c_str());
    const int dia = std::atoi(texto.substr(8, 2).c_str());
    const int hora = std::atoi(texto.substr(11, 2).c_str());
    const int minuto = std::atoi(texto.substr(14, 2).c_str());

    if (mes < 1 || mes > 12 || dia < 1 || dia > 31 || hora > 23 || minuto > 59) {
        return false;
    }

    saida = makeDateTime(ano, mes, dia, hora, minuto);
    return true;
}

// ---------------------------------------------------------------- FakeLlm

void FakeLlmClient::enfileirar(std::string resposta)
{
    respostas_.push_back(std::move(resposta));
}

void FakeLlmClient::setDisponivel(bool disponivel)
{
    disponivel_ = disponivel;
}

const std::string& FakeLlmClient::ultimoPrompt() const { return ultimoPrompt_; }

std::size_t FakeLlmClient::pendentes() const { return respostas_.size(); }

std::string FakeLlmClient::completar(const std::string& prompt)
{
    ultimoPrompt_ = prompt;
    if (respostas_.empty()) {
        throw std::logic_error("FakeLlmClient sem resposta enfileirada.");
    }
    std::string resposta = respostas_.front();
    respostas_.pop_front();
    return resposta;
}

bool FakeLlmClient::disponivel() const { return disponivel_; }

std::string FakeLlmClient::nomeModelo() const { return "fake"; }

// ------------------------------------------------------------ ChatbotBody

ChatbotBody::ChatbotBody(BellezaSystem* system, LlmClient* llm)
    : system(system), llm(llm)
{
    if (system == nullptr) {
        throw std::invalid_argument("Chatbot precisa de um BellezaSystem.");
    }
}

std::string ChatbotBody::montarPrompt(const std::string& mensagem) const
{
    // o catalogo real entra no prompt para o modelo escolher entre os
    // servicos que existem de verdade, em vez de inventar um nome
    std::ostringstream catalogo;
    BellezaSystem* mutavel = const_cast<BellezaSystem*>(system);
    std::string primeiroServico;
    std::string segundoServico;
    for (auto it = mutavel->servicosBegin(); it != mutavel->servicosEnd(); ++it) {
        catalogo << "- " << (*it)->nome() << "\n";
        if (primeiroServico.empty()) {
            primeiroServico = (*it)->nome();
        } else if (segundoServico.empty()) {
            segundoServico = (*it)->nome();
        }
    }

    // os exemplos citam servicos que existem mesmo: se citassem um nome
    // fixo, o modelo copiaria esse nome em saloes com outro catalogo
    if (primeiroServico.empty()) {
        primeiroServico = "Corte";
    }
    if (segundoServico.empty()) {
        segundoServico = primeiroServico;
    }

    DateTime agora = referencia;
    if (agora == DateTime{}) {
        agora = std::chrono::system_clock::now();
    }

    // modelos pequenos erram data relativa, entao os dias ja vao calculados.
    // a classificacao em si e ensinada por exemplos (few-shot): descrever as
    // intencoes em prosa fazia o modelo de 3B superusar listar_servicos
    const std::string hoje = formatarDiaIso(agora);
    const std::string amanha = formatarDiaIso(agora + std::chrono::hours(24));
    const std::string depois = formatarDiaIso(agora + std::chrono::hours(48));

    std::ostringstream prompt;
    prompt << "Voce classifica mensagens de clientes de um salao de beleza.\n"
           << "Responda SOMENTE com um objeto JSON, sem texto em volta.\n\n"
           << "Campos:\n"
           << "  intencao: um de [saudacao, listar_servicos, consultar_disponibilidade, "
              "meus_agendamentos, cancelar, ajuda, desconhecida]\n"
           << "  servico: nome do servico citado, exatamente como esta no catalogo, ou \"\"\n"
           << "  data: data e hora no formato AAAA-MM-DDTHH:MM, ou \"\"\n"
           << "  agendamento_id: id citado pelo cliente, ou \"\"\n\n"
           << "Catalogo de servicos:\n"
           << catalogo.str() << "\n"
           << "Data e hora de referencia: " << formatarIso(agora) << "\n"
           << "Datas relativas: \"hoje\" = " << hoje << ", \"amanha\" = " << amanha
           << ", \"depois de amanha\" = " << depois << ".\n"
           << "Periodos sem hora: \"de manha\" = 09:00, \"de tarde\" = 14:00, \"de noite\" = 19:00.\n"
           << "Se a frase nao tiver data nem hora, data = \"\".\n\n"
           << "Exemplos:\n"
           << "Mensagem: bom dia\n"
           << "JSON: {\"intencao\":\"saudacao\",\"servico\":\"\",\"data\":\"\",\"agendamento_id\":\"\"}\n"
           << "Mensagem: quanto custa " << segundoServico << "\n"
           << "JSON: {\"intencao\":\"listar_servicos\",\"servico\":\"" << segundoServico
           << "\",\"data\":\"\",\"agendamento_id\":\"\"}\n"
           << "Mensagem: tem vaga hoje de manha pra " << primeiroServico << "\n"
           << "JSON: {\"intencao\":\"consultar_disponibilidade\",\"servico\":\"" << primeiroServico
           << "\",\"data\":\"" << hoje << "T09:00\",\"agendamento_id\":\"\"}\n"
           << "Mensagem: quais sao os meus agendamentos\n"
           << "JSON: {\"intencao\":\"meus_agendamentos\",\"servico\":\"\",\"data\":\"\",\"agendamento_id\":\"\"}\n"
           << "Mensagem: quero desmarcar o AGD-3\n"
           << "JSON: {\"intencao\":\"cancelar\",\"servico\":\"\",\"data\":\"\",\"agendamento_id\":\"AGD-3\"}\n"
           << "Mensagem: o que voce consegue fazer\n"
           << "JSON: {\"intencao\":\"ajuda\",\"servico\":\"\",\"data\":\"\",\"agendamento_id\":\"\"}\n\n"
           << "Nunca afirme que existe horario livre: apenas classifique a frase.\n\n"
           << "Mensagem: " << mensagem << "\n"
           << "JSON:";
    return prompt.str();
}

InterpretacaoChat ChatbotBody::interpretarPorPalavraChave(const std::string& mensagem) const
{
    const std::string texto = normalizarTexto(mensagem);
    InterpretacaoChat interpretacao;
    interpretacao.veioDoModelo = false;

    if (texto.empty()) {
        interpretacao.intencao = IntencaoChat::Desconhecida;
        return interpretacao;
    }

    if (contem(texto, "cancel") || contem(texto, "desmarc")) {
        interpretacao.intencao = IntencaoChat::Cancelar;
    } else if (contem(texto, "meus agendamentos") || contem(texto, "minhas marcac")
        || contem(texto, "meus horarios") || contem(texto, "historico")) {
        interpretacao.intencao = IntencaoChat::MeusAgendamentos;
    } else if (contem(texto, "servico") || contem(texto, "preco") || contem(texto, "valor")
        || contem(texto, "tabela") || contem(texto, "quanto custa")) {
        interpretacao.intencao = IntencaoChat::ListarServicos;
    } else if (contem(texto, "agend") || contem(texto, "marcar") || contem(texto, "horario")
        || contem(texto, "disponi") || contem(texto, "vaga") || contem(texto, "livre")) {
        interpretacao.intencao = IntencaoChat::ConsultarDisponibilidade;
    } else if (contem(texto, "oi") || contem(texto, "ola") || contem(texto, "bom dia")
        || contem(texto, "boa tarde") || contem(texto, "boa noite")) {
        interpretacao.intencao = IntencaoChat::Saudacao;
    } else if (contem(texto, "ajuda") || contem(texto, "socorro") || contem(texto, "o que voce faz")) {
        interpretacao.intencao = IntencaoChat::Ajuda;
    } else {
        interpretacao.intencao = IntencaoChat::Desconhecida;
    }

    // procura o nome de algum servico cadastrado dentro da frase
    BellezaSystem* mutavel = const_cast<BellezaSystem*>(system);
    for (auto it = mutavel->servicosBegin(); it != mutavel->servicosEnd(); ++it) {
        const std::string nome = normalizarTexto((*it)->nome());
        if (!nome.empty() && contem(texto, nome)) {
            interpretacao.servicoTexto = (*it)->nome();
            break;
        }
    }

    return interpretacao;
}

InterpretacaoChat ChatbotBody::interpretarRespostaModelo(const std::string& json) const
{
    InterpretacaoChat interpretacao;
    interpretacao.veioDoModelo = true;
    interpretacao.servicoTexto = extrairCampoJson(json, "servico");
    interpretacao.dataTexto = extrairCampoJson(json, "data");
    interpretacao.agendamentoId = extrairCampoJson(json, "agendamento_id");

    const std::string intencao = normalizarTexto(extrairCampoJson(json, "intencao"));
    if (intencao == "saudacao") {
        interpretacao.intencao = IntencaoChat::Saudacao;
    } else if (intencao == "listar servicos") {
        interpretacao.intencao = IntencaoChat::ListarServicos;
    } else if (intencao == "consultar disponibilidade") {
        interpretacao.intencao = IntencaoChat::ConsultarDisponibilidade;
    } else if (intencao == "meus agendamentos") {
        interpretacao.intencao = IntencaoChat::MeusAgendamentos;
    } else if (intencao == "cancelar") {
        interpretacao.intencao = IntencaoChat::Cancelar;
    } else if (intencao == "ajuda") {
        interpretacao.intencao = IntencaoChat::Ajuda;
    } else {
        interpretacao.intencao = IntencaoChat::Desconhecida;
    }

    return interpretacao;
}

Servico* ChatbotBody::resolverServico(const std::string& texto) const
{
    const std::string alvo = normalizarTexto(texto);
    if (alvo.empty()) {
        return nullptr;
    }

    BellezaSystem* mutavel = const_cast<BellezaSystem*>(system);

    // primeira passada: nome igual
    for (auto it = mutavel->servicosBegin(); it != mutavel->servicosEnd(); ++it) {
        if (normalizarTexto((*it)->nome()) == alvo) {
            return *it;
        }
    }

    // segunda passada: um contem o outro ("corte" casa com "corte feminino")
    for (auto it = mutavel->servicosBegin(); it != mutavel->servicosEnd(); ++it) {
        const std::string nome = normalizarTexto((*it)->nome());
        if (!nome.empty() && (contem(nome, alvo) || contem(alvo, nome))) {
            return *it;
        }
    }

    return nullptr;
}

std::string ChatbotBody::executarListarServicos() const
{
    BellezaSystem* mutavel = const_cast<BellezaSystem*>(system);
    std::ostringstream out;
    out << "Estes sao os servicos do salao:";

    bool algum = false;
    for (auto it = mutavel->servicosBegin(); it != mutavel->servicosEnd(); ++it) {
        Servico* servico = *it;
        algum = true;
        out << "\n- " << servico->nome()
            << " | " << formatarMoeda(servico->preco())
            << " | " << servico->duracao().count() << " min";
    }

    if (!algum) {
        return "Ainda nao ha servicos cadastrados.";
    }
    return out.str();
}

std::string ChatbotBody::executarConsultarDisponibilidade(const InterpretacaoChat& interpretacao, const std::string& clienteId) const
{
    Servico* servico = resolverServico(interpretacao.servicoTexto);
    if (servico == nullptr) {
        return "Qual servico voce quer marcar? " + executarListarServicos();
    }

    DateTime quando = referencia;
    if (quando == DateTime{}) {
        quando = std::chrono::system_clock::now();
    }
    bool dataInformada = parseDataIso(interpretacao.dataTexto, quando);

    BellezaSystem* mutavel = const_cast<BellezaSystem*>(system);
    std::ostringstream out;
    out << "Para " << servico->nome() << " em " << formatDateTime(quando) << ":";

    // se o cliente tem profissional preferido, a lista marca qual e para
    // ele reconhecer na hora
    std::string preferidoId;
    if (!clienteId.empty()) {
        for (auto it = mutavel->usuariosBegin(); it != mutavel->usuariosEnd(); ++it) {
            if ((*it)->id() == clienteId) {
                preferidoId = (*it)->profissionalPreferidoId();
                break;
            }
        }
    }

    bool algum = false;
    // a disponibilidade vem do core, conferindo expediente e conflito de
    // horario; o modelo de linguagem nao participa dessa decisao
    for (auto it = mutavel->profissionaisDisponiveisBegin(servico->id(), quando);
         it != mutavel->profissionaisDisponiveisEnd(); ++it) {
        out << "\n- " << (*it)->nome();
        if (!preferidoId.empty() && (*it)->id() == preferidoId) {
            out << " (seu profissional preferido)";
        }
        algum = true;
    }

    if (!algum) {
        std::ostringstream vazio;
        vazio << "Nao encontrei profissional livre para " << servico->nome()
              << " em " << formatDateTime(quando) << ". Tente outro horario.";
        return vazio.str();
    }

    if (!dataInformada) {
        out << "\n(Usei o horario atual como referencia. Diga o dia e a hora para eu conferir outro momento.)";
    }
    out << "\nConfirme pela aba de agendamento para fechar o horario.";
    return out.str();
}

std::string ChatbotBody::executarMeusAgendamentos(const std::string& clienteId) const
{
    if (clienteId.empty()) {
        return "Entre na sua conta para eu conseguir ver os seus agendamentos.";
    }

    BellezaSystem* mutavel = const_cast<BellezaSystem*>(system);
    std::ostringstream out;
    out << "Seus agendamentos:";

    bool algum = false;
    for (auto it = mutavel->agendamentosDoClienteBegin(clienteId);
         it != mutavel->agendamentosDoClienteEnd(); ++it) {
        Agendamento* agendamento = *it;
        algum = true;
        out << "\n- " << agendamento->id()
            << " | " << formatDateTime(agendamento->inicio())
            << " | " << toString(agendamento->status());
    }

    if (!algum) {
        return "Voce ainda nao tem nenhum agendamento.";
    }
    return out.str();
}

std::string ChatbotBody::executarCancelar(const InterpretacaoChat& interpretacao, const std::string& clienteId) const
{
    if (clienteId.empty()) {
        return "Entre na sua conta para cancelar um agendamento.";
    }
    if (interpretacao.agendamentoId.empty()) {
        return "Me diga o id do agendamento que voce quer cancelar.\n"
            + executarMeusAgendamentos(clienteId);
    }

    // o assistente nao cancela sozinho: confere se o agendamento e mesmo do
    // cliente e devolve a confirmacao para ele fazer pela tela
    BellezaSystem* mutavel = const_cast<BellezaSystem*>(system);
    for (auto it = mutavel->agendamentosDoClienteBegin(clienteId);
         it != mutavel->agendamentosDoClienteEnd(); ++it) {
        Agendamento* agendamento = *it;
        if (agendamento->id() == interpretacao.agendamentoId) {
            if (!agendamento->estaAtivo()) {
                return "O agendamento " + agendamento->id() + " ja esta "
                    + toString(agendamento->status()) + ".";
            }
            return "Encontrei o agendamento " + agendamento->id() + " em "
                + formatDateTime(agendamento->inicio())
                + ". Selecione ele em \"Meus agendamentos\" e clique em Cancelar para confirmar.";
        }
    }

    return "Nao achei o agendamento " + interpretacao.agendamentoId + " na sua conta.";
}

std::string ChatbotBody::textoDeAjuda() const
{
    return "Posso ajudar com:\n"
           "- listar os servicos e precos\n"
           "- consultar profissionais livres para um servico e horario\n"
           "- mostrar os seus agendamentos\n"
           "- localizar um agendamento para voce cancelar";
}

void ChatbotBody::registrar(AutorMensagem autor, const std::string& texto)
{
    MensagemChat mensagem;
    mensagem.autor = autor;
    mensagem.texto = texto;
    mensagem.instante = (referencia == DateTime{}) ? std::chrono::system_clock::now() : referencia;
    historico.push_back(std::move(mensagem));
}

// ---------------------------------------------------------- ChatbotHandle

ChatbotHandle::ChatbotHandle() = default;

ChatbotHandle::ChatbotHandle(BellezaSystem* system, LlmClient* llm)
    : Handle<ChatbotBody>(new ChatbotBody(system, llm))
{
}

InterpretacaoChat ChatbotHandle::interpretar(const std::string& mensagem)
{
    // sem modelo ligado (ou com o servidor fora do ar) o assistente continua
    // funcionando pelo interpretador por palavra-chave
    if (pImpl_->llm == nullptr || !pImpl_->llm->disponivel()) {
        return pImpl_->interpretarPorPalavraChave(mensagem);
    }

    try {
        const std::string resposta = pImpl_->llm->completar(pImpl_->montarPrompt(mensagem));
        InterpretacaoChat interpretacao = pImpl_->interpretarRespostaModelo(resposta);

        // modelo pequeno as vezes devolve um rotulo que nao esta na lista;
        // nesse caso vale mais a pena tentar as palavras-chave
        if (interpretacao.intencao == IntencaoChat::Desconhecida) {
            InterpretacaoChat reserva = pImpl_->interpretarPorPalavraChave(mensagem);
            if (reserva.intencao != IntencaoChat::Desconhecida) {
                // aproveita os parametros que o modelo conseguiu extrair
                if (reserva.servicoTexto.empty()) {
                    reserva.servicoTexto = interpretacao.servicoTexto;
                }
                reserva.dataTexto = interpretacao.dataTexto;
                reserva.agendamentoId = interpretacao.agendamentoId;
                return reserva;
            }
        }
        return interpretacao;
    } catch (const std::exception&) {
        return pImpl_->interpretarPorPalavraChave(mensagem);
    }
}

std::string ChatbotHandle::responder(const std::string& mensagem, const std::string& clienteId)
{
    pImpl_->registrar(AutorMensagem::Cliente, mensagem);

    const InterpretacaoChat interpretacao = interpretar(mensagem);

    std::string resposta;
    switch (interpretacao.intencao) {
    case IntencaoChat::Saudacao:
        resposta = "Ola! Sou o assistente do BellezaSys. " + pImpl_->textoDeAjuda();
        break;
    case IntencaoChat::ListarServicos:
        resposta = pImpl_->executarListarServicos();
        break;
    case IntencaoChat::ConsultarDisponibilidade:
        resposta = pImpl_->executarConsultarDisponibilidade(interpretacao, clienteId);
        break;
    case IntencaoChat::MeusAgendamentos:
        resposta = pImpl_->executarMeusAgendamentos(clienteId);
        break;
    case IntencaoChat::Cancelar:
        resposta = pImpl_->executarCancelar(interpretacao, clienteId);
        break;
    case IntencaoChat::Ajuda:
        resposta = pImpl_->textoDeAjuda();
        break;
    case IntencaoChat::Desconhecida:
    default:
        resposta = "Nao entendi. " + pImpl_->textoDeAjuda();
        break;
    }

    pImpl_->registrar(AutorMensagem::Assistente, resposta);
    return resposta;
}

std::vector<MensagemChat>::const_iterator ChatbotHandle::historicoBegin() const
{
    return pImpl_->historico.cbegin();
}

std::vector<MensagemChat>::const_iterator ChatbotHandle::historicoEnd() const
{
    return pImpl_->historico.cend();
}

void ChatbotHandle::limparHistorico() { pImpl_->historico.clear(); }

bool ChatbotHandle::usandoModelo() const
{
    return pImpl_->llm != nullptr && pImpl_->llm->disponivel();
}

std::string ChatbotHandle::descricaoMotor() const
{
    if (!usandoModelo()) {
        return "palavras-chave (sem modelo)";
    }
    return pImpl_->llm->nomeModelo();
}

void ChatbotHandle::setReferencia(DateTime referencia) { pImpl_->referencia = referencia; }

} // namespace bellezasys
