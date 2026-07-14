#include "unit_Profissional.hpp"
#include "bellezasys/core/ProfissionalImpl.hpp"

#include <cassert>
#include <chrono>
#include <stdexcept>

namespace bellezasys {

// confere que o construtor padrao inicializa o ProfissionalBody vazio
bool Unit_Profissional::unit_Profissional_defaultConstructor() {
    ProfissionalHandle profissional;

    assert(profissional.pImpl_->id == "");
    assert(profissional.pImpl_->nome == "");
    assert(profissional.pImpl_->email == "");
    assert(profissional.pImpl_->servicosAtendidos.empty());
    assert(profissional.pImpl_->expedienteInicioHora == 0);
    assert(profissional.pImpl_->expedienteFimHora == 0);

    return true;
}

// confere que o construtor parametrizado guarda os dados corretamente
bool Unit_Profissional::unit_Profissional_Constructor() {
    ProfissionalHandle profissional("P1", "Ana", "ana@belleza.com", {"SER-CORTE"}, 9, 18);

    assert(profissional.pImpl_->id == "P1");
    assert(profissional.pImpl_->nome == "Ana");
    assert(profissional.pImpl_->email == "ana@belleza.com");
    assert(profissional.pImpl_->servicosAtendidos.size() == 1);
    assert(profissional.pImpl_->expedienteInicioHora == 9);
    assert(profissional.pImpl_->expedienteFimHora == 18);

    return true;
}

// confere que o construtor de copia compartilha o mesmo body (nao duplica)
bool Unit_Profissional::unit_Profissional_Copyconstructor() {
    ProfissionalHandle profissional1("P1", "Ana", "ana@belleza.com", {"SER-CORTE"}, 9, 18);
    ProfissionalHandle profissional2(profissional1);

    assert(profissional2.pImpl_->id == "P1");
    assert(profissional2.pImpl_ == profissional1.pImpl_);

    return true;
}

// confere que o destrutor nao quebra a aplicacao
bool Unit_Profissional::unit_Profissional_destructor() {
    ProfissionalHandle* profissional = new ProfissionalHandle("P1", "Ana", "ana@belleza.com", {"SER-CORTE"}, 9, 18);

    delete profissional;

    return true;
}

// confere que o operador de atribuicao passa a apontar pro mesmo body
bool Unit_Profissional::unit_Profissional_assignmentOperator() {
    ProfissionalHandle profissional1("P1", "Ana", "ana@belleza.com", {"SER-CORTE"}, 9, 18);
    ProfissionalHandle profissional2;

    profissional2 = profissional1;

    assert(profissional2.pImpl_->id == "P1");
    assert(profissional2.pImpl_ == profissional1.pImpl_);

    return true;
}

// confere que os getters retornam os dados guardados no body
bool Unit_Profissional::unit_Profissional_getters() {
    ProfissionalHandle profissional("P1", "Ana", "ana@belleza.com", {"SER-CORTE", "SER-COLOR"}, 9, 18);

    assert(profissional.isValid());
    assert(profissional.id() == "P1");
    assert(profissional.nome() == "Ana");
    assert(profissional.email() == "ana@belleza.com");
    assert(profissional.servicosAtendidos().size() == 2);
    assert(profissional.expedienteInicioHora() == 9);
    assert(profissional.expedienteFimHora() == 18);

    return true;
}

// confere que atendeServico reconhece servico atendido e recusa o resto
bool Unit_Profissional::unit_Profissional_atendeServico() {
    ProfissionalHandle profissional("P1", "Ana", "ana@belleza.com", {"SER-CORTE"}, 9, 18);

    assert(profissional.atendeServico("SER-CORTE"));
    assert(!profissional.atendeServico("SER-MANICURE"));

    return true;
}

// confere que estaNoExpediente calcula certo com base no expediente cadastrado
bool Unit_Profissional::unit_Profissional_estaNoExpediente() {
    ProfissionalHandle profissional("P1", "Ana", "ana@belleza.com", {"SER-CORTE"}, 9, 18);

    const DateTime dentroExpediente = makeDateTime(2026, 7, 7, 10, 0);
    const DateTime foraExpediente = makeDateTime(2026, 7, 7, 20, 0);

    assert(profissional.estaNoExpediente(dentroExpediente, std::chrono::minutes(45)));
    assert(!profissional.estaNoExpediente(foraExpediente, std::chrono::minutes(45)));

    return true;
}

// confere que adicionarServico adiciona sem duplicar e rejeita id vazio
bool Unit_Profissional::unit_Profissional_adicionarServico() {
    ProfissionalHandle profissional("P1", "Ana", "ana@belleza.com", {"SER-CORTE"}, 9, 18);

    profissional.adicionarServico("SER-MANICURE");
    assert(profissional.atendeServico("SER-MANICURE"));
    assert(profissional.servicosAtendidos().size() == 2);

    // adicionar um servico ja atendido nao deve duplicar
    profissional.adicionarServico("SER-MANICURE");
    assert(profissional.servicosAtendidos().size() == 2);

    bool lancouExcecao = false;
    try {
        profissional.adicionarServico("");
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    return true;
}

// confere que removerServico tira o servico da lista
bool Unit_Profissional::unit_Profissional_removerServico() {
    ProfissionalHandle profissional("P1", "Ana", "ana@belleza.com", {"SER-CORTE", "SER-MANICURE"}, 9, 18);

    profissional.removerServico("SER-CORTE");
    assert(!profissional.atendeServico("SER-CORTE"));
    assert(profissional.servicosAtendidos().size() == 1);

    return true;
}

// confere que o construtor rejeita campos obrigatorios vazios e expediente invalido
bool Unit_Profissional::unit_Profissional_validacaoConstrutor() {
    bool lancouExcecao = false;
    try {
        ProfissionalHandle profissional("", "Ana", "ana@belleza.com", {"SER-CORTE"}, 9, 18);
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    lancouExcecao = false;
    try {
        // expediente invalido: inicio depois do fim
        ProfissionalHandle profissional("P1", "Ana", "ana@belleza.com", {"SER-CORTE"}, 18, 9);
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    return true;
}

// confere a gestao de memoria: quantos handles e bodies sao criados e
// destruidos ao copiar e destruir um ProfissionalHandle
bool Unit_Profissional::unit_Profissional_handleBodyTest() {

#ifdef DEBUGING

    numHandleCreated = 0;
    numHandleDeleted = 0;
    numBodyCreated = 0;
    numBodyDeleted = 0;

    {
        ProfissionalHandle profissional1("P1", "Ana", "ana@belleza.com", {"SER-CORTE"}, 9, 18);

        {
            ProfissionalHandle profissional2(profissional1);

            assert(numHandleCreated == 2);
            assert(numBodyCreated == 1);
        }

        assert(numHandleDeleted == 1);
        assert(numBodyDeleted == 0);
    }

    assert(numHandleDeleted == 2);
    assert(numBodyDeleted == 1);

#endif

    return true;
}

// roda todos os testes unitarios de Profissional em sequencia
bool Unit_Profissional::run_unit_tests_Profissional() {
    assert(unit_Profissional_defaultConstructor());
    assert(unit_Profissional_Constructor());
    assert(unit_Profissional_Copyconstructor());
    assert(unit_Profissional_destructor());
    assert(unit_Profissional_assignmentOperator());

    assert(unit_Profissional_getters());
    assert(unit_Profissional_atendeServico());
    assert(unit_Profissional_estaNoExpediente());
    assert(unit_Profissional_adicionarServico());
    assert(unit_Profissional_removerServico());
    assert(unit_Profissional_validacaoConstrutor());

    assert(unit_Profissional_handleBodyTest());

    return true;
}

} // namespace bellezasys
