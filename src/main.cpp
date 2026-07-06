#include "mainwindow.h"
#include "logindialog.h"
#include "database.h"

#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString erroDb;
    if (!Database::instance().abrirConexao(&erroDb)) {
        QMessageBox::critical(nullptr, "Erro ao abrir banco de dados", erroDb);
        return 1;
    }

    // Loop de sessão: mostra o login; se autenticar, abre a janela principal;
    // se o usuário clicar em "Sair", volta para o login novamente.
    while (true) {
        LoginDialog login;
        if (login.exec() != QDialog::Accepted)
            return 0; // usuário fechou/cancelou o login: encerra a aplicação

        bool logout = false;
        MainWindow janela(login.idUsuario(), login.nomeUsuario());
        QObject::connect(&janela, &MainWindow::logoutSolicitado, [&logout]() {
            logout = true;
        });

        janela.show();
        a.exec();

        if (!logout)
            return 0; // janela principal fechada normalmente: encerra a aplicação
        // caso contrário, o laço reinicia e mostra o login de novo
    }
}
