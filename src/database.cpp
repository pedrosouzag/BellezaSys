#include "database.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>

Database::Database()
{
}

Database &Database::instance()
{
    static Database db;
    return db;
}

bool Database::abrirConexao(QString *erro)
{
    if (m_db.isOpen())
        return true;

    // Salva o banco na pasta de dados da aplicação (funciona em qualquer SO)
    const QString pasta = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(pasta);
    const QString caminhoArquivo = pasta + "/salao.db";

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(caminhoArquivo);

    if (!m_db.open()) {
        if (erro) *erro = m_db.lastError().text();
        return false;
    }

    QSqlQuery query(m_db);
    const bool ok = query.exec(
        "CREATE TABLE IF NOT EXISTS usuarios ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "nome TEXT NOT NULL, "
        "email TEXT NOT NULL UNIQUE, "
        "senha_hash TEXT NOT NULL, "
        "salt TEXT NOT NULL, "
        "criado_em TEXT NOT NULL"
        ")"
    );

    if (!ok && erro)
        *erro = query.lastError().text();

    return ok;
}

QString Database::gerarSalt()
{
    const int tamanho = 16;
    QByteArray bytes(tamanho, Qt::Uninitialized);
    for (int i = 0; i < tamanho; ++i)
        bytes[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    return QString(bytes.toHex());
}

QString Database::gerarHash(const QString &senha, const QString &salt)
{
    const QByteArray dados = (salt + senha).toUtf8();
    return QString(QCryptographicHash::hash(dados, QCryptographicHash::Sha256).toHex());
}

bool Database::emailCadastrado(const QString &email)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT id FROM usuarios WHERE email = :email");
    query.bindValue(":email", email.trimmed().toLower());
    if (!query.exec())
        return false;
    return query.next();
}

bool Database::cadastrarUsuario(const QString &nome, const QString &email,
                                const QString &senha, QString *erro)
{
    const QString nomeTrim = nome.trimmed();
    const QString emailTrim = email.trimmed().toLower();

    static const QRegularExpression regexEmail(
        R"(^[\w.+-]+@[\w-]+\.[A-Za-z]{2,}$)");

    if (nomeTrim.isEmpty()) {
        if (erro) *erro = "Informe o nome.";
        return false;
    }
    if (!regexEmail.match(emailTrim).hasMatch()) {
        if (erro) *erro = "E-mail inválido.";
        return false;
    }
    if (senha.length() < 6) {
        if (erro) *erro = "A senha deve ter ao menos 6 caracteres.";
        return false;
    }
    if (emailCadastrado(emailTrim)) {
        if (erro) *erro = "Este e-mail já está cadastrado.";
        return false;
    }

    const QString salt = gerarSalt();
    const QString hash = gerarHash(senha, salt);

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO usuarios (nome, email, senha_hash, salt, criado_em) "
        "VALUES (:nome, :email, :hash, :salt, :criado_em)");
    query.bindValue(":nome", nomeTrim);
    query.bindValue(":email", emailTrim);
    query.bindValue(":hash", hash);
    query.bindValue(":salt", salt);
    query.bindValue(":criado_em", QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec()) {
        if (erro) *erro = query.lastError().text();
        return false;
    }
    return true;
}

bool Database::autenticarUsuario(const QString &email, const QString &senha,
                                  int *idUsuario, QString *nomeUsuario, QString *erro)
{
    const QString emailTrim = email.trimmed().toLower();

    QSqlQuery query(m_db);
    query.prepare("SELECT id, nome, senha_hash, salt FROM usuarios WHERE email = :email");
    query.bindValue(":email", emailTrim);

    if (!query.exec()) {
        if (erro) *erro = query.lastError().text();
        return false;
    }

    if (!query.next()) {
        if (erro) *erro = "E-mail ou senha inválidos.";
        return false;
    }

    const int id = query.value("id").toInt();
    const QString nome = query.value("nome").toString();
    const QString hashSalvo = query.value("senha_hash").toString();
    const QString salt = query.value("salt").toString();

    const QString hashInformado = gerarHash(senha, salt);

    if (hashInformado != hashSalvo) {
        if (erro) *erro = "E-mail ou senha inválidos.";
        return false;
    }

    if (idUsuario) *idUsuario = id;
    if (nomeUsuario) *nomeUsuario = nome;
    return true;
}
