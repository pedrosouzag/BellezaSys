#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QString>

// Classe responsável por gerenciar a conexão com o banco SQLite
// e as operações de cadastro/autenticação de usuários (clientes).
class Database
{
public:
    static Database &instance();

    // Abre a conexão com o arquivo salao.db (cria o arquivo e a tabela se não existirem)
    bool abrirConexao(QString *erro = nullptr);

    // Cadastra um novo usuário. Retorna false e preenche 'erro' em caso de falha
    // (ex: e-mail já cadastrado, campos inválidos).
    bool cadastrarUsuario(const QString &nome, const QString &email,
                          const QString &senha, QString *erro = nullptr);

    // Verifica as credenciais. Em caso de sucesso, preenche idUsuario e nomeUsuario.
    bool autenticarUsuario(const QString &email, const QString &senha,
                           int *idUsuario, QString *nomeUsuario, QString *erro = nullptr);

    bool emailCadastrado(const QString &email);

private:
    Database();
    Q_DISABLE_COPY(Database)

    QSqlDatabase m_db;

    static QString gerarSalt();
    static QString gerarHash(const QString &senha, const QString &salt);
};

#endif // DATABASE_H
