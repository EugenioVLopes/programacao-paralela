# Diretrizes para agentes (DCA3703)

Este repositório contém as implementações e análises de desempenho das atividades práticas da disciplina DCA3703 (Programação Paralela) da UFRN, ministrada pelo Prof. Samuel Xavier de Souza.

## 1. Regra para o fluxo de ensino (/teach)

Para cada tarefa da disciplina (Tarefas 01 a 13), o agente deve gerar **um único documento HTML** em `./lessons/`, reunindo teoria e prática.

### Documento único da tarefa (ex: `lessons/0001-tarefa01-leibniz-pi.html`)

O documento deve cobrir, sem repetir conteúdo:

1. **Fundamentos de hardware**: Explicação física dos gargalos da tarefa (Memory Wall, Power Wall, gargalo de von Neumann, hierarquia de cache, linhas de 64 bytes, write-through vs write-back, dirty/invalid bits, snooping vs diretório, dependências RAW, falso compartilhamento, NUMA). Usar `assets/index.md` (slides do professor) como fonte primária do vocabulário e dos objetivos do tópico.
2. **Modelagem matemática**: Fórmulas de convergência, erro de truncamento, representação numérica (IEEE 754), speedup, leis de Amdahl e Gustafson.
3. **Contexto em HPC**: Motivação do problema e limites de hardware testados.
4. **Apêndice de referência rápida** (seção final obrigatória no mesmo arquivo):
   - **Tabelas de sintaxe**: Tipos C, funções da biblioteca padrão, pragmas e flags do compilador.
   - **Resumo executivo**: Fórmulas essenciais, constantes e regras práticas.
   - Deve usar `<section id="referencia-rapida">` com `<h2>Apêndice: Referência rápida</h2>` para âncora direta.

5. **Prática**: análise linha a linha do código C e medição do que foi executado de verdade (flags, threads, N e interpretação das curvas).
6. **Quizzes simétricos**: criar ao menos quatro questões de múltipla escolha quando o conteúdo permitir, com opções de tamanho e contagem de palavras idênticos.
7. **Navegação**: usar links relativos para o documento geral `reference/00-fundamentos-programacao-paralela.html`, código e relatório.

Não criar documentos teóricos separados em `reference/`. Os nomes antigos nessa pasta existem apenas como redirecionamentos de compatibilidade.

## 2. Portabilidade, GitHub Pages e links

- **Caminhos relativos internos**: Todo link dentro dos arquivos HTML deve usar caminhos relativos (ex: `../assets/style.css`, `reference/01-ponto-flutuante-e-leibniz.html`, `../tarefa01/tarefa01.c`).
- **Links no README.md**:
  - Materiais HTML (lições, teoria e referência rápida) devem usar URLs absolutas do GitHub Pages: `https://eugeniovlopes.github.io/programacao-paralela/...` para abrir a página renderizada diretamente no navegador.
  - Códigos C e relatórios PDF devem usar links relativos do repositório (`tarefaXX/` e `tarefaXX/relatorio.pdf`).
- **Atualização contínua de índices**: Ao concluir qualquer tarefa, atualizar obrigatoriamente:
  1. `index.html` na raiz (adicionar o card da tarefa com links para o material único — incluindo âncora `#referencia-rapida` se preciso — e relatório).
  2. `reference/00-fundamentos-programacao-paralela.html` (adicionar a linha da tarefa na tabela de mapeamento).
  3. `README.md` (adicionar a linha na tabela de tarefas com links para o GitHub Pages).

## 3. Padrões de Git e arquivos ignorados

- **Commits atômicos por tarefa**: Estruturar mensagens no padrão Conventional Commits (`feat(tarefaXX): ...`, `docs: ...`, `chore: ...`).
- **Arquivos não versionados**:
  - Roteiros de apresentação e podcast (`*roteiro*.md`, `*podcast*.md`) não devem ser commitados.
  - Binários compilados e artefatos de build LaTeX (`.aux`, `.log`, `.out`, `.fls`, `.fdb_latexmk`) devem permanecer ignorados via `.gitignore`.

## 4. Padrões de código e compilação

- **Linguagem**: C (padrão C99 ou C11) em ambiente Linux x86_64.
- **Compilador**: GCC ou Clang com flags explícitas (`-O0`, `-O2`, `-O3`, `-fopenmp`, `-lm`, `-Wall`).
- **Medição de tempo**: Usar `clock_gettime(CLOCK_MONOTONIC)` com `#define _POSIX_C_SOURCE 199309L`.
- **Ambiente de execução**: Nós de computação do NPAD/UFRN (suporte a NUMA e políticas `OMP_PROC_BIND` e `OMP_PLACES`).

## 5. Padrões visuais e HTML

- **Estilos**: Usar a folha de estilo compartilhada [assets/style.css](assets/style.css).
- **Matemática**: Renderização com KaTeX configurado para reconhecer delimitadores `$ ... $` e `$$ ... $$` via `applyMath()`.
- **Caracteres especiais**: Escapar operadores relacionais em blocos `<pre><code>` (`&lt;` e `&gt;`).
