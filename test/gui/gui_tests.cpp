// testes automatizados da GUI do BellezaSys, usando o modulo Qt Test.
//
// cada teste segue o padrao BDD (GIVEN / WHEN / THEN): monta a janela real
// (MainWindow), simula as acoes do usuario com QTest::mouseClick e
// QTest::keyClicks, e verifica o resultado observando os proprios widgets
// (labels, tabelas e combos), exatamente como um usuario veria na tela.
//
// os widgets sao localizados do jeito que existem no repositorio, sem
// nenhuma alteracao na MainWindow: botoes pelo texto, e os demais pelo
// tipo e pela ordem de criacao dentro de cada tela do QStackedWidget.

#include "MainWindow.hpp"

#include <QApplication>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTimer>
#include <QtTest/QtTest>

namespace {

// indices das telas dentro do QStackedWidget
constexpr int TELA_LOGIN = 0;
constexpr int TELA_CLIENTE = 1;
constexpr int TELA_ADMIN = 2;

// agenda o fechamento automatico do QMessageBox de aviso que a janela
// mostra quando o core recusa uma acao (senao o dialogo modal travaria o
// teste esperando um clique humano)
void fecharProximoAviso()
{
    QTimer::singleShot(0, [] {
        if (auto* aviso = qobject_cast<QMessageBox*>(QApplication::activeModalWidget())) {
            aviso->close();
        }
    });
}

// acha um botao pelo texto exibido, dentro de um widget qualquer
QPushButton* botao(QWidget* raiz, const QString& texto)
{
    for (QPushButton* candidato : raiz->findChildren<QPushButton*>()) {
        if (candidato->text() == texto) {
            return candidato;
        }
    }
    return nullptr;
}

// procura no combo o item cujo userData e o id informado e seleciona
void selecionarPorId(QComboBox* combo, const QString& id)
{
    const int indice = combo->findData(id);
    QVERIFY2(indice >= 0, qPrintable("id nao encontrado no combo: " + id));
    combo->setCurrentIndex(indice);
}

} // namespace

class GuiTests : public QObject {
    Q_OBJECT

private slots:
    void init();      // roda antes de cada teste: janela nova, sistema limpo
    void cleanup();   // roda depois de cada teste: destroi a janela

    void loginInvalidoMantemNaTelaDeLogin();
    void loginClienteAbreAreaDoCliente();
    void loginAdministradorAbrePainelAdministrativo();
    void clienteCriaAgendamentoPelaInterface();
    void clienteNaoConsegueAgendarEmHorarioOcupado();
    void administradorConcluiAtendimentoEFinanceiroAtualiza();

private:
    // as tres paginas do QStackedWidget, na ordem em que sao adicionadas
    QStackedWidget* stack() { return janela_->findChild<QStackedWidget*>(); }
    QWidget* telaLogin() { return stack()->widget(TELA_LOGIN); }
    QWidget* telaCliente() { return stack()->widget(TELA_CLIENTE); }
    QWidget* telaAdmin() { return stack()->widget(TELA_ADMIN); }

    // tela de login: os dois QLineEdit sao, na ordem de criacao, email e senha
    QLineEdit* emailInput() { return telaLogin()->findChildren<QLineEdit*>().at(0); }
    QLineEdit* senhaInput() { return telaLogin()->findChildren<QLineEdit*>().at(1); }

    // tela do cliente: combos na ordem de criacao (servico, profissional)
    QComboBox* clienteServicoCombo() { return telaCliente()->findChildren<QComboBox*>().at(0); }
    QComboBox* clienteProfissionalCombo() { return telaCliente()->findChildren<QComboBox*>().at(1); }
    QDateTimeEdit* clienteDataHora() { return telaCliente()->findChild<QDateTimeEdit*>(); }
    QTableWidget* clienteTabela() { return telaCliente()->findChild<QTableWidget*>(); }

    // label de etapa: o unico QLabel da tela do cliente que comeca com "Etapa"
    // ou traz o resultado da acao (procura pelo prefixo conhecido)
    QLabel* clienteEtapaLabel()
    {
        for (QLabel* label : telaCliente()->findChildren<QLabel*>()) {
            const QString texto = label->text();
            if (texto.startsWith("Etapa") || texto.startsWith("Agendamento criado")
                || texto.startsWith("Nenhum profissional")) {
                return label;
            }
        }
        return nullptr;
    }

