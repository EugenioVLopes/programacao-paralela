# Respostas ao roteiro da Tarefa 07: processamento paralelo de lista encadeada com OpenMP task

Este documento acompanha o arquivo `ROTEIRO.md` da Tarefa 07. Cada seção corresponde a uma etapa do roteiro, explicando o comportamento do código, o erro demonstrado e o resultado observado na execução. Os arquivos-fonte correspondentes são `e0.c` a `e6.c`.

---

## Etapa 0. Versão base sequencial (`e0.c`)

### Comportamento do código
Constrói uma lista encadeada com 5 nomes de arquivo fictícios e percorre a lista sequencialmente, imprimindo cada nome.

### Decisões de implementação
- O nó usa `char filename[128]` em vez de `int data`, atendendo ao enunciado.
- `strncpy` com limite evita estouro de buffer.
- `insert_at_end` preserva a ordem de inserção.

### Verificação esperada
```text
=== Lista sequencial ===
Arquivo: documento1.txt
Arquivo: imagem2.png
Arquivo: dados3.csv
Arquivo: relatorio4.pdf
Arquivo: backup5.zip
```

---

## Etapa 1. Abordagem ingênua (`e1.c`)

### Comportamento do código
Coloca o laço `while` de travessia diretamente dentro de `#pragma omp parallel`. Cada thread executa o laço completo e cria uma tarefa para cada nó.

### Causa do erro
No OpenMP, todas as threads do time executam o código contido na região `parallel`. Sem a diretiva `single`, todas as $T$ threads percorrem a lista inteira. Cada thread cria $N$ tarefas, totalizando $N \times T$ tarefas criadas. Cada nó acaba processado $T$ vezes.

### Observação na execução
```bash
$ OMP_NUM_THREADS=4 ./e1
```
- Total de impressões: 5 nós $\times$ 4 threads = 20 linhas. Como o `taskwait` não está no código, algumas tarefas podem ser perdidas se a região terminar antes.
- Múltiplas threads aparecem na saída.
- A ordem das linhas varia entre execuções.

---

## Etapa 2. Observação e documentação do problema (`e2.c`)

### Comportamento do código
Mantém a estrutura de `e1.c` e adiciona um contador global `processados` protegido por `#pragma omp atomic` para quantificar o erro.

### Observação na execução
```bash
$ OMP_NUM_THREADS=4 ./e2
Nos na lista (N): 5
Processamentos totais (Y): 20
Relacao: Y = N x T = 5 x 4 = 20 (esperado)
```

### Explicação do mecanismo
```text
Todas as 4 threads entram na regiao #pragma omp parallel.
Cada thread executa o while completo:
    Thread 0: cria task(doc1), task(doc2), task(doc3), task(doc4), task(doc5)
    Thread 1: cria task(doc1), task(doc2), task(doc3), task(doc4), task(doc5)
    Thread 2: cria task(doc1), task(doc2), task(doc3), task(doc4), task(doc5)
    Thread 3: cria task(doc1), task(doc2), task(doc3), task(doc4), task(doc5)
Total: 4 x 5 = 20 tasks.
Cada task incrementa o contador uma vez.
Resultado: processados = 20, em vez de 5.
```

---

## Etapa 3. `single` sem `firstprivate` (`e3.c`)

### Comportamento do código
O laço `while` passa para dentro de `#pragma omp single`. Apenas uma thread percorre a lista e cria as tarefas. Porém, a cláusula da tarefa é `shared(current)`, compartilhando o mesmo ponteiro `current` entre todas as tarefas.

### Causa do erro
O laço `while` avança `current` para o próximo nó imediatamente após criar cada tarefa. As tarefas são enfileiradas e executadas de forma assíncrona por threads que roubam trabalho da fila. Quando a tarefa começa a rodar, o ponteiro `current` já avançou para outro nó ou atingiu `NULL`.

### Observação na execução
```bash
$ OMP_NUM_THREADS=4 ./e3
```
- Várias tarefas imprimem `(NULL)` ou o nome do último nó.
- A chamada `sleep(1)` dentro da tarefa evidencia a falha, pois o laço `while` termina antes de as tarefas começarem.

### Sequência temporal do erro
```text
Momento da criacao:         Momento da execucao (1s depois):
current = n1 (doc1)         current ja chegou em NULL
    cria task(shared:current)     task executa: current = NULL -> imprime NULL
current = n2 (doc2)
    cria task(shared:current)     task executa: current = NULL -> imprime NULL
...
current = NULL (fim do while)
```

---

## Etapa 4. `single` com `firstprivate`, sem `taskwait` (`e4.c`)

