#include "MainWindow.hpp"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDateTime>
#include <QDateTimeEdit>
#include <QFormLayout>
#include <QFont>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStringList>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

#include <chrono>

using namespace bellezasys;

// funcoes utilitarias internas, so usadas neste arquivo
namespace {

// converte bellezasys::DateTime (chrono) pra QDateTime (Qt)
QDateTime toQtDateTime(DateTime value)
{
    return QDateTime::fromSecsSinceEpoch(std::chrono::system_clock::to_time_t(value));
}

// converte QDateTime (Qt) pra bellezasys::DateTime (chrono)
DateTime fromQtDateTime(const QDateTime& value)
{
    return std::chrono::system_clock::from_time_t(value.toSecsSinceEpoch());
}

// formata um valor em reais, ex: "R$ 80.00"
QString dinheiro(double value)
{
    return QString("R$ %1").arg(value, 0, 'f', 2);
}

// cria um label em destaque (usado como titulo de cada tela)
QLabel* titulo(const QString& texto)
{
    auto* label = new QLabel(texto);
    QFont font = label->font();
    font.setPointSize(16);
    font.setBold(true);
    label->setFont(font);
    return label;
}

// cria um item de tabela somente leitura (nao editavel pelo usuario)
QTableWidgetItem* item(const QString& text)
{
    auto* tableItem = new QTableWidgetItem(text);
    tableItem->setFlags(tableItem->flags() & ~Qt::ItemIsEditable);
    return tableItem;
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    // cria a instancia do core atraves da fabrica (registro global do
    // BellezaSystem); esse ponteiro e a unica coisa que esta janela
    // possui de fato, tudo o resto e derivado dele
    system_ = BellezaSystem::createModel();

    criarDadosIniciais();
    construirInterface();
    atualizarTudo();
}

MainWindow::~MainWindow()
{
    // desregistra e destroi o sistema (e tudo que ele possui)
    BellezaSystem::deleteModel(system_);
}

// cadastra usuarios/servicos/profissionais/agendamentos de exemplo, so
// pra a interface ja abrir com dados pra demonstrar o fluxo
void MainWindow::criarDadosIniciais()
{
    system_->cadastrarUsuario("CLI-1", "Marina Souza", "marina@email.com", "123", Papel::Cliente);
    system_->cadastrarUsuario("CLI-2", "Joao Lima", "joao@email.com", "123", Papel::Cliente);
    system_->cadastrarUsuario("FUN-1", "Ana Ribeiro", "ana@belleza.com", "123", Papel::Funcionario);
    system_->cadastrarUsuario("ADM-1", "Carla Admin", "admin@belleza.com", "admin", Papel::Administrador);

    system_->cadastrarServico("SER-CORTE", "Corte feminino", 80.0, std::chrono::minutes(45), 0.35);
    system_->cadastrarServico("SER-MANICURE", "Manicure", 45.0, std::chrono::minutes(40), 0.30);
    system_->cadastrarServico("SER-COLOR", "Coloracao", 180.0, std::chrono::minutes(120), 0.40);

    system_->cadastrarProfissional("PRO-ANA", "Ana Ribeiro", "ana@belleza.com", {"SER-CORTE", "SER-COLOR"}, 9, 18);
    system_->cadastrarProfissional("PRO-BIA", "Bia Santos", "bia@belleza.com", {"SER-MANICURE", "SER-CORTE"}, 10, 19);

    Agendamento* primeiro = system_->agendar("CLI-1", "PRO-ANA", "SER-CORTE", makeDateTime(2026, 7, 7, 10, 0));
    system_->agendar("CLI-2", "PRO-BIA", "SER-MANICURE", makeDateTime(2026, 7, 7, 11, 0));
    system_->concluirAgendamento(primeiro->id());
}

// monta o QStackedWidget com as 3 telas (login, cliente, administrador)
void MainWindow::construirInterface()
{
    stack_ = new QStackedWidget(this);
    stack_->addWidget(criarTelaLogin());
    stack_->addWidget(criarTelaCliente());
    stack_->addWidget(criarTelaAdministrador());
    setCentralWidget(stack_);
    setWindowTitle("BellezaSys - Jornada de uso");
    resize(1180, 760);
}

// monta a tela de login (email/senha + atalhos de demonstracao)
QWidget* MainWindow::criarTelaLogin()
{
    auto* page = new QWidget(this);
    auto* root = new QVBoxLayout(page);
    root->setContentsMargins(48, 48, 48, 48);

    root->addWidget(titulo("BellezaSys"));
    root->addWidget(new QLabel("Escolha um perfil de teste e entre para visualizar permissões diferentes."));

    auto* box = new QGroupBox("Entrada", page);
    auto* form = new QGridLayout(box);
    emailInput_ = new QLineEdit("marina@email.com", box);
    senhaInput_ = new QLineEdit("123", box);
    senhaInput_->setEchoMode(QLineEdit::Password);
    auto* clienteDemoButton = new QPushButton("Usar cliente", box);
    auto* adminDemoButton = new QPushButton("Usar administrador", box);
    auto* entrarButton = new QPushButton("Entrar", box);
    loginStatus_ = new QLabel("Cliente: marina@email.com / 123 | Admin: admin@belleza.com / admin", box);

    form->addWidget(new QLabel("Email"), 0, 0);
    form->addWidget(emailInput_, 0, 1, 1, 3);
    form->addWidget(new QLabel("Senha"), 1, 0);
    form->addWidget(senhaInput_, 1, 1, 1, 3);
    form->addWidget(clienteDemoButton, 2, 1);
    form->addWidget(adminDemoButton, 2, 2);
    form->addWidget(entrarButton, 2, 3);
    form->addWidget(loginStatus_, 3, 0, 1, 4);
    root->addWidget(box);

    auto* useCase = new QGroupBox("Caso de uso para apresentação", page);
    auto* useCaseLayout = new QVBoxLayout(useCase);
    useCaseLayout->addWidget(new QLabel("1. Cliente entra, consulta serviços e agenda um horário disponível."));
    useCaseLayout->addWidget(new QLabel("2. Cliente vê apenas os próprios agendamentos e pode remarcar ou cancelar."));
    useCaseLayout->addWidget(new QLabel("3. Administrador entra, visualiza toda a agenda, conclui atendimentos e acompanha caixa/comissões."));
    root->addWidget(useCase);
    root->addStretch();

    connect(clienteDemoButton, &QPushButton::clicked, this, [this]() {
        emailInput_->setText("marina@email.com");
        senhaInput_->setText("123");
    });
    connect(adminDemoButton, &QPushButton::clicked, this, [this]() {
        emailInput_->setText("admin@belleza.com");
        senhaInput_->setText("admin");
    });
    connect(entrarButton, &QPushButton::clicked, this, [this]() { autenticarUsuario(); });
    connect(senhaInput_, &QLineEdit::returnPressed, this, [this]() { autenticarUsuario(); });

    return page;
}

// monta a tela do cliente: formulario de novo agendamento + tabela com
// os agendamentos do proprio cliente
QWidget* MainWindow::criarTelaCliente()
{
    auto* page = new QWidget(this);
    auto* root = new QVBoxLayout(page);

    auto* top = new QHBoxLayout();
    clienteNomeLabel_ = titulo("Área do cliente");
    auto* sairButton = new QPushButton("Sair", page);
    top->addWidget(clienteNomeLabel_);
    top->addStretch();
    top->addWidget(sairButton);
    root->addLayout(top);

    clienteEtapaLabel_ = new QLabel("Etapa 1: escolha o serviço, data e profissional disponível.", page);
    root->addWidget(clienteEtapaLabel_);

    auto* flowBox = new QGroupBox("Novo agendamento", page);
    auto* form = new QGridLayout(flowBox);
    clienteServicoCombo_ = new QComboBox(flowBox);
    clienteProfissionalCombo_ = new QComboBox(flowBox);
    clienteDataHoraInput_ = new QDateTimeEdit(QDateTime(QDate(2026, 7, 7), QTime(12, 0)), flowBox);
    clienteDataHoraInput_->setDisplayFormat("dd/MM/yyyy HH:mm");
    clienteDataHoraInput_->setCalendarPopup(true);
    auto* consultarButton = new QPushButton("Consultar disponibilidade", flowBox);
    auto* agendarButton = new QPushButton("Confirmar agendamento", flowBox);

    form->addWidget(new QLabel("Serviço"), 0, 0);
    form->addWidget(clienteServicoCombo_, 0, 1);
    form->addWidget(new QLabel("Data e hora"), 0, 2);
    form->addWidget(clienteDataHoraInput_, 0, 3);
    form->addWidget(new QLabel("Profissional disponível"), 1, 0);
    form->addWidget(clienteProfissionalCombo_, 1, 1, 1, 2);
    form->addWidget(consultarButton, 1, 3);
    form->addWidget(agendarButton, 2, 3);
    root->addWidget(flowBox);

    auto* meusBox = new QGroupBox("Meus agendamentos", page);
    auto* meusLayout = new QVBoxLayout(meusBox);
    clienteAgendamentosTable_ = new QTableWidget(0, 6, meusBox);
    clienteAgendamentosTable_->setHorizontalHeaderLabels({"ID", "Profissional", "Serviço", "Início", "Valor", "Status"});
    clienteAgendamentosTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    clienteAgendamentosTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    clienteAgendamentosTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    meusLayout->addWidget(clienteAgendamentosTable_);

    auto* actions = new QHBoxLayout();
    auto* remarcarButton = new QPushButton("Remarcar para data escolhida", meusBox);
    auto* cancelarButton = new QPushButton("Cancelar selecionado", meusBox);
    actions->addStretch();
    actions->addWidget(remarcarButton);
    actions->addWidget(cancelarButton);
    meusLayout->addLayout(actions);
    root->addWidget(meusBox);

    connect(sairButton, &QPushButton::clicked, this, [this]() { sair(); });
    connect(consultarButton, &QPushButton::clicked, this, [this]() { atualizarProfissionaisCliente(); });
    connect(agendarButton, &QPushButton::clicked, this, [this]() { clienteCriarAgendamento(); });
    connect(remarcarButton, &QPushButton::clicked, this, [this]() { clienteRemarcarSelecionado(); });
    connect(cancelarButton, &QPushButton::clicked, this, [this]() { clienteCancelarSelecionado(); });
    connect(clienteServicoCombo_, &QComboBox::currentTextChanged, this, [this]() { atualizarProfissionaisCliente(); });
    connect(clienteDataHoraInput_, &QDateTimeEdit::dateTimeChanged, this, [this]() { atualizarProfissionaisCliente(); });

    return page;
}

// monta a tela do administrador: cartoes de resumo + abas de agenda
// geral, cadastros e financeiro
QWidget* MainWindow::criarTelaAdministrador()
{
    auto* page = new QWidget(this);
    auto* root = new QVBoxLayout(page);

    auto* top = new QHBoxLayout();
    adminNomeLabel_ = titulo("Painel administrativo");
    auto* sairButton = new QPushButton("Sair", page);
    top->addWidget(adminNomeLabel_);
    top->addStretch();
    top->addWidget(sairButton);
    root->addLayout(top);

    auto* cards = new QGridLayout();
    adminTotalAgendamentosLabel_ = new QLabel(page);
    adminAtivosLabel_ = new QLabel(page);
    adminSaldoLabel_ = new QLabel(page);
    adminComissaoLabel_ = new QLabel(page);
    cards->addWidget(new QLabel("Total de agendamentos"), 0, 0);
    cards->addWidget(adminTotalAgendamentosLabel_, 1, 0);
    cards->addWidget(new QLabel("Agendamentos ativos"), 0, 1);
    cards->addWidget(adminAtivosLabel_, 1, 1);
    cards->addWidget(new QLabel("Saldo do caixa"), 0, 2);
    cards->addWidget(adminSaldoLabel_, 1, 2);
    cards->addWidget(new QLabel("Comissões geradas"), 0, 3);
    cards->addWidget(adminComissaoLabel_, 1, 3);
    root->addLayout(cards);

    auto* tabs = new QTabWidget(page);

    auto* agendaTab = new QWidget(tabs);
    auto* agendaLayout = new QVBoxLayout(agendaTab);
    auto* novoBox = new QGroupBox("Criar agendamento pelo administrador", agendaTab);
    auto* novoForm = new QGridLayout(novoBox);
    adminClienteCombo_ = new QComboBox(novoBox);
    adminServicoCombo_ = new QComboBox(novoBox);
    adminProfissionalCombo_ = new QComboBox(novoBox);
    adminDataHoraInput_ = new QDateTimeEdit(QDateTime(QDate(2026, 7, 7), QTime(13, 0)), novoBox);
    adminDataHoraInput_->setDisplayFormat("dd/MM/yyyy HH:mm");
    adminDataHoraInput_->setCalendarPopup(true);
    auto* consultarButton = new QPushButton("Consultar disponibilidade", novoBox);
    auto* criarButton = new QPushButton("Criar para cliente", novoBox);
    novoForm->addWidget(new QLabel("Cliente"), 0, 0);
    novoForm->addWidget(adminClienteCombo_, 0, 1);
    novoForm->addWidget(new QLabel("Serviço"), 0, 2);
    novoForm->addWidget(adminServicoCombo_, 0, 3);
    novoForm->addWidget(new QLabel("Data e hora"), 1, 0);
    novoForm->addWidget(adminDataHoraInput_, 1, 1);
    novoForm->addWidget(new QLabel("Profissional"), 1, 2);
    novoForm->addWidget(adminProfissionalCombo_, 1, 3);
    novoForm->addWidget(consultarButton, 2, 2);
    novoForm->addWidget(criarButton, 2, 3);
    agendaLayout->addWidget(novoBox);

    adminAgendamentosTable_ = new QTableWidget(0, 7, agendaTab);
    adminAgendamentosTable_->setHorizontalHeaderLabels({"ID", "Cliente", "Profissional", "Serviço", "Início", "Valor", "Status"});
    adminAgendamentosTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    adminAgendamentosTable_->setSelectionBehavior(QAbstractItemView::SelectRows);
    adminAgendamentosTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    agendaLayout->addWidget(adminAgendamentosTable_);

    auto* adminActions = new QHBoxLayout();
    auto* remarcarButton = new QPushButton("Remarcar selecionado", agendaTab);
    auto* cancelarButton = new QPushButton("Cancelar selecionado", agendaTab);
    auto* concluirButton = new QPushButton("Concluir atendimento", agendaTab);
    adminActions->addStretch();
    adminActions->addWidget(remarcarButton);
    adminActions->addWidget(cancelarButton);
    adminActions->addWidget(concluirButton);
    agendaLayout->addLayout(adminActions);
    tabs->addTab(agendaTab, "Agenda geral");

    auto* cadastrosTab = new QWidget(tabs);
    auto* cadastrosLayout = new QGridLayout(cadastrosTab);
    adminServicosTable_ = new QTableWidget(0, 4, cadastrosTab);
    adminServicosTable_->setHorizontalHeaderLabels({"ID", "Serviço", "Preço", "Duração"});
    adminServicosTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    adminProfissionaisTable_ = new QTableWidget(0, 5, cadastrosTab);
    adminProfissionaisTable_->setHorizontalHeaderLabels({"ID", "Nome", "Email", "Expediente", "Serviços"});
    adminProfissionaisTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    cadastrosLayout->addWidget(new QLabel("Serviços cadastrados"), 0, 0);
    cadastrosLayout->addWidget(new QLabel("Profissionais cadastrados"), 0, 1);
    cadastrosLayout->addWidget(adminServicosTable_, 1, 0);
    cadastrosLayout->addWidget(adminProfissionaisTable_, 1, 1);
    tabs->addTab(cadastrosTab, "Cadastros");

    auto* financeiroTab = new QWidget(tabs);
    auto* financeiroLayout = new QVBoxLayout(financeiroTab);
    financeiroLayout->addWidget(new QLabel("O financeiro é atualizado quando um atendimento é concluído pelo administrador."));
    financeiroLayout->addWidget(new QLabel("Use a aba Agenda geral, selecione um atendimento ativo e clique em Concluir atendimento."));
    financeiroLayout->addWidget(new QLabel("Os cartões superiores mostram o saldo de caixa e as comissões geradas."));
    financeiroLayout->addStretch();
    tabs->addTab(financeiroTab, "Financeiro");

    root->addWidget(tabs);

    connect(sairButton, &QPushButton::clicked, this, [this]() { sair(); });
    connect(consultarButton, &QPushButton::clicked, this, [this]() { atualizarProfissionaisAdmin(); });
    connect(criarButton, &QPushButton::clicked, this, [this]() { adminCriarAgendamento(); });
    connect(remarcarButton, &QPushButton::clicked, this, [this]() { adminRemarcarSelecionado(); });
    connect(cancelarButton, &QPushButton::clicked, this, [this]() { adminCancelarSelecionado(); });
    connect(concluirButton, &QPushButton::clicked, this, [this]() { adminConcluirSelecionado(); });
    connect(adminServicoCombo_, &QComboBox::currentTextChanged, this, [this]() { atualizarProfissionaisAdmin(); });
    connect(adminDataHoraInput_, &QDateTimeEdit::dateTimeChanged, this, [this]() { atualizarProfissionaisAdmin(); });

    return page;
}

// confere email/senha no core; se der certo, procura o usuario pra
// aplicar o perfil dele (cliente ou administrador)
void MainWindow::autenticarUsuario()
{
    const QString email = emailInput_->text();
    const QString senha = senhaInput_->text();
    if (!system_->login(email.toStdString(), senha.toStdString())) {
        loginStatus_->setText("Email ou senha inválidos.");
        return;
    }

    for (auto it = system_->usuariosBegin(); it != system_->usuariosEnd(); ++it) {
        Usuario* usuario = *it;
        if (usuario->email() == email.toStdString()) {
            aplicarPerfilLogado(*usuario);
            return;
        }
    }
}

void MainWindow::sair()
{
    usuarioLogadoId_.clear();
    loginStatus_->setText("Sessão encerrada. Escolha outro perfil para continuar a demonstração.");
    stack_->setCurrentIndex(0);
}

// guarda o usuario logado e troca pra tela certa de acordo com o papel
void MainWindow::aplicarPerfilLogado(const Usuario& usuario)
{
    usuarioLogadoId_ = QString::fromStdString(usuario.id());
    papelLogado_ = usuario.papel();
    atualizarTudo();

    if (usuario.papel() == Papel::Administrador) {
        adminNomeLabel_->setText("Painel administrativo - " + QString::fromStdString(usuario.nome()));
        stack_->setCurrentIndex(2);
    } else {
        clienteNomeLabel_->setText("Área do cliente - " + QString::fromStdString(usuario.nome()));
        stack_->setCurrentIndex(1);
    }
}

// preenche o combo de servicos da tela do cliente (o id real do servico
// fica guardado no userData do item, o texto e so pra exibicao)
void MainWindow::preencherCombosCliente()
{
    clienteServicoCombo_->clear();
    for (auto it = system_->servicosBegin(); it != system_->servicosEnd(); ++it) {
        Servico* servico = *it;
        clienteServicoCombo_->addItem(QString("%1 - %2").arg(QString::fromStdString(servico->nome()), dinheiro(servico->preco())), QString::fromStdString(servico->id()));
    }
}

// preenche os combos de cliente e servico da tela do administrador
void MainWindow::preencherCombosAdmin()
{
    adminClienteCombo_->clear();
    adminServicoCombo_->clear();

    for (auto it = system_->usuariosBegin(); it != system_->usuariosEnd(); ++it) {
        Usuario* usuario = *it;
        if (usuario->papel() == Papel::Cliente) {
            adminClienteCombo_->addItem(QString::fromStdString(usuario->nome()), QString::fromStdString(usuario->id()));
        }
    }

    for (auto it = system_->servicosBegin(); it != system_->servicosEnd(); ++it) {
        Servico* servico = *it;
        adminServicoCombo_->addItem(QString("%1 - %2").arg(QString::fromStdString(servico->nome()), dinheiro(servico->preco())), QString::fromStdString(servico->id()));
    }
}

// consulta no core quais profissionais estao disponiveis pro servico e
// horario escolhidos, e preenche o combo do cliente com o resultado
void MainWindow::atualizarProfissionaisCliente()
{
    clienteProfissionalCombo_->clear();
    const QString servicoId = comboIdAtual(clienteServicoCombo_);
    if (servicoId.isEmpty()) {
        return;
    }

    try {
        for (auto it = system_->profissionaisDisponiveisBegin(servicoId.toStdString(), dataHoraCliente()); it != system_->profissionaisDisponiveisEnd(); ++it) {
            Profissional* profissional = *it;
            clienteProfissionalCombo_->addItem(QString::fromStdString(profissional->nome()), QString::fromStdString(profissional->id()));
        }
        clienteEtapaLabel_->setText(clienteProfissionalCombo_->count() > 0
                                        ? "Etapa 2: escolha o profissional e confirme o agendamento."
                                        : "Nenhum profissional livre nesse horário; escolha outra data ou serviço.");
    } catch (const std::exception& erro) {
        exibirErro("Disponibilidade", erro);
    }
}

// mesma consulta de disponibilidade, so que pro combo da tela do admin
void MainWindow::atualizarProfissionaisAdmin()
{
    adminProfissionalCombo_->clear();
    const QString servicoId = comboIdAtual(adminServicoCombo_);
    if (servicoId.isEmpty()) {
        return;
    }

    try {
        for (auto it = system_->profissionaisDisponiveisBegin(servicoId.toStdString(), dataHoraAdmin()); it != system_->profissionaisDisponiveisEnd(); ++it) {
            Profissional* profissional = *it;
            adminProfissionalCombo_->addItem(QString::fromStdString(profissional->nome()), QString::fromStdString(profissional->id()));
        }
    } catch (const std::exception& erro) {
        exibirErro("Disponibilidade", erro);
    }
}

// redesenha tudo (combos, tabelas, cartoes) a partir do estado atual do
// core; chamado depois de qualquer acao que muda o sistema
void MainWindow::atualizarTudo()
{
    preencherCombosCliente();
    preencherCombosAdmin();
    atualizarProfissionaisCliente();
    atualizarProfissionaisAdmin();
    atualizarTabelaCliente();
    atualizarTabelaAdmin();
    atualizarFinanceiroAdmin();
    atualizarResumoAdmin();
}

// lista so os agendamentos do cliente logado
void MainWindow::atualizarTabelaCliente()
{
    // sem usuario logado, mostra a tabela vazia; senao materializa o
    // intervalo [Begin, End) num vetor local pra poder indexar por linha
    std::vector<Agendamento*> agendamentos;
    if (!usuarioLogadoId_.isEmpty()) {
        auto inicio = system_->agendamentosDoClienteBegin(usuarioLogadoId_.toStdString());
        auto fim = system_->agendamentosDoClienteEnd();
        agendamentos.assign(inicio, fim);
    }
    clienteAgendamentosTable_->setRowCount(static_cast<int>(agendamentos.size()));

    for (int row = 0; row < static_cast<int>(agendamentos.size()); ++row) {
        Agendamento* agendamento = agendamentos[static_cast<size_t>(row)];
        clienteAgendamentosTable_->setItem(row, 0, item(QString::fromStdString(agendamento->id())));
        clienteAgendamentosTable_->setItem(row, 1, item(nomeProfissional(agendamento->profissionalId())));
        clienteAgendamentosTable_->setItem(row, 2, item(nomeServico(agendamento->servicoId())));
        clienteAgendamentosTable_->setItem(row, 3, item(toQtDateTime(agendamento->inicio()).toString("dd/MM/yyyy HH:mm")));
        clienteAgendamentosTable_->setItem(row, 4, item(dinheiro(agendamento->valor())));
        clienteAgendamentosTable_->setItem(row, 5, item(QString::fromStdString(toString(agendamento->status()))));
    }
}

// lista todos os agendamentos do salao, de todos os clientes
void MainWindow::atualizarTabelaAdmin()
{
    auto inicio = system_->agendamentosBegin();
    auto fim = system_->agendamentosEnd();
    std::vector<Agendamento*> agendamentos(inicio, fim);
    adminAgendamentosTable_->setRowCount(static_cast<int>(agendamentos.size()));

    for (int row = 0; row < static_cast<int>(agendamentos.size()); ++row) {
        Agendamento* agendamento = agendamentos[static_cast<size_t>(row)];
        adminAgendamentosTable_->setItem(row, 0, item(QString::fromStdString(agendamento->id())));
        adminAgendamentosTable_->setItem(row, 1, item(nomeUsuario(agendamento->clienteId())));
        adminAgendamentosTable_->setItem(row, 2, item(nomeProfissional(agendamento->profissionalId())));
        adminAgendamentosTable_->setItem(row, 3, item(nomeServico(agendamento->servicoId())));
        adminAgendamentosTable_->setItem(row, 4, item(toQtDateTime(agendamento->inicio()).toString("dd/MM/yyyy HH:mm")));
        adminAgendamentosTable_->setItem(row, 5, item(dinheiro(agendamento->valor())));
        adminAgendamentosTable_->setItem(row, 6, item(QString::fromStdString(toString(agendamento->status()))));
    }
}

// atualiza os cartoes de saldo de caixa e comissoes
void MainWindow::atualizarFinanceiroAdmin()
{
    adminSaldoLabel_->setText(dinheiro(system_->financeiro().saldo()));
    adminComissaoLabel_->setText(dinheiro(system_->financeiro().totalComissoes()));
}

// atualiza os cartoes de total/ativos e as tabelas de servicos e
// profissionais cadastrados (aba Cadastros)
void MainWindow::atualizarResumoAdmin()
{
    std::vector<Agendamento*> agendamentos(system_->agendamentosBegin(), system_->agendamentosEnd());
    // conta quantos agendamentos ainda estao ativos (nao cancelados nem concluidos)
    int ativos = 0;
    for (Agendamento* agendamento : agendamentos) {
        if (agendamento->estaAtivo()) {
            ++ativos;
        }
    }
    adminTotalAgendamentosLabel_->setText(QString::number(static_cast<int>(agendamentos.size())));
    adminAtivosLabel_->setText(QString::number(ativos));

    std::vector<Servico*> servicos(system_->servicosBegin(), system_->servicosEnd());
    adminServicosTable_->setRowCount(static_cast<int>(servicos.size()));
    for (int row = 0; row < static_cast<int>(servicos.size()); ++row) {
        Servico* servico = servicos[static_cast<size_t>(row)];
        adminServicosTable_->setItem(row, 0, item(QString::fromStdString(servico->id())));
        adminServicosTable_->setItem(row, 1, item(QString::fromStdString(servico->nome())));
        adminServicosTable_->setItem(row, 2, item(dinheiro(servico->preco())));
        adminServicosTable_->setItem(row, 3, item(QString("%1 min").arg(servico->duracao().count())));
    }

    std::vector<Profissional*> profissionais(system_->profissionaisBegin(), system_->profissionaisEnd());
    adminProfissionaisTable_->setRowCount(static_cast<int>(profissionais.size()));
    for (int row = 0; row < static_cast<int>(profissionais.size()); ++row) {
        Profissional* profissional = profissionais[static_cast<size_t>(row)];
        QStringList servicosAtendidos;
        for (const std::string& servicoId : profissional->servicosAtendidos()) {
            servicosAtendidos << nomeServico(servicoId);
        }
        adminProfissionaisTable_->setItem(row, 0, item(QString::fromStdString(profissional->id())));
        adminProfissionaisTable_->setItem(row, 1, item(QString::fromStdString(profissional->nome())));
        adminProfissionaisTable_->setItem(row, 2, item(QString::fromStdString(profissional->email())));
        adminProfissionaisTable_->setItem(row, 3, item(QString("%1h às %2h").arg(profissional->expedienteInicioHora()).arg(profissional->expedienteFimHora())));
        adminProfissionaisTable_->setItem(row, 4, item(servicosAtendidos.join(", ")));
    }
}

// pede ao core pra criar o agendamento com os dados escolhidos pelo
// cliente; qualquer excecao vira um aviso na tela em vez de derrubar o app
void MainWindow::clienteCriarAgendamento()
{
    try {
        if (usuarioLogadoId_.isEmpty()) {
            throw std::logic_error("Faça login como cliente.");
        }
        if (clienteProfissionalCombo_->count() == 0) {
            throw std::logic_error("Nenhum profissional disponível para esse serviço e horário.");
        }
        system_->agendar(usuarioLogadoId_.toStdString(), comboIdAtual(clienteProfissionalCombo_).toStdString(), comboIdAtual(clienteServicoCombo_).toStdString(), dataHoraCliente());
        atualizarTudo();
        clienteEtapaLabel_->setText("Agendamento criado. A lista abaixo mostra apenas seus horários.");
    } catch (const std::exception& erro) {
        exibirErro("Agendamento do cliente", erro);
    }
}

// remarca o agendamento selecionado na tabela do cliente pra data/hora
// escolhida no formulario
void MainWindow::clienteRemarcarSelecionado()
{
    const QString id = idSelecionado(clienteAgendamentosTable_);
    if (id.isEmpty()) {
        return;
    }

    try {
        system_->remarcarAgendamento(id.toStdString(), dataHoraCliente());
        atualizarTudo();
    } catch (const std::exception& erro) {
        exibirErro("Remarcação do cliente", erro);
    }
}

// cancela o agendamento selecionado na tabela do cliente
void MainWindow::clienteCancelarSelecionado()
{
    const QString id = idSelecionado(clienteAgendamentosTable_);
    if (id.isEmpty()) {
        return;
    }

    try {
        system_->cancelarAgendamento(id.toStdString());
        atualizarTudo();
    } catch (const std::exception& erro) {
        exibirErro("Cancelamento do cliente", erro);
    }
}

// o administrador pode criar um agendamento em nome de qualquer cliente
void MainWindow::adminCriarAgendamento()
{
    try {
        if (adminProfissionalCombo_->count() == 0) {
            throw std::logic_error("Nenhum profissional disponível para esse serviço e horário.");
        }
        system_->agendar(comboIdAtual(adminClienteCombo_).toStdString(), comboIdAtual(adminProfissionalCombo_).toStdString(), comboIdAtual(adminServicoCombo_).toStdString(), dataHoraAdmin());
        atualizarTudo();
    } catch (const std::exception& erro) {
        exibirErro("Agendamento administrativo", erro);
    }
}

void MainWindow::adminRemarcarSelecionado()
{
    const QString id = idSelecionado(adminAgendamentosTable_);
    if (id.isEmpty()) {
        return;
    }

    try {
        system_->remarcarAgendamento(id.toStdString(), dataHoraAdmin());
        atualizarTudo();
    } catch (const std::exception& erro) {
        exibirErro("Remarcação administrativa", erro);
    }
}

void MainWindow::adminCancelarSelecionado()
{
    const QString id = idSelecionado(adminAgendamentosTable_);
    if (id.isEmpty()) {
        return;
    }

    try {
        system_->cancelarAgendamento(id.toStdString());
        atualizarTudo();
    } catch (const std::exception& erro) {
        exibirErro("Cancelamento administrativo", erro);
    }
}

// conclui o atendimento selecionado; isso tambem registra o pagamento no
// financeiro (ver BellezaSystem::concluirAgendamento)
void MainWindow::adminConcluirSelecionado()
{
    const QString id = idSelecionado(adminAgendamentosTable_);
    if (id.isEmpty()) {
        return;
    }

    try {
        system_->concluirAgendamento(id.toStdString());
        atualizarTudo();
    } catch (const std::exception& erro) {
        exibirErro("Conclusão de atendimento", erro);
    }
}

// mostra a mensagem da excecao num QMessageBox de aviso
void MainWindow::exibirErro(const QString& titulo, const std::exception& erro)
{
    QMessageBox::warning(this, titulo, QString::fromStdString(erro.what()));
}

// le o id real guardado no userData do item selecionado no combo (o
// texto exibido e so um nome amigavel)
QString MainWindow::comboIdAtual(const QComboBox* combo) const
{
    return combo->currentData().toString();
}

// le o id guardado na primeira coluna da linha selecionada; string vazia
// se nada estiver selecionado
QString MainWindow::idSelecionado(const QTableWidget* table) const
{
    const int row = table->currentRow();
    if (row < 0 || table->item(row, 0) == nullptr) {
        return QString();
    }
    return table->item(row, 0)->text();
}

DateTime MainWindow::dataHoraCliente() const
{
    return fromQtDateTime(clienteDataHoraInput_->dateTime());
}

DateTime MainWindow::dataHoraAdmin() const
{
    return fromQtDateTime(adminDataHoraInput_->dateTime());
}

// busca linear pelo id; usada so pra exibir nomes nas tabelas, entao o
// custo nao importa (poucos registros de demonstracao)
QString MainWindow::nomeUsuario(const std::string& id) const
{
    for (auto it = system_->usuariosBegin(); it != system_->usuariosEnd(); ++it) {
        Usuario* usuario = *it;
        if (usuario->id() == id) {
            return QString::fromStdString(usuario->nome());
        }
    }
    return QString::fromStdString(id);
}

QString MainWindow::nomeServico(const std::string& id) const
{
    for (auto it = system_->servicosBegin(); it != system_->servicosEnd(); ++it) {
        Servico* servico = *it;
        if (servico->id() == id) {
            return QString::fromStdString(servico->nome());
        }
    }
    return QString::fromStdString(id);
}

QString MainWindow::nomeProfissional(const std::string& id) const
{
    for (auto it = system_->profissionaisBegin(); it != system_->profissionaisEnd(); ++it) {
        Profissional* profissional = *it;
        if (profissional->id() == id) {
            return QString::fromStdString(profissional->nome());
        }
    }
    return QString::fromStdString(id);
}
