#include "bellezasys/core/UsuarioImpl.hpp"

#include <stdexcept>
#include <utility>

namespace bellezasys {

// valida os campos obrigatorios antes de guardar os dados
UsuarioBody::UsuarioBody(std::string id, std::string nome, std::string email, std::string senha, Papel papel)
    : id(std::move(id)), nome(std::move(nome)), email(std::move(email)), senha(std::move(senha)), papel(papel)
{
    if (this->id.empty() || this->nome.empty() || this->email.empty() || this->senha.empty()) {
        throw std::invalid_argument("Usuario precisa de id, nome, email e senha.");
    }
}

UsuarioHandle::UsuarioHandle() = default;

UsuarioHandle::UsuarioHandle(std::string id, std::string nome, std::string email, std::string senha, Papel papel)
    : Handle<UsuarioBody>(new UsuarioBody(std::move(id), std::move(nome), std::move(email), std::move(senha), papel))
{
}

bool UsuarioHandle::isValid() const { return !pImpl_->id.empty(); }
const std::string& UsuarioHandle::id() const { return pImpl_->id; }
const std::string& UsuarioHandle::nome() const { return pImpl_->nome; }
const std::string& UsuarioHandle::email() const { return pImpl_->email; }
Papel UsuarioHandle::papel() const { return pImpl_->papel; }

void UsuarioHandle::setNome(const std::string& nome)
{
    if (nome.empty()) {
        throw std::invalid_argument("Nome nao pode ser vazio.");
    }
    pImpl_->nome = nome;
}

void UsuarioHandle::setEmail(const std::string& email)
{
    if (email.empty()) {
        throw std::invalid_argument("Email nao pode ser vazio.");
    }
    pImpl_->email = email;
}

const std::string& UsuarioHandle::profissionalPreferidoId() const { return pImpl_->profissionalPreferidoId; }
const std::string& UsuarioHandle::observacoes() const { return pImpl_->observacoes; }

// string vazia e valida: significa que o cliente nao tem preferencia. quem
// confere se o profissional existe mesmo e BellezaSystem::definirPreferencias
void UsuarioHandle::setProfissionalPreferidoId(const std::string& profissionalId)
{
    pImpl_->profissionalPreferidoId = profissionalId;
}

void UsuarioHandle::setObservacoes(const std::string& observacoes)
{
    pImpl_->observacoes = observacoes;
}

// compara a senha informada com a senha guardada no body
bool UsuarioHandle::autenticar(const std::string& senha) const
{
    return isValid() && pImpl_->senha == senha;
}

// so troca a senha se a senha atual informada estiver correta
void UsuarioHandle::alterarSenha(const std::string& senhaAtual, const std::string& novaSenha)
{
    if (!autenticar(senhaAtual)) {
        throw std::invalid_argument("Senha atual incorreta.");
    }
    if (novaSenha.empty()) {
        throw std::invalid_argument("Nova senha nao pode ser vazia.");
    }
    pImpl_->senha = novaSenha;
}

} // namespace bellezasys
