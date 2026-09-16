# Extras da Tarefa 11

Material complementar, separado do escopo mínimo do enunciado.
Os fontes principais continuam em `../v*.c`; não há cópias independentes deles aqui.

- `execute_chunks.sh`, `plot_chunks.py`, `chunksize_plot.png` e `runs/chunks-*`: estudo de chunks equivalentes. O experimento substitui o chunk fixo 16 apenas no texto enviado ao compilador, sem modificar os fontes principais.
- `execute_stable.sh` e `runs/final-stable/`: benchmark com ordem aleatória, pausas e dez repetições.
- `gerar_quadros.c`, `plot_visualizacao.py`, `difusao3d.js`, imagens, quadros e `test_visualizacao.cjs`: visualização complementar.
- `relatorio-completo.tex` e `relatorio-completo.pdf`: relatório anterior preservado, incluindo o estudo de chunks.
- `roteiro-defesa.md`: material local de apresentação, mantido fora do versionamento.

A partir de `tarefa11/`:

```bash
bash extras/execute_chunks.sh
python3 extras/plot_chunks.py extras/runs/CHUNKS/raw_chunks.csv
OUT=extras/runs/nova-execucao bash extras/execute_stable.sh
python3 plot_results.py extras/runs/nova-execucao/raw.csv
# Regenerar a figura de 500 passos usada no relatório principal:
python3 extras/plot_visualizacao.py
# Compilar o relatório completo preservado:
cd extras
pdflatex -interaction=nonstopmode -halt-on-error relatorio-completo.tex
pdflatex -interaction=nonstopmode -halt-on-error relatorio-completo.tex
```

Os scripts de benchmark resolvem os fontes a partir de `tarefa11/` e, por padrão,
gravam em `extras/runs/`. O diretório informado em `OUT` também é relativo a `tarefa11/`.
Resultados locais e quadros binários continuam ignorados pelo Git.

## Demonstração de 2.100.000 passos


A página da lição reproduz 201 quadros (do passo 0 ao 2.100.000), a 30 quadros/s,
em cerca de 6,7 segundos. O cálculo ocorre previamente em `gerar_quadros.c`, na malha
512×512 em double, com o mesmo stencil e parâmetros de `v0_seq.c`. O pico cai cerca
de 99,04%, restando menos de 1% da altura inicial. Os campos zero e unitário são exibidos como estados invariantes exatos.
Os sete programas de benchmark, resultados e figura do relatório mantêm 500 passos.

```bash
# A partir da raiz do repositório; requer GCC, NumPy e Matplotlib.
python3 tarefa11/extras/plot_visualizacao.py --animacao
node tarefa11/extras/test_visualizacao.cjs
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
