# BellezaSys

Sistema de automação de agendamentos para salões de beleza, desenvolvido como parte da disciplina de Engenharia de Software.

## Objetivo

O BellezaSys tem como missão organizar e automatizar o processo de agendamento de salões de beleza, permitindo que clientes consultem serviços, preços e profissionais disponíveis de forma estruturada, reduzindo conflitos de horários e melhorando a eficiência operacional do negócio.

## Funcionalidades principais

* Cadastro e login de usuários (clientes, funcionários e administradores)
* Agendamento, remarcação e cancelamento de horários
* Consulta de disponibilidade de profissionais em tempo real
* Gestão financeira (fluxo de caixa e comissões)
* Assistente virtual e lembretes automáticos via WhatsApp

## Estrutura

```
.
├── src/      # Código-fonte principal
├── include/  # Arquivos de cabeçalho
├── bin/      # Arquivos compilados
├── test/     # Testes do sistema
└── doc/      # Documentação
```

## Compilação

Para compilar o projeto:

```bash
make
```

## Testes

Para executar os testes:

```bash
make test
```

## Versionamento

O projeto segue o modelo Gitflow, com as seguintes branches:

* `master`
* `develop`
* `staging`
