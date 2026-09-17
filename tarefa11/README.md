# Tarefa 11: Particionamento de dados e balanceamento de carga

## Escopo do enunciado

Conforme a Tarefa 11 em `assets/index.md` (slide 36):

- Simular somente a viscosidade com diferenças finitas e evolução temporal.
- Validar os campos parado e constante e a difusão de uma pequena perturbação.
- Paralelizar com OpenMP e comparar `schedule` e `collapse`.

O núcleo já implementa esses requisitos: `v0_seq.c` é a referência sequencial;
`v1`/`v2` usam static, `v3`/`v4` dynamic e `v6`/`v5` guided, sem/com collapse.
Os sete arquivos são independentes. Não é o sistema completo de Navier–Stokes:
advecção, pressão e forças externas são omitidas.

## Validar e executar

A partir de `tarefa11/`:

```bash
bash test.sh
gcc -std=c11 -O2 -Wall -Wextra -fopenmp v0_seq.c -lm -o ns_v0
gcc -std=c11 -O2 -Wall -Wextra -fopenmp v1_static.c -lm -o ns_v1
./ns_v0 1
./ns_v0 2
OMP_NUM_THREADS=4 ./ns_v1 campo.txt
bash execute.sh
python3 plot_results.py runs/EXECUCAO/raw.csv
```

Malha 512×512, 500 passos, definidos no código. Somente a v0 recebe
`[modo [arquivo-campo]]`: `0` = gaussiana, `1` = zero, `2` = constante unitária.
As versões v1–v6 sempre simulam a gaussiana e recebem apenas `[arquivo-campo]`.
A exportação é opcional e fica fora da temporização. As bordas são nulas na
gaussiana e iguais ao campo constante nos modos de validação da v0.

`test.sh` compila com avisos tratados como erros, verifica os três modos na v0
e compara 18 campos gaussianos completos das paralelas com a referência,
usando 1, 2 e 4 threads. Também verifica as interfaces e falhas de saída.
`execute.sh` mede de 1 até o número de núcleos físicos detectados pelo `lscpu`, com cinco repetições e aquecimento;
guarda CSV e metadados em `runs/`. Usa GCC C11, `-O2 -Wall -Wextra`, `-lm` e
`-fopenmp` em todas as versões para usar `omp_get_wtime()`. A v0 continua sequencial, sem região paralela. `CC`, `OUT`, `OMP_PROC_BIND` e `OMP_PLACES`
são configuráveis. No NPAD, executar dentro da alocação de um nó de computação.
Os testes exigem GCC/OpenMP e Python; os gráficos exigem Matplotlib.

## Resultados e relatório

O conjunto medido está em `runs/20260917-152818-2/`. O gráfico de tempo e `visualizacao_difusao.png`
documentam o experimento de 500 passos. O relatório principal está em
[relatorio.pdf](relatorio.pdf), com fonte em `relatorio.tex`.

## Extras separados

[extras/](extras/) preserva a animação de 2.100.000 passos, seus geradores e testes,
o estudo adicional de chunks, o protocolo ampliado de benchmark e a versão
completa anterior do relatório. Nada disso é necessário para compilar, validar
ou medir as sete versões acima. Instruções: [extras/README.md](extras/README.md).

Os gráficos e a tabela foram refeitos com `omp_get_wtime()`, cinco repetições por configuração e intervalo de um segundo entre medições. Os dados brutos e metadados desta coleta estão em `runs/20260917-152818-2/`.
