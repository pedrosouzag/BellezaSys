#include "bib.hpp"
#include <iostream>

using namespace std;

bool dispararVenda(Cliente cliente, Pedido pedido)
{
    if (!pedido.pagamentoAprovado){
        return false;
    }

    cout << "enviando pedido..." << endl;
    cout << "cliente: " << cliente.nome << endl;
    cout << "endereco: " << cliente.endereco << endl;
    cout << "produto: " << pedido.produto << endl;

    return true;
}