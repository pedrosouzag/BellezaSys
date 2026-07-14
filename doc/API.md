# API do nucleo

O nucleo fica em `include/bellezasys/core` e nao depende de Qt. A interface grafica usa `AgendaService`, um alias da fachada `BellezaSystem`, como API principal de agendamento.

## Principais classes

- `Usuario`: representa clientes, funcionarios e administradores.
- `Servico`: preco, duracao e percentual de comissao.
- `Profissional`: expediente e lista de servicos atendidos.
- `Agendamento`: horario, cliente, profissional, servico, valor e status.
- `Financeiro`: movimentos de caixa e comissoes geradas.
- `BellezaSystem`: orquestra cadastros, login, disponibilidade e agenda.

Cada classe segue o padrao handle-body: uma interface abstrata (`X.hpp`),
a implementacao (`XBody`/`XHandle`) em `XImpl.hpp`, e o `.cpp` correspondente.

## Exemplo de uso

```cpp
#include "bellezasys/core/AgendaService.hpp"

using namespace bellezasys;

AgendaService* system = BellezaSystem::createModel();

system->cadastrarUsuario("CLI-1", "Marina", "marina@email.com", "123", Papel::Cliente);
system->cadastrarServico("SER-CORTE", "Corte", 80.0, std::chrono::minutes(45), 0.35);
system->cadastrarProfissional("PRO-1", "Ana", "ana@belleza.com", {"SER-CORTE"}, 9, 18);

auto horario = makeDateTime(2026, 7, 7, 10, 0);
Agendamento* agendamento = system->agendar("CLI-1", "PRO-1", "SER-CORTE", horario);
system->concluirAgendamento(agendamento->id());
system->salvarEmArquivo("data/bellezasys.db");

BellezaSystem::deleteModel(system);
```

## Proximas evolucoes

- Banco SQLite ou relacional.
- Envio real de lembretes por WhatsApp usando uma API externa.
- Controle de permissoes por perfil autenticado.
- Relatorios financeiros por periodo.

## Handle-body

`HandleBody.hpp` centraliza o padrao `Body`/`Handle<T>` usado por todas as
classes de dominio. `BellezaSystem` e criado atraves da fabrica estatica
`createModel()`/`deleteModel()`, igual ao `Model` da disciplina, e nao
retorna colecoes inteiras por valor: toda consulta com varios resultados
(`usuarios`, `servicos`, `profissionais`, `agendamentos`,
`profissionaisDisponiveis`, `agendamentosDoCliente`,
`agendamentosDoProfissional`, `agendaDoProfissionalNoDia`) expoe um par de iteradores `Begin()`/`End()`.

```cpp
for (auto it = system->profissionaisBegin(); it != system->profissionaisEnd(); ++it) {
    Profissional* profissional = *it;
    // ...
}
```

## Persistencia simples

O prototipo salva e carrega os dados em arquivo texto, mantendo usuarios,
servicos, profissionais, agendamentos, status e o financeiro reconstruido
a partir dos atendimentos concluidos.

```cpp
system->salvarEmArquivo("data/bellezasys.db");
system->carregarDeArquivo("data/bellezasys.db");
```

Os metodos de cadastro (`cadastrarUsuario`, `cadastrarServico`,
`cadastrarProfissional`) retornam um ponteiro para o objeto recem-criado,
que pertence ao `BellezaSystem` (nao deve ser deletado manualmente).