    // tela do admin: a tabela da agenda geral e a unica com 7 colunas
    QTableWidget* adminAgendaTabela()
    {
        for (QTableWidget* tabela : telaAdmin()->findChildren<QTableWidget*>()) {
            if (tabela->columnCount() == 7) {
                return tabela;
            }
        }
        return nullptr;
    }

    // cartoes do painel: labels de valor sao criados na ordem
    // total, ativos, saldo, comissao — os dois ultimos exibem "R$ ..."
    QList<QLabel*> cartoesAdmin()
    {
        QList<QLabel*> valores;
        for (QLabel* label : telaAdmin()->findChildren<QLabel*>()) {
            const QString texto = label->text();
            bool ehNumero = false;
            texto.toInt(&ehNumero);
            if (ehNumero || texto.startsWith("R$")) {
                valores.append(label);
            }
        }
        return valores; // [total, ativos, saldo, comissao]
    }

    // preenche email/senha e clica em Entrar
    void logar(const QString& email, const QString& senha)
    {
        emailInput()->clear();
        QTest::keyClicks(emailInput(), email);
        senhaInput()->clear();
        QTest::keyClicks(senhaInput(), senha);
        QTest::mouseClick(botao(telaLogin(), "Entrar"), Qt::LeftButton);
    }

    MainWindow* janela_ = nullptr;
};

void GuiTests::init()
{
    janela_ = new MainWindow();
    janela_->show();
}

void GuiTests::cleanup()
{
    delete janela_;
    janela_ = nullptr;
}

// GIVEN a tela de login aberta.
// WHEN o usuario digita uma senha errada e clica em Entrar.
// THEN a aplicacao continua na tela de login e mostra a mensagem de erro.
void GuiTests::loginInvalidoMantemNaTelaDeLogin()
{
    logar("marina@email.com", "senha-errada");

    QCOMPARE(stack()->currentIndex(), TELA_LOGIN);

    // a mensagem de erro aparece em algum label da tela de login
    bool erroVisivel = false;
    for (QLabel* label : telaLogin()->findChildren<QLabel*>()) {
        if (label->text() == "Email ou senha inválidos.") {
            erroVisivel = true;
        }
    }
    QVERIFY2(erroVisivel, "mensagem de erro de login nao apareceu");
}

// GIVEN a tela de login aberta.
// WHEN a cliente Marina digita email e senha corretos e clica em Entrar.
// THEN a aplicacao troca pra area do cliente.
void GuiTests::loginClienteAbreAreaDoCliente()
{
    logar("marina@email.com", "123");

    QCOMPARE(stack()->currentIndex(), TELA_CLIENTE);
}

// GIVEN a tela de login aberta.
// WHEN a administradora entra com as credenciais de admin.
// THEN a aplicacao abre o painel administrativo, com os cartoes de resumo
// refletindo os dados de demonstracao (2 agendamentos, 1 ativo).
void GuiTests::loginAdministradorAbrePainelAdministrativo()
{
    logar("admin@belleza.com", "admin");

    QCOMPARE(stack()->currentIndex(), TELA_ADMIN);

    const QList<QLabel*> cartoes = cartoesAdmin();
    QCOMPARE(cartoes.size(), 4);
    QCOMPARE(cartoes.at(0)->text(), QString("2"));  // total de agendamentos
    QCOMPARE(cartoes.at(1)->text(), QString("1"));  // agendamentos ativos
}

