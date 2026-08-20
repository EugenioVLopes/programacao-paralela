# DCA3703 — Programação Paralela

Repositório de implementações, análises de desempenho, relatórios técnicos e materiais didáticos interativos das atividades práticas da disciplina **DCA3703 (Programação Paralela)** do Departamento de Engenharia de Computação e Automação da **Universidade Federal do Rio Grande do Norte (UFRN)**, ministrada pelo **Prof. Samuel Xavier de Souza**.

---

## 🌐 Portal de Lições Interativas (GitHub Pages)

Os materiais de estudo práticos e teóricos podem ser visualizados diretamente no navegador:

👉 **[Acessar o Portal DCA3703 no GitHub Pages](index.html)** *(ou abra [`index.html`](index.html) localmente)*

---

## 📚 Mapeamento de Tarefas e Materiais Didáticos

Cada tarefa possui uma **tríade documental integrada** acompanhada de código C e relatório em LaTeX:

| Tarefa | Tema / Conceito Chave | Lição Prática Interativa | Teoria Aprofundada | Referência Rápida | Código C & Relatório |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Geral** | Fundamentos de HPC & Taxonomia | — | [00. Fundamentos Gerais](reference/00-fundamentos-programacao-paralela.html) | — | — |
| **01** | Série de Leibniz, Precisão Numérica & Erro | [Lição 01](lessons/0001-tarefa01-leibniz-pi.html) | [Teoria 01](reference/01-teoria-ponto-flutuante-hardware.html) | [Referência 01](reference/01-ponto-flutuante-e-leibniz.html) | [`tarefa01/`](tarefa01/) &bull; [PDF](tarefa01/relatorio.pdf) |
| **02** | Multiplicação Matriz-Vetor (MxV) & Cache | [Lição 02](lessons/0002-tarefa02-mxv-cache.html) | [Teoria 02](reference/02-teoria-hierarquia-memoria-cache.html) | [Referência 02](reference/02-mxv-cache-localidade.html) | [`tarefa02/`](tarefa02/) &bull; [PDF](tarefa02/relatorio.pdf) |
| **03** | Paralelismo em Nível de Instrução (ILP) | [Lição 03](lessons/0003-tarefa03-ilp-acumuladores.html) | [Teoria 03](reference/03-teoria-ilp-pipeline-superscalar.html) | [Referência 03](reference/03-ilp-acumuladores-otimizacao.html) | [`tarefa03/`](tarefa03/) &bull; [PDF](tarefa03/relatorio.pdf) |

---

## 🏗️ Estrutura do Repositório

```text
.
├── index.html                   # Portal inicial para GitHub Pages
├── assets/
│   ├── style.css                # Folha de estilo compartilhada (claro/escuro, responsivo)
│   └── scientific_report.sty    # Pacote de formatação LaTeX de relatórios
├── lessons/                     # Lições práticas interativas com quizzes simétricos
│   ├── 0001-tarefa01-leibniz-pi.html
│   ├── 0002-tarefa02-mxv-cache.html
│   └── 0003-tarefa03-ilp-acumuladores.html
├── reference/                   # Fundamentação teórica física e referências rápidas
│   ├── 00-fundamentos-programacao-paralela.html
│   ├── 01-teoria-ponto-flutuante-hardware.html
│   ├── 01-ponto-flutuante-e-leibniz.html
│   ├── 02-teoria-hierarquia-memoria-cache.html
│   ├── 02-mxv-cache-localidade.html
│   ├── 03-teoria-ilp-pipeline-superscalar.html
│   └── 03-ilp-acumuladores-otimizacao.html
├── tarefa01/                    # Código C, script de benchmark e relatório (Leibniz)
│   ├── tarefa01.c
│   ├── execute.sh
│   ├── plot_results.py
│   └── relatorio.tex (e relatorio.pdf)
├── tarefa02/                    # Código C, benchmark e relatório (MxV row vs column)
│   ├── row_major.c
│   ├── column_major.c
│   ├── execute.sh
│   ├── plot_results.py
│   └── relatorio.tex (e relatorio.pdf)
└── tarefa03/                    # Código C, benchmark e relatório (ILP & acumuladores)
    ├── tarefa03.c
    ├── execute.sh
    ├── plot_results.py
    └── relatorio.tex (e relatorio.pdf)
```

---

## ⚡ Como Executar os Experimentos

### 1. Compilação e Execução dos Códigos em C

Todos os programas foram desenvolvidos no padrão **C99/C11** para ambiente Linux x86_64 e nós de supercomputação do NPAD/UFRN.

Exemplo de execução da **Tarefa 03 (ILP)**:

```bash
cd tarefa03/

# Executar benchmark automatizado nos níveis -O0, -O2 e -O3
chmod +x execute.sh
./execute.sh

# Gerar gráfico de análise
python3 plot_results.py
```

### 2. Visualização Local dos Documentos HTML

Para abrir as lições e referências localmente com renderização matemática via KaTeX e temas interativos:

```bash
# Opção A: Abrir diretamente no navegador padrão
xdg-open index.html

# Opção B: Servidor HTTP local simples em Python
python3 -m http.server 8080
# Acesse no navegador: http://localhost:8080
```

---

## 📖 Padrões de Design e Metodologia

- **Visual e Tipografia**: Folha de estilos [`assets/style.css`](assets/style.css) com tipografia limpa (*Crimson Pro*, *Inter*, *JetBrains Mono*), paleta adaptativa com modo escuro automático/manual e destaque de sintaxe em CSS puro.
- **Renderização Matemática**: Renderizador **KaTeX** com delimitadores `$...$` e `$$...$$` para fórmulas de latência, vazão e speedup.
- **Medição de Tempo de Alta Precisão**: Utilização de `clock_gettime(CLOCK_MONOTONIC)` com resolução de nanossegundos.
- **Quizzes Simétricos**: Questões interativas com alternativas de contagem de palavras e comprimento simétricos para avaliação sem viés de extensão de texto.

---

## 👤 Autor

- **Eugênio Vitor Lopes dos Santos** — UFRN (DCA3703)
