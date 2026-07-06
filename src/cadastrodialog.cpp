#include "cadastrodialog.h"
#include "database.h"

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

CadastroDialog::CadastroDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Cadastro de Cliente");
    setMinimumWidth(320);

    m_nomeEdit = new QLineEdit(this);
    m_emailEdit = new QLineEdit(this);
    m_senhaEdit = new QLineEdit(this);
    m_senhaEdit->setEchoMode(QLineEdit::Password);
    m_confirmarSenhaEdit = new QLineEdit(this);
    m_confirmarSenhaEdit->setEchoMode(QLineEdit::Password);

    auto *formLayout = new QFormLayout;
    formLayout->addRow("Nome:", m_nomeEdit);
    formLayout->addRow("E-mail:", m_emailEdit);
    formLayout->addRow("Senha:", m_senhaEdit);
    formLayout->addRow("Confirmar senha:", m_confirmarSenhaEdit);

    m_mensagemLabel = new QLabel(this);
    m_mensagemLabel->setStyleSheet("color: red;");
    m_mensagemLabel->setWordWrap(true);

    auto *cadastrarButton = new QPushButton("Cadastrar", this);
    auto *cancelarButton = new QPushButton("Cancelar", this);

    auto *botoesLayout = new QHBoxLayout;
    botoesLayout->addWidget(cancelarButton);
    botoesLayout->addStretch();
    botoesLayout->addWidget(cadastrarButton);

    auto *layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->addLayout(formLayout);
    layoutPrincipal->addWidget(m_mensagemLabel);
    layoutPrincipal->addLayout(botoesLayout);

    connect(cadastrarButton, &QPushButton::clicked, this, &CadastroDialog::onCadastrarClicado);
    connect(cancelarButton, &QPushButton::clicked, this, &QDialog::reject);
    cadastrarButton->setDefault(true);
}

void CadastroDialog::onCadastrarClicado()
{
    m_mensagemLabel->clear();

    if (m_senhaEdit->text() != m_confirmarSenhaEdit->text()) {
        m_mensagemLabel->setText("As senhas não coincidem.");
        return;
    }

    QString erro;
    const bool ok = Database::instance().cadastrarUsuario(
        m_nomeEdit->text(), m_emailEdit->text(), m_senhaEdit->text(), &erro);

    if (!ok) {
        m_mensagemLabel->setText(erro);
        return;
    }

    QMessageBox::information(this, "Cadastro realizado",
                              "Cadastro realizado com sucesso! Você já pode fazer login.");
    accept();
}
