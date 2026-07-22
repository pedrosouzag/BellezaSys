# API do nucleo

O nucleo fica em `include/bellezasys/core` e nao depende de Qt. A interface grafica usa `AgendaService`, um alias da fachada `BellezaSystem`, como API principal de agendamento.

## Principais classes

- `Usuario`: representa clientes, funcionarios e administradores, com as
  preferencias do cliente (profissional preferido e observacoes).
- `Servico`: preco, duracao e percentual de comissao.
- `Profissional`: expediente e lista de servicos atendidos.
- `Agendamento`: horario, cliente, profissional, servico, valor e status.
- `Financeiro`: movimentos de caixa, comissoes geradas e relatorios por periodo.
- `BellezaSystem`: orquestra cadastros, login, disponibilidade e agenda.
- `Chatbot`: assistente virtual que traduz frases em consultas ao `BellezaSystem`.
- `LlmClient`: interface do modelo de linguagem (`FakeLlmClient` nos testes,
  `OllamaLlmClient` na GUI).

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
- Deixar o assistente executar o agendamento, e nao so consultar.
- Autenticacao no servidor, para o controle de acesso deixar de ser
  contornavel por quem tem o objeto `BellezaSystem` em maos.

## Controle de acesso por perfil

`login()` so confere a senha. Quem liga o controle de acesso e
`iniciarSessao()`, que guarda o usuario autenticado; a partir dai as
operacoes controladas exigem permissao do papel dele.

| Permissao | Cliente | Funcionario | Administrador |
| --- | --- | --- | --- |
| `GerenciarCadastros` | nao | nao | sim |
| `GerenciarClientes` | nao | sim | sim |
| `AgendarParaTerceiros` | nao | sim | sim |
| `ConcluirAtendimento` | nao | sim | sim |
| `VerFinanceiro` | nao | sim | sim |

O cliente age sobre os proprios dados sem precisar de permissao nenhuma:
marca, remarca e cancela o proprio horario e edita as proprias preferencias.

```cpp
system->iniciarSessao("marina@email.com", "123");
system->agendar("CLI-1", "PRO-1", "SER-CORTE", horario); // ok: e ela mesma
system->agendar("CLI-2", "PRO-1", "SER-CORTE", horario); // lanca logic_error
system->encerrarSessao();
```

**Limitacao conhecida:** sem sessao aberta, tudo e permitido. Isso e
proposital, para o bootstrap do primeiro administrador, para
`carregarDeArquivo()` e para os testes montarem cenarios. Ou seja, este e um
controle de acesso de aplicacao, e nao uma barreira de seguranca: quem tem o
objeto `BellezaSystem` em maos contorna tudo apenas nao abrindo sessao. Numa
versao real a autenticacao teria de ficar no servidor.

## Relatorios financeiros

`Financeiro::relatorioPorPeriodo(inicio, fim)` consolida entradas, saidas,
saldo, comissoes e quantidade de atendimentos do intervalo, que e fechado nas
duas pontas. `relatorioPorProfissionalBegin(inicio, fim)`/`...End()` quebram o
mesmo periodo por profissional, do que mais faturou para o que menos faturou.

Os lancamentos usam a **data do atendimento**, e nao a data em que o pagamento
foi digitado. E isso que faz o relatorio bater com a agenda e mantem as datas
corretas quando o financeiro e reconstruido ao carregar o arquivo.

```cpp
const RelatorioFinanceiro julho = system->financeiro().relatorioPorPeriodo(
    makeDateTime(2026, 7, 1, 0, 0), makeDateTime(2026, 7, 31, 23, 59));
```

Como `Begin()` recalcula o cache e `End()` apenas devolve o fim dele, os dois
nunca podem ir como argumentos da mesma chamada (a ordem de avaliacao nao e
garantida):

```cpp
auto primeira = financeiro.relatorioPorProfissionalBegin(inicio, fim);
auto ultima = financeiro.relatorioPorProfissionalEnd();
std::vector<RelatorioProfissional> linhas(primeira, ultima);
```

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

As preferencias do cliente vao numa linha `PREFERENCIA` propria, gravada
depois dos profissionais que ela referencia e apenas para quem tem algo
preenchido. A linha `USUARIO` seguiu com os mesmos 6 campos, entao arquivos
gravados antes desta versao continuam carregando sem conversao.

```cpp
system->salvarEmArquivo("data/bellezasys.db");
system->carregarDeArquivo("data/bellezasys.db");
```

Os metodos de cadastro (`cadastrarUsuario`, `cadastrarServico`,
`cadastrarProfissional`) retornam um ponteiro para o objeto recem-criado,
que pertence ao `BellezaSystem` (nao deve ser deletado manualmente).
