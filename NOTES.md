# Notas e mapa de tarefas da DCA3703

## Contexto do curso
- **Disciplina**: DCA3703 (Programação Paralela, UFRN)
- **Docente**: Prof. Samuel Xavier de Souza
- **Infraestrutura**: Linux local e nós do cluster NPAD/UFRN
- **Sequência da turma**:
  - Tarefa 01: Série de Leibniz (pasta `tarefa01`) ✅
  - Tarefa 02: Multiplicação Matriz-Vetor Row vs Col-Major (pasta `tarefa02`) ✅
  - Tarefa 03: ILP e laços (pasta `tarefa03`)
  - Tarefas 04 a 13: Módulos seguintes do programa

## Documentos criados

### Tarefa 01 (Leibniz)
| Tipo | Arquivo |
|------|---------|
| Teoria aprofundada | `reference/01-teoria-ponto-flutuante-hardware.html` |
| Referência rápida | `reference/01-ponto-flutuante-e-leibniz.html` |
| Lição prática | `lessons/0001-tarefa01-leibniz-pi.html` |

### Tarefa 02 (MxV — Cache e Localidade)
| Tipo | Arquivo |
|------|---------|
| Teoria aprofundada | `reference/02-teoria-hierarquia-memoria-cache.html` |
| Referência rápida | `reference/02-mxv-cache-localidade.html` |
| Lição prática | `lessons/0002-tarefa02-mxv-cache.html` |

**Temas cobertos (T02)**: gargalo de von Neumann, Memory Wall, hierarquia L1/L2/L3, linhas de cache de 64 bytes, localidade espacial e temporal, row-major vs column-major, modelo Roofline (I = 0,25 FLOP/byte), NUMA, `clock()` vs `clock_gettime()`.

## Roteiro das 13 tarefas

### Módulo 1: Linha de base sequencial e microarquitetura
- **Tarefa 01 (`tarefa01`)**: Série de Leibniz para $\pi$. Ponto flutuante `double`, erro de truncamento e medição de convergência.
- **Tarefa 02 (`tarefa02`)**: Multiplicação Matriz-Vetor (MxV). Acesso à memória contígua em C, linhas de cache de 64 bytes e impacto de cache misses.
- **Tarefa 03 (`tarefa03`)**: Paralelismo em nível de instrução (ILP). Quebra de dependências RAW com múltiplos acumuladores e flags `-O0`, `-O2` e `-O3`.

### Módulo 2: Multitarefas e primeiros passos em OpenMP
- **Tarefa 04**: Laços memory-bound vs compute-bound com OpenMP e avaliação de SMT.
- **Tarefa 05**: Contagem de primos com `#pragma omp parallel for` e análise de desbalanceamento de carga.

### Módulo 3: Memória compartilhada e tarefas
- **Tarefa 06**: Estimativa de $\pi$ com Monte Carlo, correção de corrida com `critical` e escopos de variáveis (`private`, `firstprivate`, `shared`, `default(none)`).
- **Tarefa 07**: Processamento de lista encadeada com `#pragma omp task`, `taskwait` e `single`.

### Módulo 4: Coerência de cache
- **Tarefa 08**: Falso compartilhamento com Monte Carlo, vetor por thread, `rand` vs `rand_r` e protocolo MESI.

### Módulo 5: Sincronização
- **Tarefa 09**: Inserção em listas com seções críticas nomeadas vs locks explícitos (`omp_lock_t`).
- **Tarefa 10**: Comparação quantitativa entre `atomic`, `critical`, variáveis privadas e `reduction`.

### Módulo 6: Aplicação, escalabilidade e cluster
- **Tarefa 11**: Navier-Stokes 2D simplificado (difusão) com `collapse` e `schedule`.
- **Tarefa 12**: Medição de escalabilidade forte e fraca no cluster NPAD.
- **Tarefa 13**: Afinidade de threads (`OMP_PROC_BIND` e `OMP_PLACES`) em nós NUMA do NPAD.
