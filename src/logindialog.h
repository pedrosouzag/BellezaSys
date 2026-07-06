#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QString>

class QLineEdit;
class QLabel;

// Diálogo de login. Também permite abrir o cadastro de um novo cliente.
class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);

    int idUsuario() const { return m_idUsuario; }
    QString nomeUsuario() const { return m_nomeUsuario; }

private slots:
    void onEntrarClicado();
    void onCadastrarClicado();

private:
    QLineEdit *m_emailEdit;
    QLineEdit *m_senhaEdit;
    QLabel *m_mensagemLabel;

    int m_idUsuario = -1;
    QString m_nomeUsuario;
};

#endif // LOGINDIALOG_H
