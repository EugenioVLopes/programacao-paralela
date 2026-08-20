# Diretrizes para agentes (DCA3703)

Este repositório contém as implementações e análises de desempenho das atividades práticas da disciplina DCA3703 (Programação Paralela) da UFRN, ministrada pelo Prof. Samuel Xavier de Souza.

## 1. Regra para o fluxo de ensino (/teach)

Para cada tarefa da disciplina (Tarefas 01 a 13), o agente deve gerar **três documentos integrados**:

### A. Documento de teoria aprofundada em ./reference/ (ex: `reference/01-teoria-ponto-flutuante-hardware.html`)
Documento teórico dedicado à tarefa, cobrindo:
1. **Fundamentos de hardware**: Explicação física dos gargalos da tarefa (Memory Wall, Power Wall, hierarquia de cache, linhas de 64 bytes, dependências RAW, falso compartilhamento, NUMA).
2. **Modelagem matemática**: Fórmulas de convergência, erro de truncamento, representação numérica (IEEE 754), speedup, leis de Amdahl e Gustafson.
3. **Contexto em HPC**: Motivação do problema e limites de hardware testados.

### B. Documento de referência rápida em ./reference/ (ex: `reference/01-ponto-flutuante-e-leibniz.html`)
Guia resumido para consulta rápida, contendo:
1. **Tabelas de sintaxe**: Tipos C, funções da biblioteca padrão, pragmas e flags do compilador.
2. **Resumo executivo**: Fórmulas essenciais, constantes e regras práticas.

### C. Lição prática em ./lessons/ (ex: `lessons/0001-tarefa01-leibniz-pi.html`)
Arquivo HTML autocontido focado na prática e na teoria aplicada, contendo:
1. **Análise do código C**: Explicação linha a linha de tipos, ponteiros, laços e chamadas de sistema.
2. **Conceitos de programação paralela**:
   - **Poder computacional**: Definição e aplicações reais que demandam maior capacidade (simulações científicas, previsão do tempo, IA, modelos de linguagem).
   - **Limitação de processadores sequenciais**: Limites físicos espaciais e energéticos, estagnação de frequência e barreira térmica (Power Wall).
   - **Cores lógicos**: Transição do aumento de frequência em núcleo único para o uso de múltiplos núcleos lógicos operando paralelamente.
3. **Quizzes simétricos**: Questões de múltipla escolha com opções de tamanho e contagem de palavras idênticos.
4. **Navegação**: Links relativos para o documento de teoria da tarefa, referência rápida da tarefa e documento geral `reference/00-fundamentos-programacao-paralela.html`.

## 2. Portabilidade e links relativos

- **Caminhos relativos**: Todo link em arquivos Markdown ou HTML deve usar caminhos relativos (ex: `../assets/style.css`, `reference/01-ponto-flutuante-e-leibniz.html`, `../tarefa01/tarefa01.c`).
- **Proibição de caminhos absolutos**: Não use caminhos absolutos do sistema (como `/home/...` ou `file:///...`) para manter o repositório funcional ao ser clonado do GitHub.

## 3. Padrões de código e compilação

- **Linguagem**: C (padrão C99 ou C11) em ambiente Linux x86_64.
- **Compilador**: GCC ou Clang com flags explícitas (`-O0`, `-O2`, `-O3`, `-fopenmp`, `-lm`, `-Wall`).
- **Medição de tempo**: Usar `clock_gettime(CLOCK_MONOTONIC)` com `#define _POSIX_C_SOURCE 199309L`.
- **Ambiente de execução**: Nós de computação do NPAD/UFRN (suporte a NUMA e políticas `OMP_PROC_BIND` e `OMP_PLACES`).

## 4. Padrões visuais e HTML

- **Estilos**: Usar a folha de estilo compartilhada [assets/style.css](assets/style.css).
- **Matemática**: Renderização com KaTeX configurado para reconhecer delimitadores `$ ... $` e `$$ ... $$` via `applyMath()`.
- **Caracteres especiais**: Escapar operadores relacionais em blocos `<pre><code>` (`&lt;` e `&gt;`).
