#ifndef BIB_HPP
#define BIB_HPP

#include "cliente.hpp"
#include "pedido.hpp"

bool dispararVenda(Cliente cliente, Pedido pedido);
void setarRastreio(Pedido &pedido, string codigoRastreio);

#endif