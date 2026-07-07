#include "unit_Servico.hpp"
#include "bellezasys/core/ServicoImpl.hpp"

#include <cassert>
#include <chrono>
#include <stdexcept>

namespace bellezasys {

// confere que o construtor padrao inicializa o ServicoBody com valores zerados
bool Unit_Servico::unit_Servico_defaultConstructor() {
    ServicoHandle servico;

    assert(servico.pImpl_->id == "");
    assert(servico.pImpl_->nome == "");
    assert(servico.pImpl_->preco == 0.0);
    assert(servico.pImpl_->duracao == std::chrono::minutes(0));
    assert(servico.pImpl_->percentualComissao == 0.0);

    return true;
}

// confere que o construtor parametrizado guarda os dados corretamente
bool Unit_Servico::unit_Servico_Constructor() {
    ServicoHandle servico("S1", "Corte", 80.0, std::chrono::minutes(45), 0.35);

    assert(servico.pImpl_->id == "S1");
    assert(servico.pImpl_->nome == "Corte");
    assert(servico.pImpl_->preco == 80.0);
    assert(servico.pImpl_->duracao == std::chrono::minutes(45));
    assert(servico.pImpl_->percentualComissao == 0.35);

    return true;
}

// confere que o construtor de copia compartilha o mesmo body (nao duplica)
bool Unit_Servico::unit_Servico_Copyconstructor() {
    ServicoHandle servico1("S1", "Corte", 80.0, std::chrono::minutes(45), 0.35);
    ServicoHandle servico2(servico1);

    assert(servico2.pImpl_->id == "S1");
    assert(servico2.pImpl_ == servico1.pImpl_);

    return true;
}

// confere que o destrutor nao quebra a aplicacao
bool Unit_Servico::unit_Servico_destructor() {
    ServicoHandle* servico = new ServicoHandle("S1", "Corte", 80.0, std::chrono::minutes(45), 0.35);

    delete servico;

    return true;
}

// confere que o operador de atribuicao passa a apontar pro mesmo body
bool Unit_Servico::unit_Servico_assignmentOperator() {
    ServicoHandle servico1("S1", "Corte", 80.0, std::chrono::minutes(45), 0.35);
    ServicoHandle servico2;

    servico2 = servico1;

    assert(servico2.pImpl_->id == "S1");
    assert(servico2.pImpl_ == servico1.pImpl_);

    return true;
}

// confere que os getters retornam os dados guardados no body
bool Unit_Servico::unit_Servico_getters() {
    ServicoHandle servico("S1", "Corte", 80.0, std::chrono::minutes(45), 0.35);

    assert(servico.isValid());
    assert(servico.id() == "S1");
    assert(servico.nome() == "Corte");
    assert(servico.preco() == 80.0);
    assert(servico.duracao() == std::chrono::minutes(45));
    assert(servico.percentualComissao() == 0.35);

    return true;
}

// confere que setNome atualiza o nome e rejeita nome vazio
bool Unit_Servico::unit_Servico_setNome() {
    ServicoHandle servico("S1", "Corte", 80.0, std::chrono::minutes(45), 0.35);

    servico.setNome("Corte Premium");
    assert(servico.pImpl_->nome == "Corte Premium");

    bool lancouExcecao = false;
    try {
        servico.setNome("");
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    return true;
}

// confere que setPreco atualiza o preco e rejeita valor negativo
bool Unit_Servico::unit_Servico_setPreco() {
    ServicoHandle servico("S1", "Corte", 80.0, std::chrono::minutes(45), 0.35);

    servico.setPreco(90.0);
    assert(servico.pImpl_->preco == 90.0);

    bool lancouExcecao = false;
    try {
        servico.setPreco(-1.0);
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    return true;
}

// confere que setDuracao atualiza a duracao e rejeita valor nao positivo
bool Unit_Servico::unit_Servico_setDuracao() {
    ServicoHandle servico("S1", "Corte", 80.0, std::chrono::minutes(45), 0.35);

    servico.setDuracao(std::chrono::minutes(60));
    assert(servico.pImpl_->duracao == std::chrono::minutes(60));

    bool lancouExcecao = false;
    try {
        servico.setDuracao(std::chrono::minutes(0));
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    return true;
}

// confere que setPercentualComissao aceita 0-1 e rejeita fora da faixa
bool Unit_Servico::unit_Servico_setPercentualComissao() {
    ServicoHandle servico("S1", "Corte", 80.0, std::chrono::minutes(45), 0.35);

    servico.setPercentualComissao(0.5);
    assert(servico.pImpl_->percentualComissao == 0.5);

    bool lancouExcecao = false;
    try {
        servico.setPercentualComissao(1.5);
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    return true;
}

// confere que o construtor rejeita campos obrigatorios vazios e preco negativo
bool Unit_Servico::unit_Servico_validacaoConstrutor() {
    bool lancouExcecao = false;
    try {
        ServicoHandle servico("", "Corte", 80.0, std::chrono::minutes(45), 0.35);
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    lancouExcecao = false;
    try {
        ServicoHandle servico("S1", "Corte", -10.0, std::chrono::minutes(45), 0.35);
    } catch (const std::invalid_argument&) {
        lancouExcecao = true;
    }
    assert(lancouExcecao);

    return true;
}

// confere a gestao de memoria: quantos handles e bodies sao criados e
// destruidos ao copiar e destruir um ServicoHandle
bool Unit_Servico::unit_Servico_handleBodyTest() {

#ifdef DEBUGING

    numHandleCreated = 0;
    numHandleDeleted = 0;
    numBodyCreated = 0;
    numBodyDeleted = 0;

    {
        ServicoHandle servico1("S1", "Corte", 80.0, std::chrono::minutes(45), 0.35);

        {
            ServicoHandle servico2(servico1);

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

// roda todos os testes unitarios de Servico em sequencia
bool Unit_Servico::run_unit_tests_Servico() {
    assert(unit_Servico_defaultConstructor());
    assert(unit_Servico_Constructor());
    assert(unit_Servico_Copyconstructor());
    assert(unit_Servico_destructor());
    assert(unit_Servico_assignmentOperator());

    assert(unit_Servico_getters());
    assert(unit_Servico_setNome());
    assert(unit_Servico_setPreco());
    assert(unit_Servico_setDuracao());
    assert(unit_Servico_setPercentualComissao());
    assert(unit_Servico_validacaoConstrutor());

    assert(unit_Servico_handleBodyTest());

    return true;
}

} // namespace bellezasys
