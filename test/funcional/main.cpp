#include "funcional_tests.hpp"

// ponto de entrada dos testes funcionais.
//
// roda os tres cenarios de ponta a ponta em sequencia; qualquer assert
// que falhar interrompe a execucao na hora
int main() {

    jornadaClienteFuncionalTest();          // cenario 1: jornada completa do cliente
    conflitoDeAgendaFuncionalTest();         // cenario 2: conflito de agenda
    cenarioCompletoSalaoFuncionalTest();     // cenario 3: dia completo com varios clientes/profissionais
    jornadaAdministrativaPersistenciaFuncionalTest(); // cenario 4: cadastro, persistencia e agenda filtrada

    return 0;
}
