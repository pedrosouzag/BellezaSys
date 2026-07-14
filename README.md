# BellezaSys

Sistema de automacao de agendamentos para saloes de beleza, com nucleo C++ separado da interface Qt.

## Estrutura

```text
.
├── include/bellezasys/core/  # API publica da biblioteca (interfaces + *Impl.hpp)
├── src/core/                 # Implementacao da biblioteca (Body/Handle)
├── src/gui/                  # Interface Qt Widgets
├── test/unit/                # Testes unitarios (sem Qt), um Unit_X por classe
├── test/funcional/           # Testes funcionais/integracao (sem Qt)
├── doc/                      # Documentacao do projeto
└── bin/                      # Binarios gerados pelo Makefile (make test)
```

## Funcionalidades iniciadas

- Cadastro e login de usuarios.
- Cadastro de clientes, servicos e profissionais pela interface administrativa.
- Consulta de disponibilidade por servico, profissional, expediente e conflito de horario.
- Agenda visual filtrada por profissional e data.
- Agendamento, remarcacao, cancelamento e conclusao.
- Registro financeiro com saldo de caixa e comissoes.
- Persistencia simples em arquivo texto (`data/bellezasys.db`).
- Interface Qt para demonstrar o fluxo inicial ao cliente.

## Testes (unitarios + funcionais, sem Qt)

```bash
make test
```

Compila e roda `test/unit` (Unit_Usuario, Unit_Profissional, Unit_Servico,
Unit_Agendamento, Unit_Financeiro, Unit_BellezaSystem) e `test/funcional`
(cenarios de ponta a ponta), sem nenhuma dependencia de Qt.

## Compilar e rodar a interface Qt

```bash
qmake BellezaSys.pro
make        # ou mingw32-make no Windows com MinGW
./BellezaSys
```

Para nao sobrescrever o Makefile dos testes ao compilar a interface no WSL:

```bash
mkdir -p build/qt
qmake BellezaSys.pro -o build/qt/Makefile
make -C build/qt
./build/qt/BellezaSys
```

Credenciais demonstrativas:

- `admin@belleza.com` / `admin`
- `marina@email.com` / `123`

## Gitflow sugerido

- `master`: versoes estaveis entregues.
- `develop`: integracao do grupo.
- `staging`: validacao antes da entrega.
- `feature/core-agendamento`: regras de agenda.
- `feature/gui-agenda`: telas Qt.
- `feature/financeiro`: fluxo de caixa e comissoes.


## API com handle-body profissional

Cada entidade (`Usuario`, `Profissional`, `Servico`, `Agendamento`, `Financeiro`,
`BellezaSystem`) segue o padrao Handle/Body da disciplina: uma interface
abstrata (`X.hpp`), a implementacao com `XBody`/`XHandle` (`XImpl.hpp`) e o
`.cpp` correspondente. `HandleBody.hpp` traz o `Body`/`Handle<T>` genericos
com contagem de referencias.

`BellezaSystem` e criado/destruido pela fabrica estatica (`createModel()`/
`deleteModel()`), igual ao `Model` da disciplina, e nao retorna colecoes
inteiras por valor: toda consulta com varios resultados expoe um par
`Begin()`/`End()` de iterador.

Exemplo de uso recomendado:

```cpp
#include "bellezasys/core/AgendaService.hpp"

using namespace bellezasys;

AgendaService* agenda = BellezaSystem::createModel();
Servico* corte = agenda->cadastrarServico("SER-CORTE", "Corte", 80.0, std::chrono::minutes(45), 0.35);
Profissional* ana = agenda->cadastrarProfissional("PRO-1", "Ana", "ana@belleza.com", {"SER-CORTE"}, 9, 18);
Usuario* cliente = agenda->cadastrarUsuario("CLI-1", "Marina", "marina@email.com", "123", Papel::Cliente);
Agendamento* agendamento = agenda->agendar(cliente->id(), ana->id(), corte->id(), makeDateTime(2026, 7, 7, 10, 0));

BellezaSystem::deleteModel(agenda);
```

## Documentacao (Doxygen)

```bash
cd doc
doxygen Doxyfile
```

Gera `doc/html/index.html` a partir dos comentarios `///` em `include/`,
`src/` e `test/`.


## Roteiro visual para demonstracao

A interface Qt agora separa claramente as permissoes por perfil.

### Trilha do cliente

Login:

- `marina@email.com` / `123`

Fluxo sugerido:

1. Entrar como cliente.
2. Escolher servico, data e hora.
3. Consultar profissionais disponiveis.
4. Confirmar agendamento.
5. Ver que a tabela mostra apenas os horarios desse cliente.
6. Remarcar ou cancelar um agendamento selecionado.

### Trilha do administrador

Login:

- `admin@belleza.com` / `admin`

Fluxo sugerido:

1. Entrar como administrador.
2. Ver os indicadores gerais de agenda, caixa e comissoes.
3. Criar agendamento para qualquer cliente.
4. Visualizar toda a agenda do salao.
5. Remarcar, cancelar ou concluir atendimentos.
6. Ao concluir um atendimento, observar a atualizacao do financeiro.
7. Conferir os servicos e profissionais cadastrados na aba Cadastros.
