#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include <string>

using namespace std;

class Cliente
{
    private:
        string nome;
        string endereco;

    public:

        Cliente () : nome(""), endereco ("") {}
        
        Cliente(string nome, string endereco) {
            this->nome = nome;
            this->endereco = endereco;
        }

        string getNome(){
            return nome;
        }

        string getEndereco(){
            return endereco;
        }
};

#endif