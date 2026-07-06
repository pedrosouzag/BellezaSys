#ifndef CADASTRODIALOG_H
#define CADASTRODIALOG_H

#include <QDialog>

class QLineEdit;
class QLabel;

// Diálogo de cadastro de novo cliente.
class CadastroDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CadastroDialog(QWidget *parent = nullptr);

private slots:
    void onCadastrarClicado();

private:
    QLineEdit *m_nomeEdit;
    QLineEdit *m_emailEdit;
    QLineEdit *m_senhaEdit;
    QLineEdit *m_confirmarSenhaEdit;
    QLabel *m_mensagemLabel;
};

#endif // CADASTRODIALOG_H
