#pragma once

#include "bellezasys/core/AgendaService.hpp"

#include <QMainWindow>

class QComboBox;
class QDateTimeEdit;
class QLabel;
class QLineEdit;
class QPushButton;
class QStackedWidget;
class QTableWidget;

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
    void atualizarFinanceiroAdmin();
    void atualizarResumoAdmin();

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

    QLabel* adminNomeLabel_ = nullptr;
    QLabel* adminTotalAgendamentosLabel_ = nullptr;
    QLabel* adminAtivosLabel_ = nullptr;
    QLabel* adminSaldoLabel_ = nullptr;
    QLabel* adminComissaoLabel_ = nullptr;
    QComboBox* adminClienteCombo_ = nullptr;
    QComboBox* adminServicoCombo_ = nullptr;
    QComboBox* adminProfissionalCombo_ = nullptr;
    QDateTimeEdit* adminDataHoraInput_ = nullptr;
    QTableWidget* adminAgendamentosTable_ = nullptr;
    QTableWidget* adminServicosTable_ = nullptr;
    QTableWidget* adminProfissionaisTable_ = nullptr;
};
