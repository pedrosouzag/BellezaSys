#include <cassert>
#include "../src/bib.hpp"


int main () {
    Cliente cliente;
    Pedido pedido, pedido2;

    cliente.nome = "Pedro";
    cliente.endereco = "Rua teste";

    pedido.produto = "Camisa Santos";
    pedido.valor = 130.00;
    pedido.pagamentoAprovado = true;

    pedido2.produto = "Camisa Santos";
    pedido2.valor = 130.00;
    pedido2.pagamentoAprovado = false;

    assert(dispararVenda(cliente, pedido) == true);
    assert(dispararVenda(cliente, pedido2) == false);
    //assert(dispararVenda(cliente, pedido2) == true); // teste pra nao passar

    return 0;

}


