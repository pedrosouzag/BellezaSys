#include "bib.hpp"

int main () {
    Cliente cliente;
    Pedido pedido;

    cliente.nome = "Pedro";
    cliente.endereco = "Rua teste";

    pedido.produto = "Camisa Santos";
    pedido.valor = 130.00;
    pedido.pagamentoAprovado = true;

    dispararVenda(cliente, pedido);
    return 0;

}
