#ifndef PEDIDO_HPP
#define PEDIDO_HPP

#include <string>
using namespace std;

class Pedido
{
    public:
        string produto;
        float valor;
        bool pagamentoAprovado;
};

#endif