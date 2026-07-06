#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMenuBar>
#include <QLabel>
#include <QMessageBox>

MainWindow::MainWindow(int idUsuario, const QString &nomeUsuario, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_idUsuario(idUsuario)
    , m_nomeUsuario(nomeUsuario)
{
    ui->setupUi(this);

    setWindowTitle("Salão - Produtos");

    // Mensagem de boas-vindas simples no centro da janela.
    // (Aqui entraria futuramente a tela de produtos do salão.)
    auto *boasVindas = new QLabel(QString("Bem-vindo(a), %1!").arg(m_nomeUsuario), this);
    boasVindas->setAlignment(Qt::AlignCenter);
    boasVindas->setStyleSheet("font-size: 18px;");
    setCentralWidget(boasVindas);

    configurarMenuConta();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::configurarMenuConta()
{
    auto *menuConta = menuBar()->addMenu("Conta");
    auto *acaoSair = menuConta->addAction("Sair");
    connect(acaoSair, &QAction::triggered, this, &MainWindow::onSairClicado);
}

void MainWindow::onSairClicado()
{
    const auto resposta = QMessageBox::question(this, "Sair",
        "Tem certeza que deseja sair da conta?");
    if (resposta != QMessageBox::Yes)
        return;

    emit logoutSolicitado();
    close();
}
