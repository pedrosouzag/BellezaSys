#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int idUsuario, const QString &nomeUsuario, QWidget *parent = nullptr);
    ~MainWindow() override;

signals:
    // Emitido quando o usuário clica em "Sair", para que o main.cpp volte à tela de login
    void logoutSolicitado();

private slots:
    void onSairClicado();

private:
    void configurarMenuConta();

    Ui::MainWindow *ui;
    int m_idUsuario;
    QString m_nomeUsuario;
};
#endif // MAINWINDOW_H
