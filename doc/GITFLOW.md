# Gitflow do BellezaSys

## Branches principais

- `master`: codigo estavel de entrega.
- `develop`: integracao das funcionalidades do grupo.
- `staging`: validacao antes da entrega final.

## Fluxo de trabalho

1. Criar uma branch a partir de `develop`, por exemplo `feature/core-agendamento`.
2. Fazer commits pequenos e com mensagens objetivas.
3. Abrir pull request para `develop`.
4. Testar com `make test` antes de integrar.
5. Promover `develop` para `staging` quando a versao estiver pronta para validacao.
6. Promover `staging` para `master` apenas na entrega estavel.

## Sugestoes de divisao

- Pessoa 1: nucleo de usuarios, servicos e profissionais.
- Pessoa 2: regras de agendamento e disponibilidade.
- Pessoa 3: financeiro e relatorios.
- Pessoa 4: interface Qt e integracao visual.
- Pessoa 5: persistencia, testes e documentacao.
