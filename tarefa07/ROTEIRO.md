# Roteiro de estudo da Tarefa 07: processamento paralelo de lista encadeada com OpenMP task

Este roteiro guia o estudo dos arquivos incrementais `e0.c` a `e5.c`. Cada arquivo introduz um erro ou uma correção no uso de tarefas OpenMP para processar uma lista encadeada.

---

## Etapa 0. Versão base sequencial (`e0.c`)

Objetivo: entender a estrutura da lista encadeada antes de introduzir paralelismo.

`e0.c` constrói a lista com 5 nomes de arquivo e a percorre sequencialmente. Mostra a saída esperada sem qualquer diretiva OpenMP.

```bash
gcc -Wall -o e0 e0.c
./e0
```

---

## Etapa 1. Abordagem ingênua (`e2.c`)

Objetivo: observar o erro de colocar o laço diretamente dentro de `#pragma omp parallel` e quantificá-lo.

`e2.c` coloca o `while` dentro de `parallel` sem `single`, e usa um contador para mostrar que o resultado é $N \times T$ processamentos.

```bash
gcc -Wall -fopenmp -o e2 e2.c
OMP_NUM_THREADS=4 ./e2
```

---

## Etapa 2. Quantificar o problema (`e2.c`)

Objetivo: medir o excesso de processamento com um contador.

`e2.c` adiciona um contador `processados` para confirmar a relação $Y = N \times T$.

```bash
gcc -Wall -fopenmp -o e2 e2.c
OMP_NUM_THREADS=4 ./e2
```

---

## Etapa 3. `single` sem `firstprivate` (`e3.c`)

Objetivo: entender por que o ponteiro da travessia precisa ser copiado.

`e3.c` usa `single` (só uma thread cria tarefas), mas `shared(atual)` faz todas as tarefas compartilharem o mesmo ponteiro, que avança enquanto as tarefas esperam.

```bash
gcc -Wall -fopenmp -o e3 e3.c
OMP_NUM_THREADS=4 ./e3
```

---

## Etapa 4. `firstprivate` sem `taskwait` (`e4.c`)

Objetivo: mostrar que capturar o ponteiro não basta sem sincronizar o fim.

`e4.c` usa `firstprivate(atual)` — cada tarefa captura o endereço correto. Mas sem `taskwait`, a região paralela pode encerrar antes do fim das tarefas.

```bash
gcc -Wall -fopenmp -o e4 e4.c
OMP_NUM_THREADS=4 ./e4
```

---

## Etapa 5. Versão correta (`e5.c`)

Objetivo: aplicar os três elementos que garantem o processamento correto.

`e5.c` combina `single` + `firstprivate(atual)` + `taskwait`. Cada nó é processado exatamente uma vez.

```bash
gcc -Wall -fopenmp -o e5 e5.c
OMP_NUM_THREADS=4 ./e5
```

---

## Lista de autoavaliação

- [ ] A versão sequencial (`e0.c`) cria a lista e executa sem erros de memória.
- [ ] A versão sem `single` (`e2.c`) demonstra a multiplicação do processamento ($N \times T$).
- [ ] O papel de `single` na separação entre thread produtora e threads consumidoras está claro.
- [ ] A cláusula `firstprivate(atual)` garante a cópia do endereço correto para cada tarefa.
- [ ] A diretiva `taskwait` sincroniza o encerramento da região paralela.
- [ ] O comportamento não-determinístico na atribuição de threads foi observado e compreendido.
