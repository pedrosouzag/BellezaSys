#include <cassert>
#include <iostream>

#include "../src/bib.hpp"

using namespace std;

int main()
{
    Cliente cliente("Pedro", "Rua teste");

    Pedido pedido("Camisa Santos", 130.00, true);

    Pedido pedido2("Camisa Santos", 130.00, false);

    assert(dispararVenda(cliente, pedido) == true);

    assert(dispararVenda(cliente, pedido2) == false);

    // assert(dispararVenda(cliente, pedido2) == true); // teste para nao passar

    setarRastreio(pedido, "BR123456789");

    assert(pedido.getRastreio() == "BR123456789");

    return 0;
}