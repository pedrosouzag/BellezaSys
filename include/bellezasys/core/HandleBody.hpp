#pragma once

namespace bellezasys {

/// contadores globais usados nos testes unitarios para verificar a criacao
/// e destruicao de objetos Handle/Body (ver handleBodyTest em cada classe
/// de teste)
#define DEBUGING
#ifdef DEBUGING
extern int numHandleCreated;
extern int numHandleDeleted;
extern int numBodyCreated;
extern int numBodyDeleted;
#endif

/// classe Body: guarda os dados reais de uma entidade e controla quantos
/// Handles apontam pra ela atraves de contagem de referencias (refCount_).
/// quando o ultimo Handle e destruido, o Body se autodestroi.
class Body {
public:
    Body();

    /// incrementa a contagem de referencias
    void attach();

    /// decrementa a contagem de referencias; quando chega a zero, o
    /// proprio Body se deleta (delete this)
    void detach();

    /// retorna quantos Handles apontam para este Body no momento
    int refCount() const;

    virtual ~Body();

protected:
    // body nao pode ser copiado diretamente; a copia acontece via Handle
    Body(const Body&) = delete;
    Body& operator=(const Body&) = delete;

private:
    int refCount_ = 0;
};

/// classe Handle<T>: implementa o padrao "bridge" (handle/body). representa
/// a interface publica de uma entidade, delegando os dados reais para um
/// Body do tipo T (pImpl_). copiar um Handle nao duplica o Body, so
/// incrementa a contagem de referencias.
template <class T>
class Handle {

public:
    /// construtor padrao: cria um novo Body do tipo T
    Handle()
        : pImpl_(new T())
    {
        pImpl_->attach();
#ifdef DEBUGING
        numHandleCreated++;
#endif
    }

    /// construtor que recebe um Body ja alocado (usado pelas classes
    /// concretas que constroem o Body com dados especificos)
    explicit Handle(T* body)
        : pImpl_(body)
    {
        pImpl_->attach();
#ifdef DEBUGING
        numHandleCreated++;
#endif
    }

    /// construtor de copia: aponta para o mesmo Body do outro handle
    Handle(const Handle& other)
        : pImpl_(other.pImpl_)
    {
        pImpl_->attach();
#ifdef DEBUGING
        numHandleCreated++;
#endif
    }

    /// construtor de movimentacao: rouba o ponteiro do outro handle, sem
    /// mexer na contagem de referencias do Body
    Handle(Handle&& other) noexcept
        : pImpl_(other.pImpl_)
    {
        other.pImpl_ = nullptr;
#ifdef DEBUGING
        numHandleCreated++;
#endif
    }

    /// destrutor: libera a referencia ao Body (que se autodestroi quando
    /// a contagem chega a zero)
    virtual ~Handle()
    {
        if (pImpl_ != nullptr) {
            pImpl_->detach();
        }
#ifdef DEBUGING
        numHandleDeleted++;
#endif
    }

    /// operador de atribuicao por copia: solta o Body antigo e passa a
    /// apontar para o Body do outro handle
    Handle& operator=(const Handle& other)
    {
        if (this != &other) {
            other.pImpl_->attach();
            if (pImpl_ != nullptr) {
                pImpl_->detach();
            }
            pImpl_ = other.pImpl_;
        }
        return *this;
    }

    /// operador de atribuicao por movimentacao
    Handle& operator=(Handle&& other) noexcept
    {
        if (this != &other) {
            if (pImpl_ != nullptr) {
                pImpl_->detach();
            }
            pImpl_ = other.pImpl_;
            other.pImpl_ = nullptr;
        }
        return *this;
    }

protected:
    // ponteiro para a implementacao real (o body)
    T* pImpl_;
};

} // namespace bellezasys
