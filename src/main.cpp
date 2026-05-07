#include "bib.hpp"

int main()
{
    Cliente cliente("Pedro", "Rua teste");

    Pedido pedido("Camisa Santos", 130.00, true);

    dispararVenda(cliente, pedido);

    setarRastreio(pedido, "BR123456789");

    return 0;
}