# DCA3703: Programação Paralela

Implementações, relatórios e lições práticas da disciplina DCA3703 (Programação Paralela) da Universidade Federal do Rio Grande do Norte (UFRN), ministrada pelo Prof. Samuel Xavier de Souza.

---

## Portal de lições

Os materiais de estudo podem ser acessados diretamente no GitHub Pages:

🔗 **[https://eugeniovlopes.github.io/programacao-paralela/](https://eugeniovlopes.github.io/programacao-paralela/)**

*(Para visualização local, abra [`index.html`](index.html) no navegador)*

---

## Mapeamento de tarefas

Cada tarefa tem lição prática, teoria dedicada, guia de consulta rápida, código C e relatório em LaTeX.

| Tarefa | Tema | Lição prática | Teoria | Referência rápida | Código e relatório |
| :--- | :--- | :--- | :--- | :--- | :--- |
| Geral | Fundamentos de HPC e taxonomia | — | [Fundamentos gerais](reference/00-fundamentos-programacao-paralela.html) | — | — |
| 01 | Série de Leibniz e precisão numérica | [Lição 01](lessons/0001-tarefa01-leibniz-pi.html) | [Teoria 01](reference/01-teoria-ponto-flutuante-hardware.html) | [Referência 01](reference/01-ponto-flutuante-e-leibniz.html) | [`tarefa01/`](tarefa01/) e [PDF](tarefa01/relatorio.pdf) |
| 02 | Multiplicação matriz-vetor e cache | [Lição 02](lessons/0002-tarefa02-mxv-cache.html) | [Teoria 02](reference/02-teoria-hierarquia-memoria-cache.html) | [Referência 02](reference/02-mxv-cache-localidade.html) | [`tarefa02/`](tarefa02/) e [PDF](tarefa02/relatorio.pdf) |
| 03 | Paralelismo em nível de instrução (ILP) | [Lição 03](lessons/0003-tarefa03-ilp-acumuladores.html) | [Teoria 03](reference/03-teoria-ilp-pipeline-superscalar.html) | [Referência 03](reference/03-ilp-acumuladores-otimizacao.html) | [`tarefa03/`](tarefa03/) e [PDF](tarefa03/relatorio.pdf) |

---

## Estrutura do repositório

```text
.
├── index.html                   # Página inicial para o GitHub Pages
├── assets/
│   ├── style.css                # Estilo das páginas HTML
│   └── scientific_report.sty    # Pacote LaTeX dos relatórios
├── lessons/                     # Lições com quizzes
│   ├── 0001-tarefa01-leibniz-pi.html
│   ├── 0002-tarefa02-mxv-cache.html
│   └── 0003-tarefa03-ilp-acumuladores.html
├── reference/                   # Teoria de hardware e tabelas de consulta
│   ├── 00-fundamentos-programacao-paralela.html
│   ├── 01-teoria-ponto-flutuante-hardware.html
│   ├── 01-ponto-flutuante-e-leibniz.html
│   ├── 02-teoria-hierarquia-memoria-cache.html
│   ├── 02-mxv-cache-localidade.html
│   ├── 03-teoria-ilp-pipeline-superscalar.html
│   └── 03-ilp-acumuladores-otimizacao.html
├── tarefa01/                    # Código C, script de teste e relatório de Leibniz
│   ├── tarefa01.c
│   ├── execute.sh
│   ├── plot_results.py
│   └── relatorio.tex
├── tarefa02/                    # Código C, teste e relatório de acesso à matriz
│   ├── row_major.c
│   ├── column_major.c
│   ├── execute.sh
│   ├── plot_results.py
│   └── relatorio.tex
└── tarefa03/                    # Código C, teste e relatório de ILP com acumuladores
    ├── tarefa03.c
    ├── execute.sh
    ├── plot_results.py
    └── relatorio.tex
```

---

## Como executar os experimentos

### Compilar e rodar os programas em C

Os códigos seguem o padrão C11 para Linux x86_64.

Exemplo com a tarefa 03:

```bash
cd tarefa03/

# Executa o benchmark nos níveis -O0, -O2 e -O3
chmod +x execute.sh
./execute.sh

# Gera o gráfico de tempos
python3 plot_results.py
```

### Visualizar os arquivos HTML

Para abrir as lições no navegador:

```bash
# Abrir direto no navegador
xdg-open index.html

# Ou subir um servidor local
python3 -m http.server 8080
```

---

## Detalhes técnicos

- A folha [`assets/style.css`](assets/style.css) define tipografia com fontes Crimson Pro, Inter e JetBrains Mono, com suporte a tema claro e escuro.
- Fórmulas matemáticas usam KaTeX com delimitadores `$...$` e `$$...$$`.
- Medições de tempo usam `clock_gettime(CLOCK_MONOTONIC)` com resolução em nanossegundos.
- As alternativas dos quizzes têm tamanhos de texto equivalentes para evitar pistas pelo comprimento da resposta.

---

## Autor

Eugênio Vitor Lopes dos Santos, aluno de Engenharia de Computação na UFRN.
