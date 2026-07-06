#include "logindialog.h"
#include "cadastrodialog.h"
#include "database.h"

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Login - Salão");
    setMinimumWidth(320);

    m_emailEdit = new QLineEdit(this);
    m_senhaEdit = new QLineEdit(this);
    m_senhaEdit->setEchoMode(QLineEdit::Password);

    auto *formLayout = new QFormLayout;
    formLayout->addRow("E-mail:", m_emailEdit);
    formLayout->addRow("Senha:", m_senhaEdit);

    m_mensagemLabel = new QLabel(this);
    m_mensagemLabel->setStyleSheet("color: red;");
    m_mensagemLabel->setWordWrap(true);

    auto *entrarButton = new QPushButton("Entrar", this);
    auto *cadastrarButton = new QPushButton("Criar conta", this);

    auto *botoesLayout = new QHBoxLayout;
    botoesLayout->addWidget(cadastrarButton);
    botoesLayout->addStretch();
    botoesLayout->addWidget(entrarButton);

    auto *layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->addLayout(formLayout);
    layoutPrincipal->addWidget(m_mensagemLabel);
    layoutPrincipal->addLayout(botoesLayout);

    connect(entrarButton, &QPushButton::clicked, this, &LoginDialog::onEntrarClicado);
    connect(cadastrarButton, &QPushButton::clicked, this, &LoginDialog::onCadastrarClicado);
    connect(m_senhaEdit, &QLineEdit::returnPressed, this, &LoginDialog::onEntrarClicado);

    entrarButton->setDefault(true);
}

void LoginDialog::onEntrarClicado()
{
    m_mensagemLabel->clear();

    QString erro;
    int id = -1;
    QString nome;

    const bool ok = Database::instance().autenticarUsuario(
        m_emailEdit->text(), m_senhaEdit->text(), &id, &nome, &erro);

    if (!ok) {
        m_mensagemLabel->setText(erro);
        return;
    }

    m_idUsuario = id;
    m_nomeUsuario = nome;
    accept();
}

void LoginDialog::onCadastrarClicado()
{
    CadastroDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        m_mensagemLabel->setStyleSheet("color: green;");
        m_mensagemLabel->setText("Conta criada! Faça login abaixo.");
    }
}
