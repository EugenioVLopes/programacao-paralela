# Tarefa 11 — difusão viscosa 2D

Implementação de uma componente escalar sob viscosidade apenas. Nesta simplificação de Navier–Stokes, advecção, pressão e forças externas são omitidas; portanto, o programa não resolve o sistema incompressível completo.

## Validar e executar

```bash
bash test.sh
gcc -std=c11 -O2 -Wall -Wextra v0_seq.c -lm -o ns_v0
gcc -std=c11 -O2 -Wall -Wextra -fopenmp v1_static.c -lm -o ns_v1
OMP_NUM_THREADS=4 ./ns_v1 2
./ns_v0 0 campo.txt
bash execute.sh
python3 plot_results.py runs/EXECUCAO/raw.csv
# Protocolo mais estável: 10 repetições, pausa e ordem aleatória
OUT=runs/final-stable ./execute_stable.sh
python3 plot_results.py runs/final-stable/raw.csv
```

A constante `tamanho_grid=512` representa uma malha de 512×512 células; `num_passos_tempo=500` determina 500 atualizações temporais. Ambas ficam definidas diretamente em cada código. Interface: `[modo [arquivo-campo]]`. Modo 0: gaussiana; 1: zero; 2: constante unitária. Contornos Dirichlet fixos (zero nos modos 0/1; um no modo 2). A exportação textual da grade ocorre fora da temporização.

Cada `v*.c` é completo e independente, com validação de entrada, condições iniciais, cronômetro e exportação. A duplicação é intencional para estudo: os pragmas estão escritos diretamente, sem macros de seleção ou header comum. Compare com `diff -u v1_static.c v2_static_collapse.c`. Ao corrigir a lógica numérica, replique a mudança nos sete arquivos e rode os testes. v0 é sequencial; v1/v2 static sem/com collapse; v3/v4 dynamic,16 sem/com collapse; v6/v5 guided,16 sem/com collapse. A região paralela é persistente; barreiras do for e single protegem cada troca de buffers.

Os testes aprovam 30 comparações exatas de campos completos de 512×512, preservam os campos parado e constante, verificam a queda do pico perturbado e rejeitam entradas inválidas. Testes finitos não provam convergência nem ausência de toda falha possível.

`execute.sh` percorre automaticamente de 1 até `nproc` threads, executa cinco repetições e preserva tempos e metadados em diretório exclusivo, sem sobrescrever resultados anteriores. O CSV contém somente `version,threads,rep,seconds`. `execute_chunks.sh` compara chunks equivalentes a 1, 4, 16 e 64 linhas usando oito threads. `CC`, `OUT`, `OMP_PROC_BIND` e `OMP_PLACES` continuam configuráveis. Usa O2, sem fast-math. Os gráficos requerem matplotlib; o teste requer somente GCC/OpenMP e Python padrão. No NPAD, execute dentro da alocação de um nó de computação.

## Demonstração de 2.100.000 passos

A página da lição reproduz 201 quadros (do passo 0 ao 2.100.000), a 30 quadros/s,
em cerca de 6,7 segundos. O cálculo ocorre previamente em `gerar_quadros.c`, na malha
512×512 em double, com o mesmo stencil e parâmetros de `v0_seq.c`. O pico cai cerca
de 99,04%, restando menos de 1% da altura inicial. Os campos zero e unitário são exibidos como estados invariantes exatos.
Os sete programas de benchmark, resultados e figura do relatório mantêm 500 passos.

```bash
# A partir da raiz do repositório; requer GCC, NumPy e Matplotlib.
python3 tarefa11/plot_visualizacao.py --animacao
node tarefa11/test_visualizacao.cjs
# Servir a página por HTTP para permitir o carregamento dos quadros.
python3 -m http.server 8000
# Abrir /lessons/0011-tarefa11-difusao-schedule.html#visualizacao-3d
```

O Python compila e executa o gerador e cria `visualizacao_animacao.png` e
`quadros_difusao.bin` (3.397.724 bytes), ambos necessários à publicação da página.
Sem `--animacao`, o script continua gerando apenas a figura original de 500 passos.
O JavaScript mantém rotação, pausa, reinício e busca imediata entre os quadros;
em dispositivos lentos, pula quadros para manter a duração. As escalas ficam fixas.
Se o carregamento falhar, a figura estática continua disponível.

Formato do arquivo: float32 little-endian; cabeçalho de cinco valores
`[512, 65, 2100000, 201, 0.2]` (grade, amostras por eixo, passos totais, quadros, Δt), seguido
de 201 registros com o pico da malha completa e 65×65 amostras em ordem de linhas.
O quadro de índice `k` (0 a 200) corresponde ao passo `round(2100000*(k/200)²)`,
arredondando empates para cima: 0, 53, 210, 473, …, 2.100.000. Esse espaçamento
quadrático concentra quadros no início; a velocidade do tempo simulado varia durante
a reprodução. Os índices espaciais amostrados são `round(k*511/64)`. A conversão para float32 ocorre somente
na exportação, com erro absoluto inferior a 4×10⁻⁹ neste campo. O gerador não mede
desempenho; sua saída inclui E/S. A geração executa todos os 2,1 milhões de passos
e pode levar vários minutos. O teste compara os passos 53 e 21.000 com a referência
C e o pico final com os modos próprios do stencil discreto, além de validar a
sequência de passos, bordas, queda do pico, modos invariantes e entradas inválidas.

## Resultados e relatório

O benchmark foi executado com grade 512×512 e 500 passos. Os pontos de 1 a 25 threads usam cinco repetições; 26 a 28 threads foram confirmados com dez repetições devido aos picos da primeira varredura. O conjunto consolidado está em `runs/resultado-confirmado/`; os gráficos são `tempo_execucao_plot.png`, `tempo_execucao_log_plot.png` e `speedup_plot.png`. O menor tempo global foi 0,0183 s com `static` sem `collapse` e 16 threads, speedup 5,72. O estudo de chunks está em `runs/chunks-20260911-152008-206654/` e em `chunksize_plot.png`.

`relatorio.tex` segue a formatação das Tarefas 09 e 10: identificação centralizada, páginas simples, seções de enunciado, fundamentação, implementação, resultados, discussão e conclusão, além de apêndice com os sete códigos completos. `relatorio.pdf` é a versão compilada.
