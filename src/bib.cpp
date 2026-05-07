#include "bib.hpp"
#include <iostream>

using namespace std;

bool dispararVenda(Cliente cliente, Pedido pedido)
{
    if (!pedido.getPagamentoAprovado()){
        return false;
    }

    cout << "enviando pedido" << endl;
    cout << "cliente: " << cliente.getNome() << endl;
    cout << "endereco: " << cliente.getEndereco() << endl;
    cout << "produto: " << pedido.getProduto() << endl;

    return true;
}

void setarRastreio(Pedido &pedido, string codigoRastreio)
{
    pedido.setRastreio(codigoRastreio);

    cout << "codigo de rastreio: " << pedido.getRastreio() << endl;
}