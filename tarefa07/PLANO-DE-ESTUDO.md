# Plano de estudo da Tarefa 07: processamento paralelo de lista encadeada com OpenMP task

Este plano organiza o estudo teórico e a experimentação prática para a Tarefa 07, focando no modelo de execução de tarefas do OpenMP e na manipulação de estruturas dinâmicas em paralelo.

---

## Lista de verificação

- [ ] Lição 06 lida (`lessons/0006-tarefa07-lista-task.html`)
- [ ] Teoria 06 lida (`reference/06-teoria-task-openmp.html`)
- [ ] Referência 06 consultada (`reference/06-task-openmp.html`)
- [ ] Lição 07 concluída (`lessons/0007-tarefa07-taskwork-stealing.html`)
- [ ] Roteiro de implementação executado (`tarefa07/ROTEIRO.md`)
- [ ] Códigos de teste `e0.c` a `e6.c` compilando sem warnings
- [ ] Relatório da Tarefa 07 elaborado

---

## Parte 1. Leitura e fundamentação teórica

Tempo estimado: 30 a 45 minutos.

| Ordem | Documento | Conteúdo principal |
|---|---|---|
| 1 | `reference/06-teoria-task-openmp.html` | Teoria de tarefas, roubo de trabalho e ciclo de vida |
| 2 | `reference/06-task-openmp.html` | Sintaxe de diretivas e cláusulas de escopo |
| 3 | `lessons/0006-tarefa07-lista-task.html` | Código dissecado com `single`, `task` e `taskwait` |
| 4 | `lessons/0007-tarefa07-taskwork-stealing.html` | Experimentos com comportamento não-determinístico |
| 5 | `tarefa07/RESPOSTAS.md` | Respostas detalhadas de cada etapa |
| 6 | `tarefa07/ROTEIRO.md` | Roteiro prático de implementação |

### Arquivos de referência em `tarefa07/`
| Arquivo | Etapa | Mecanismo analisado |
|---|---|---|
| `e0.c` | 0 | Versão base sequencial |
| `e1.c` | 1 | Abordagem ingênua (`parallel` sem `single`) |
| `e2.c` | 2 | Contador atômico quantificando o excesso ($N \times T$) |
| `e3.c` | 3 | `single` sem `firstprivate`: ponteiro compartilhado inválido |
| `e4.c` | 4 | `single` com `firstprivate`, sem `taskwait` |
| `e5.c` | 5 | Solução sincronizada e correta |
| `e6.c` | 6 | Versão configurável com $N$ dinâmico e medição de tempo |

### Perguntas centrais desta etapa
1. Por que `parallel for` não se aplica diretamente a listas encadeadas sem vetorização prévia?
2. Como funciona o roubo de trabalho (*work-stealing*) entre threads ociosas e ocupadas?
3. Quais são as diferenças funcionais entre `taskwait`, `taskgroup` e `taskyield`?

---

## Parte 2. Experimentos práticos isolados

Tempo estimado: 60 a 90 minutos.

Os experimentos isolados constroem intuição prática sobre o runtime do OpenMP antes da implementação do programa principal.

### Experimento A. Identificação das threads
- Arquivo: `tarefa07/exp_a_threads.c`
- Conteúdo: região `parallel` com `omp_get_thread_num()` e `omp_get_num_threads()`.
- Objetivo: verificar a criação do time de threads e a ordem variável de saída no terminal.
- Referência: Seção 2 da Lição 07.

### Experimento B. Comportamento da diretiva `single`
- Arquivo: `tarefa07/exp_b_single.c`
- Conteúdo: região `parallel` com bloco `single` interno e mensagens dentro e fora do bloco.
- Objetivo: confirmar que o bloco `single` executa uma única vez e analisar o efeito de `nowait`.
- Referência: Seção 3 da Lição 07.

### Experimento C. Escopo de iteração (`shared` versus `firstprivate`)
- Arquivo: `tarefa07/exp_c_shared_vs_firstprivate.c`
- Conteúdo: laço `for` dentro de `single` gerando tarefas; testar `shared(i)` e depois `firstprivate(i)`.
- Objetivo: observar que `shared(i)` faz todas as tarefas lerem o valor final do laço.
- Referência: Seção 4 da Lição 07.

