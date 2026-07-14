#include "unit_tests.hpp"

#include "unit_Usuario.hpp"
#include "unit_Profissional.hpp"
#include "unit_Servico.hpp"
#include "unit_Agendamento.hpp"
#include "unit_Financeiro.hpp"
#include "unit_BellezaSystem.hpp"

#include <iostream>

using namespace std;
using namespace bellezasys;

// roda todas as suites de teste unitario, uma classe do core por vez
int main() {

    Unit_Usuario::run_unit_tests_Usuario();
    cout << "all Usuario unit tests passed" << endl;

    Unit_Profissional::run_unit_tests_Profissional();
    cout << "all Profissional unit tests passed" << endl;

    Unit_Servico::run_unit_tests_Servico();
    cout << "all Servico unit tests passed" << endl;

    Unit_Agendamento::run_unit_tests_Agendamento();
    cout << "all Agendamento unit tests passed" << endl;

    Unit_Financeiro::run_unit_tests_Financeiro();
    cout << "all Financeiro unit tests passed" << endl;

    Unit_BellezaSystem::run_unit_tests_BellezaSystem();
    cout << "all BellezaSystem unit tests passed" << endl;

    return 0;
}
