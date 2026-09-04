# Learning Record 0002: plano de estudo da Tarefa 07

## Data
2025-09-01

## Contexto
O estudante solicitou um plano de estudo complementar ao roteiro de implementação (`tarefa07/ROTEIRO.md`), com foco no entendimento do modelo de tarefas do OpenMP.

## Diagnóstico da zona de desenvolvimento proximal
O estudante domina os pré-requisitos fundamentais (listas encadeadas, escopo de variáveis OpenMP e distinção entre `shared`, `private` e `firstprivate`). A progressão foca no modelo de escalonamento dinâmico e na experimentação prática.

## Materiais criados

### Lição 07: experimentos práticos de aprofundamento
- Arquivo: `lessons/0007-tarefa07-taskwork-stealing.html`
- Conteúdo:
  1. Modelo de execução de tarefas (*work-stealing*).
  2. Experimento A: identificação de threads.
  3. Experimento B: comportamento da diretiva `single`.
  4. Experimento C: escopo de `shared(i)` versus `firstprivate(i)`.
  5. Experimento D: repetição de tarefas sem `single`.
  6. Experimento E: encerramento prematuro sem `taskwait`.
  7. Experimento F: observação do roubo de trabalho.
  8. Tabela de decisões de projeto.
  9. Questionário de fixação.

### Plano de estudo
- Arquivo: `tarefa07/PLANO-DE-ESTUDO.md`
- Estrutura em cinco partes:
  1. Fundamentação teórica com os documentos do repositório.
  2. Experimentos práticos isolados.
  3. Implementação guiada passo a passo.
  4. Análise e validação dos resultados.
  5. Tópicos avançados opcionais (`taskgroup`, `depend`, vetorização e tempo de execução).

### Atualização do portal de lições
- `index.html` atualizado com o link para a Lição 07 no card da Tarefa 07.

## Próximos passos
1. Leitura de `reference/06-teoria-task-openmp.html`.
2. Execução do Experimento C em `lessons/0007-tarefa07-taskwork-stealing.html`.
3. Desenvolvimento do código conforme `tarefa07/ROTEIRO.md`.

## Notas
Os experimentos isolados foram planejados para execução antes da edição do código principal, permitindo testar cada hipótese de isolamento de falhas.
