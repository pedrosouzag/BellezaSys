#pragma once

#include "bellezasys/core/AgendaService.hpp"
#include "bellezasys/core/ChatbotImpl.hpp"

#include <QMainWindow>

#include <memory>
#include <string>
#include <vector>

class OllamaLlmClient;

class QComboBox;
class QDateEdit;
class QDateTimeEdit;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QSpinBox;
class QStackedWidget;
class QTableWidget;
class QTextEdit;

/// janela principal do BellezaSys. mostra tres telas dentro de um
/// QStackedWidget: login, area do cliente e painel do administrador. toda
/// a logica de negocio fica em bellezasys::AgendaService (system_); esta
/// classe so monta a interface e traduz as acoes do usuario em chamadas
/// pro core.
class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    /// popula o sistema com usuarios/servicos/profissionais/agendamentos
    /// de demonstracao, so pra facilitar a apresentacao
    void criarDadosIniciais();
    void carregarDadosPersistidosOuDemo();
    void salvarDadosSilenciosamente();

    /// monta o QStackedWidget com as 3 telas
    void construirInterface();
    QWidget* criarTelaLogin();
    QWidget* criarTelaCliente();
    QWidget* criarTelaAdministrador();

    /// le email/senha da tela de login e chama system_->login()
    void autenticarUsuario();

    /// volta pra tela de login e limpa o usuario logado
    void sair();

    /// decide pra qual tela ir (cliente ou admin) de acordo com o papel
    void aplicarPerfilLogado(const bellezasys::Usuario& usuario);

    void preencherCombosCliente();
    void preencherCombosAdmin();
    void atualizarProfissionaisCliente();
    void atualizarProfissionaisAdmin();

    /// reconstroi todos os combos/tabelas/labels a partir do estado atual
    /// do system_ (chamado sempre que algo muda)
    void atualizarTudo();
    void atualizarTabelaCliente();
    void atualizarTabelaAdmin();
    void atualizarAgendaFiltradaAdmin();
    void atualizarFinanceiroAdmin();

    /// gera o relatorio do periodo escolhido na aba Relatorios
    void atualizarRelatorioAdmin();
    void atualizarResumoAdmin();

    void adminCadastrarCliente();

    /// grava o profissional preferido e as observacoes do cliente escolhido
    void adminSalvarPreferencias();

    /// traz para o formulario as preferencias ja gravadas do cliente
    /// selecionado no combo
    void adminCarregarPreferencias();
    void adminCadastrarServico();
    void adminCadastrarProfissional();
    /// monta o painel do assistente virtual dentro da tela do cliente
    QWidget* criarPainelAssistente(QWidget* parent);

    /// envia o que o cliente digitou pro Chatbot e mostra a resposta
    void assistenteEnviarMensagem();

    /// reescreve a conversa a partir do historico do Chatbot
    void atualizarConversaAssistente();

    /// atualiza o rotulo que diz se o assistente esta usando o modelo local
    /// (Ollama) ou o modo palavras-chave
    void atualizarStatusAssistente();

    void clienteCriarAgendamento();
    void clienteRemarcarSelecionado();
    void clienteCancelarSelecionado();
    void adminCriarAgendamento();
    void adminRemarcarSelecionado();
    void adminCancelarSelecionado();
    void adminConcluirSelecionado();

    /// mostra as excecoes do core (invalid_argument, logic_error) num
    /// QMessageBox, ao inves de deixar a excecao propagar
    void exibirErro(const QString& titulo, const std::exception& erro);

    /// le o id guardado no userData do item selecionado no combo
    QString comboIdAtual(const QComboBox* combo) const;

    /// le o id guardado na primeira coluna da linha selecionada na tabela
    QString idSelecionado(const QTableWidget* table) const;

    bellezasys::DateTime dataHoraCliente() const;
    bellezasys::DateTime dataHoraAdmin() const;

    /// convertem um id em nome legivel, pra mostrar nas tabelas
    QString nomeUsuario(const std::string& id) const;
    QString nomeServico(const std::string& id) const;
    QString nomeProfissional(const std::string& id) const;
    QString proximoId(const QString& prefixo) const;
    std::vector<std::string> servicosSelecionadosParaProfissional() const;

    /// dono da instancia do core (criada com BellezaSystem::createModel(),
    /// destruida com BellezaSystem::deleteModel() no destrutor)
    bellezasys::AgendaService* system_ = nullptr;
    QString usuarioLogadoId_;
    bellezasys::Papel papelLogado_ = bellezasys::Papel::Cliente;

    QStackedWidget* stack_ = nullptr;
    QLineEdit* emailInput_ = nullptr;
    QLineEdit* senhaInput_ = nullptr;
    QLabel* loginStatus_ = nullptr;

    QLabel* clienteNomeLabel_ = nullptr;
    QComboBox* clienteServicoCombo_ = nullptr;
    QComboBox* clienteProfissionalCombo_ = nullptr;
    QDateTimeEdit* clienteDataHoraInput_ = nullptr;
    QTableWidget* clienteAgendamentosTable_ = nullptr;
    QLabel* clienteEtapaLabel_ = nullptr;

    /// cliente do modelo local; fica nulo quando o Qt Network nao consegue
    /// falar com o Ollama. o Chatbot funciona nos dois casos
    std::unique_ptr<OllamaLlmClient> llm_;
    std::unique_ptr<bellezasys::ChatbotHandle> chatbot_;
    QTextEdit* assistenteConversa_ = nullptr;
    QLineEdit* assistenteEntrada_ = nullptr;
    QLabel* assistenteStatusLabel_ = nullptr;
    QPushButton* assistenteEnviarButton_ = nullptr;

    QLabel* adminNomeLabel_ = nullptr;
    QLabel* adminTotalAgendamentosLabel_ = nullptr;
    QLabel* adminAtivosLabel_ = nullptr;
    QLabel* adminSaldoLabel_ = nullptr;
    QLabel* adminComissaoLabel_ = nullptr;
    QComboBox* adminClienteCombo_ = nullptr;
    QComboBox* adminServicoCombo_ = nullptr;
    QComboBox* adminProfissionalCombo_ = nullptr;
    QDateTimeEdit* adminDataHoraInput_ = nullptr;
    QComboBox* adminFiltroProfissionalCombo_ = nullptr;
    QDateEdit* adminFiltroDataInput_ = nullptr;
    QTableWidget* adminAgendamentosTable_ = nullptr;
    QTableWidget* adminAgendaFiltradaTable_ = nullptr;
    QDateEdit* adminRelInicioInput_ = nullptr;
    QDateEdit* adminRelFimInput_ = nullptr;
    QLabel* adminRelResumoLabel_ = nullptr;
    QTableWidget* adminRelProfissionaisTable_ = nullptr;
    QTableWidget* adminServicosTable_ = nullptr;
    QTableWidget* adminProfissionaisTable_ = nullptr;
    QTableWidget* adminClientesTable_ = nullptr;
    QLineEdit* adminClienteNomeInput_ = nullptr;
    QLineEdit* adminClienteEmailInput_ = nullptr;
    QLineEdit* adminClienteSenhaInput_ = nullptr;
    QComboBox* adminPrefClienteCombo_ = nullptr;
    QComboBox* adminPrefProfissionalCombo_ = nullptr;
    QLineEdit* adminPrefObservacoesInput_ = nullptr;
    QLineEdit* adminServicoNomeInput_ = nullptr;
    QDoubleSpinBox* adminServicoPrecoInput_ = nullptr;
    QSpinBox* adminServicoDuracaoInput_ = nullptr;
    QDoubleSpinBox* adminServicoComissaoInput_ = nullptr;
    QLineEdit* adminProfissionalNomeInput_ = nullptr;
    QLineEdit* adminProfissionalEmailInput_ = nullptr;
    QSpinBox* adminProfissionalInicioInput_ = nullptr;
    QSpinBox* adminProfissionalFimInput_ = nullptr;
    QListWidget* adminProfissionalServicosList_ = nullptr;
};