### Comportamento do código
Usa `firstprivate(current)`. Cada tarefa recebe uma cópia privada de `current`, inicializada com o valor do ponteiro no momento da criação da tarefa.

### Diferença entre `firstprivate` e `private`
- `private(current)` cria uma cópia por tarefa sem inicialização, contendo lixo de memória.
- `firstprivate(current)` cria a cópia e copia o valor que `current` tinha quando a tarefa foi instanciada.

### Limitação restante
Falta a diretiva `#pragma omp taskwait`. Se a função `main` termina logo após a região paralela, o time de threads encerra e tarefas pendentes são interrompidas antes da conclusão.

### Observação na execução
```bash
$ OMP_NUM_THREADS=4 ./e4
```
- Cada nó imprime o nome correto do arquivo.
- O contador final pode ser menor que 5 porque tarefas pendentes não concluem a tempo.
- O identificador da thread varia conforme o escalonador do OpenMP distribui o trabalho.

---

## Etapa 5. `single` com `firstprivate` e `taskwait` (`e5.c`)

### Comportamento do código
Versão correta e completa com três elementos essenciais:
1. `single`: apenas uma thread percorre a lista e gera as tarefas.
2. `firstprivate(current)`: cada tarefa captura o endereço do nó no momento da criação.
3. `taskwait`: barreira explícita que aguarda o término de todas as tarefas antes de fechar a região paralela.

### Observação na execução
```bash
$ OMP_NUM_THREADS=4 ./e5
Nos na lista: 5 | Processamentos: 5 (devem ser iguais!)
```
- Exatamente 5 impressões, uma por nó.
- Threads distintas executam tarefas distintas por meio de roubo de trabalho (*work-stealing*).
- A ordem de execução varia entre rodadas, mas a contagem total permanece constante.

---

## Etapa 6. Versão final configurável (`e6.c`)

### Adições implementadas
- Número de nós dinâmico via argumento de linha de comando (`./e6 50`).
- Medição de tempo com `clock_gettime(CLOCK_MONOTONIC)` e `#define _POSIX_C_SOURCE 199309L`.
- Carga de trabalho variável com `usleep` para demonstrar o balanceamento dinâmico.
- Verificação automática entre $N$ e a contagem de tarefas concluídas.
- Liberação explícita da lista com `free_list`.

### Como executar
```bash
gcc -Wall -fopenmp -o e6 e6.c
OMP_NUM_THREADS=4 ./e6 20
```

---

## Resumo das respostas às perguntas do enunciado

### Todos os nós foram processados?
- `e1.c`, `e2.c`: Sim, mas com repetições em excesso ($N \times T$ vezes).
- `e3.c`: Não de forma confiável. Muitas tarefas leem ponteiro nulo ou valores sobrescritos.
- `e4.c`: A maioria, mas algumas tarefas podem ser canceladas prematuramente sem `taskwait`.
- `e5.c`, `e6.c`: Sim, exatamente uma vez por nó.

### Algum nó foi processado mais de uma vez ou ignorado?
- `e1.c`, `e2.c`: Processados $T$ vezes cada.
- `e3.c`: Nós iniciais são ignorados e substituídos pelo nó final devido ao ponteiro compartilhado.
- `e4.c`: Nós finais podem ser ignorados se a execução terminar antes das tarefas.
- `e5.c`, `e6.c`: Nenhum nó é repetido e nenhum é ignorado.

### O comportamento muda entre execuções?
- O que varia: a thread responsável por executar cada tarefa e a ordem temporal das impressões.
- O que não varia: a quantidade total de processamentos por nó nas versões corretas (`e5.c` e `e6.c`).

### Como garantir que cada nó seja processado uma única vez e por apenas uma tarefa?
```c
#pragma omp parallel
{
    #pragma omp single
    {
        Node *current = head;
        while (current != NULL) {
            #pragma omp task firstprivate(current)
            {
                printf("%s | thread %d\n",
                       current->filename,
                       omp_get_thread_num());
            }
            current = current->next;
        }
    }
    #pragma omp taskwait
}
```

---

## Tabela de verificação das versões

| Arquivo | Compila sem warning | Nós processados | Nome correto | `taskwait` presente |
|---|---|---|---|---|
| `e0.c` | Sim | N (sequencial) | Sim | N/A |
| `e1.c` | Sim | N x T (repetido) | Sim | Não |
| `e2.c` | Sim | N x T (quantificado) | Sim | Não |
| `e3.c` | Sim | Variável (nulo/lixo) | Não | Não |
| `e4.c` | Sim | Menor ou igual a N | Sim | Não |
| `e5.c` | Sim | Exatamente N | Sim | Sim |
| `e6.c` | Sim | Exatamente N | Sim | Sim |