### Experimento D. Multiplicação de tarefas sem `single`
- Arquivo: `tarefa07/exp_d_sem_single.c`
- Conteúdo: laço gerando tarefas diretamente em `parallel` sem `single`, com contador atômico.
- Objetivo: comprovar a relação $\text{total} = \text{iterações} \times T$.
- Referência: Seção 5 da Lição 07.

### Experimento E. Encerramento prematuro sem `taskwait`
- Arquivo: `tarefa07/exp_e_taskwait.c`
- Conteúdo: tarefas com carga de trabalho artificial; execução com e sem `taskwait`.
- Objetivo: observar o encerramento da região paralela antes da finalização das tarefas pendentes.
- Referência: Seção 6 da Lição 07.

### Experimento F. Observação de roubo de trabalho
- Arquivo: `tarefa07/exp_f_workstealing.c`
- Conteúdo: thread única gerando 12 tarefas com atraso de 1 segundo; contagem de execuções por thread.
- Objetivo: verificar a redistribuição dinâmica de tarefas para threads ociosas.
- Referência: Seção 7 da Lição 07.

### Instruções de compilação dos experimentos
```bash
gcc -Wall -fopenmp -o exp_a exp_a_threads.c
export OMP_NUM_THREADS=4
./exp_a
```

---

## Parte 3. Implementação guiada da Tarefa 07

Tempo estimado: 45 a 60 minutos.

Siga as etapas descritas em `tarefa07/ROTEIRO.md`:

| Etapa | Foco | Verificação |
|---|---|---|
| Etapa 0 | Estudar `e0.c` (versão sequencial base) | Compilação e execução corretas |
| Etapa 1 | Introduzir `parallel` e `task` sem `single` | Contagem de impressões igual a $N \times T$ |
| Etapa 2 | Quantificar repetições com contador atômico | Diagnóstico do excesso de trabalho |
| Etapa 3 | Inserir `single` para geração única de tarefas | Contagem de tarefas igual a $N$ |
| Etapa 4 | Adicionar `firstprivate(current)` | Cada nó processa o arquivo correto |
| Etapa 5 | Inserir `taskwait` | Região paralela aguarda conclusão de todas as tarefas |

---

## Parte 4. Análise e validação dos resultados

Tempo estimado: 20 a 30 minutos.

Responda aos seguintes pontos:
1. Quantas vezes cada nó foi processado na versão sem `single` e na versão com `single`?
2. Qual seria o resultado caso fosse usado `private(current)` em vez de `firstprivate(current)`?
3. Por que a diretiva `taskwait` é necessária mesmo quando as tarefas apenas imprimem dados na saída padrão?
4. Quais fatores variam entre execuções e quais permanecem determinísticos?
5. Como formular em uma única sentença a regra para processamento seguro de listas com tarefas OpenMP?

---

## Parte 5. Tópicos avançados opcionais

1. **`taskgroup` versus `taskwait`:** avaliar cenários com tarefas aninhadas em árvore onde `taskgroup` oferece sincronização profunda.
2. **Dependências com `depend(in: ...)` e `depend(out: ...)`:** expressar grafos de tarefas com restrições de precedência.
3. **Conversão para vetor de ponteiros:** comparar a abordagem de percorrer a lista uma vez para preencher um vetor e aplicar `#pragma omp parallel for`.
4. **Perfil de desempenho:** medir tempos de execução sequencial versus paralelo para listas de 10 a 100.000 nós.

---

## Materiais de consulta

| Tipo | Arquivo | Finalidade |
|---|---|---|
| Roteiro prático | `tarefa07/ROTEIRO.md` | Guia passo a passo de desenvolvimento |
| Teoria de hardware e runtime | `reference/06-teoria-task-openmp.html` | Mecânica do escalonador e modelo de memória |
| Consulta rápida | `reference/06-task-openmp.html` | Sintaxe de diretivas e cláusulas OpenMP |
| Lição de código | `lessons/0006-tarefa07-lista-task.html` | Análise linha a linha do padrão correto |
| Lição de experimentos | `lessons/0007-tarefa07-taskwork-stealing.html` | Demonstração do modelo de roubo de trabalho |
| Fundamentos gerais | `reference/00-fundamentos-programacao-paralela.html` | Leis de Amdahl, Gustafson e conceitos de HPC |