// GIVEN a cliente Marina logada, com o servico Corte e um horario livre
// (08/07 as 14:00) escolhidos no formulario.
// WHEN ela consulta a disponibilidade e confirma o agendamento.
// THEN a profissional aparece como disponivel, a tabela "Meus
// agendamentos" ganha uma linha nova e a etapa confirma a criacao.
void GuiTests::clienteCriaAgendamentoPelaInterface()
{
    logar("marina@email.com", "123");

    selecionarPorId(clienteServicoCombo(), "SER-CORTE");
    clienteDataHora()->setDateTime(QDateTime(QDate(2026, 7, 8), QTime(14, 0)));
    QTest::mouseClick(botao(telaCliente(), "Consultar disponibilidade"), Qt::LeftButton);

    QVERIFY2(clienteProfissionalCombo()->count() > 0, "nenhum profissional disponivel no horario livre");

    const int linhasAntes = clienteTabela()->rowCount();

    QTest::mouseClick(botao(telaCliente(), "Confirmar agendamento"), Qt::LeftButton);

    QCOMPARE(clienteTabela()->rowCount(), linhasAntes + 1);
    QCOMPARE(clienteEtapaLabel()->text(),
             QString("Agendamento criado. A lista abaixo mostra apenas seus horários."));
}

// GIVEN a cliente Marina logada e o servico Manicure escolhido num
// horario em que a unica profissional que faz manicure (Bia) ja esta
// ocupada (07/07 as 11:15, dentro do atendimento das 11:00).
// WHEN ela consulta a disponibilidade e insiste em confirmar.
// THEN nenhum profissional e listado, a interface avisa que nao ha
// horario livre e nenhuma linha nova aparece na tabela.
void GuiTests::clienteNaoConsegueAgendarEmHorarioOcupado()
{
    logar("marina@email.com", "123");

    selecionarPorId(clienteServicoCombo(), "SER-MANICURE");
    clienteDataHora()->setDateTime(QDateTime(QDate(2026, 7, 7), QTime(11, 15)));
    QTest::mouseClick(botao(telaCliente(), "Consultar disponibilidade"), Qt::LeftButton);

    QCOMPARE(clienteProfissionalCombo()->count(), 0);
    QCOMPARE(clienteEtapaLabel()->text(),
             QString("Nenhum profissional livre nesse horário; escolha outra data ou serviço."));

    const int linhasAntes = clienteTabela()->rowCount();

    fecharProximoAviso();
    QTest::mouseClick(botao(telaCliente(), "Confirmar agendamento"), Qt::LeftButton);

    QCOMPARE(clienteTabela()->rowCount(), linhasAntes);
}

// GIVEN a administradora logada; o caixa mostra R$ 52.00 (corte de
// R$ 80.00 ja concluido, menos 35% de comissao) e existe 1 atendimento
// ativo (manicure de R$ 45.00, comissao 30%).
// WHEN ela seleciona o atendimento ativo na agenda geral e clica em
// Concluir atendimento.
// THEN o status na tabela vira Concluido, o saldo sobe pra R$ 83.50,
// as comissoes vao pra R$ 41.50 e o cartao de ativos zera.
void GuiTests::administradorConcluiAtendimentoEFinanceiroAtualiza()
{
    logar("admin@belleza.com", "admin");

    QList<QLabel*> cartoes = cartoesAdmin();
    QCOMPARE(cartoes.at(2)->text(), QString("R$ 52.00"));  // saldo
    QCOMPARE(cartoes.at(3)->text(), QString("R$ 28.00"));  // comissoes

    // acha na agenda geral a linha cujo status ainda esta ativo
    QTableWidget* tabela = adminAgendaTabela();
    int linhaAtiva = -1;
    for (int linha = 0; linha < tabela->rowCount(); ++linha) {
        const QString status = tabela->item(linha, 6)->text();
        if (status != "Concluido" && status != "Cancelado") {
            linhaAtiva = linha;
            break;
        }
    }
    QVERIFY2(linhaAtiva >= 0, "nenhum atendimento ativo na agenda geral");
    tabela->setCurrentCell(linhaAtiva, 0);

    QTest::mouseClick(botao(telaAdmin(), "Concluir atendimento"), Qt::LeftButton);

    QCOMPARE(tabela->item(linhaAtiva, 6)->text(), QString("Concluido"));
    cartoes = cartoesAdmin();
    QCOMPARE(cartoes.at(2)->text(), QString("R$ 83.50"));
    QCOMPARE(cartoes.at(3)->text(), QString("R$ 41.50"));
    QCOMPARE(cartoes.at(1)->text(), QString("0"));
}

QTEST_MAIN(GuiTests)
#include "gui_tests.moc"
