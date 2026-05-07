#ifndef PEDIDO_HPP
#define PEDIDO_HPP

#include <string>

using namespace std;

class Pedido
{
    private:
        string produto;
        float valor;
        bool pagamentoAprovado;
        string rastreio;

    public:

        Pedido() : produto(""), valor (0.0), pagamentoAprovado(false), rastreio("") {}
        
        Pedido(string produto, float valor, bool pagamentoAprovado) {
            this->produto = produto;
            this->valor = valor;
            this->pagamentoAprovado = pagamentoAprovado;
        }

        string getProduto(){
            return produto;
        }

        bool getPagamentoAprovado() {
            return pagamentoAprovado;
        }

        void setRastreio(string rastreio){
            this->rastreio = rastreio;
        }

        string getRastreio() {
            return rastreio;
        }
};

#endif