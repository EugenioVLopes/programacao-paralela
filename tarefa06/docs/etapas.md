# Roteiro passo a passo — Estimativa estocástica de π com OpenMP

Este é um guia para você **implementar sozinho**, na ordem certa. Cada
passo diz *o que* fazer e *por quê*, mas não entrega o código pronto —
você decide como escrever cada trecho. Sempre que um passo pedir para
"pensar" em algo, é porque a resposta não é única: existe mais de uma
forma correta e vale parar e refletir antes de codar.

Sugestão de organização: crie 3 arquivos-fonte, um por etapa —
`v1_race.c`, `v2_critical.c` e `v3_clauses.c` — reaproveitando o código
de um para o próximo.

---

## Etapa 0 — Preparar o esqueleto sequencial

Antes de paralelizar qualquer coisa, tenha uma versão sequencial
funcionando e correta. Isso te dá um "gabarito" para comparar os
resultados das versões paralelas depois.

1. Inclua os cabeçalhos necessários: `<stdio.h>` (para `printf`),
   `<stdlib.h>` (para `atol` e `rand_r`), `<math.h>` (para `fabs` e a
   constante `M_PI`) e `<omp.h>` (para as funções e diretivas OpenMP —
   sem esse include, `#pragma omp` até compila em alguns casos, mas
   funções como `omp_get_thread_num()` não existirão).

2. Declare uma variável `long N` para o número total de amostras.
   Pense: por que `long` e não `int`? (Dica: para valores de N na casa
   dos milhões/bilhões, `int` de 32 bits pode não ser suficiente
   dependendo da plataforma — `long` é mais seguro para contagens
   grandes.)

3. Declare o contador de acertos, algo como `long dentro_circulo = 0;`,
   **fora** de qualquer laço, inicializado em zero.

4. Escreva um laço `for` de `i = 0` até `N`, sorteando dentro dele dois
   números `x` e `y` no intervalo `[-1, 1]`.
   - Pense em como transformar o retorno de `rand()` (que vai de `0` a
     `RAND_MAX`) para o intervalo `[-1, 1]`. Dica: primeiro normalize
     para `[0, 1]` dividindo por `RAND_MAX`, depois mapeie para
     `[-1, 1]` com uma multiplicação e um deslocamento.

5. Dentro do laço, teste a condição de pertencimento ao círculo
   unitário (`x² + y² ≤ 1`) e, se verdadeira, incremente o contador de
   acertos.

6. Depois do laço, calcule a estimativa de π com a fórmula
   `4 * dentro_circulo / N` — cuidado com **divisão inteira**: pense em
   por que o resultado dá errado se você não fizer casting para
   `double` em algum ponto dessa conta.

7. Imprima `N`, o contador de acertos e a estimativa de π, comparando
   com `M_PI` (de `<math.h>`) usando `fabs()` para calcular o erro
   absoluto.

8. Compile com `gcc -O2 arquivo.c -o saida -lm` (o `-lm` é necessário
   por causa de `fabs`/`M_PI`) e rode algumas vezes para confirmar que
   o resultado é sempre plausível (perto de 3.14159...).

---

## Etapa 1 — Paralelizar de forma ingênua (para observar o problema)

Objetivo desta etapa: **produzir o bug de propósito**, para depois
entender exatamente o que ele causa.

9. Copie o arquivo da Etapa 0 para uma nova versão. Adicione, logo
   acima do laço `for`, a diretiva `#pragma omp parallel for`.
   - Pense: essa única linha faz duas coisas ao mesmo tempo — cria um
     time de threads **e** distribui as iterações entre elas. Escreva
     em um comentário, com suas próprias palavras, o que cada uma das
     duas partes (`parallel` e `for`) contribui separadamente.

10. Rode o programa várias vezes seguidas (num laço de shell, por
    exemplo `for i in 1 2 3; do ./programa; done`) e observe se o
    valor de `dentro_circulo` muda entre execuções.
    - Se você tiver acesso a uma máquina com vários núcleos, force
      várias threads com `OMP_NUM_THREADS=8 ./programa` antes de
      rodar. Quanto mais núcleos reais e quanto mais rápido for o
      corpo do laço, maior a chance do problema aparecer visivelmente.

11. Escreva, em um comentário no topo do arquivo, sua hipótese do que
    está causando qualquer inconsistência observada (ou, se você não
    conseguiu observar nenhuma, escreva por que a ausência de erro
    visível **não** significa que o código está correto). Pense
    especificamente em: quantas instruções de máquina são necessárias
    para executar `contador++`? O que acontece se duas threads
    executarem essas instruções entrelaçadas?

12. **Desafio opcional de diagnóstico:** escreva um segundo programa
    minúsculo — sem a lógica de Monte Carlo, só um laço que incrementa
    um contador compartilhado milhões de vezes dentro de um
    `#pragma omp parallel for`, sem nenhuma amostragem aleatória no
    meio. Compile com `-O0` (sem otimização) e rode várias vezes.
    Pense em por que esse programa "puro" tende a evidenciar o bug de
    forma mais clara e repetível do que a versão com `rand_r`.

---

## Etapa 2 — Corrigir com `#pragma omp critical`

13. Copie o arquivo da Etapa 1 para uma nova versão. Localize a linha
    onde o contador de acertos é incrementado.

14. Envolva **apenas essa linha** (não o laço inteiro!) com
    `#pragma omp critical` seguido de um bloco `{ }`.
    - Pense: por que envolver o laço inteiro com `critical` seria um
      erro grave de desempenho? O que sobraria de paralelo se você
      fizesse isso?

15. Recompile e rode várias vezes. Confirme que `dentro_circulo` (e,
    portanto, a estimativa de π) agora é **sempre o mesmo valor**,
    execução após execução.

16. Meça o tempo de execução (você pode usar `omp_get_wtime()` antes e
    depois da região paralela, ou o comando `time` do shell). Rode com
    diferentes valores de `OMP_NUM_THREADS` (1, 2, 4, 8...) e anote os
    tempos em uma tabela.
    - Pense: o tempo melhora de forma proporcional ao número de
      threads? Se não, por quê? (Dica: pense em quantas vezes, no
      total, o programa entra na seção crítica, e o que cada entrada
      custa quando há disputa entre threads.)

---

## Etapa 3 — Reestruturar com `parallel` + `for` separados e cláusulas

Esta etapa tem dois objetivos: (a) reduzir a contenção da seção
crítica, acumulando localmente por thread; e (b) tornar o escopo de
cada variável explícito.

17. Copie o arquivo da Etapa 2 para uma nova versão. Separe a diretiva
    única em duas: `#pragma omp parallel` (com um bloco `{ }` logo
    abaixo) contendo, dentro dele, `#pragma omp for` sobre o mesmo
    laço de antes.
    - Pense: qual código deveria ficar **dentro do bloco `parallel`
      mas fora do `for`** (executado uma vez por thread) e qual deve
      ficar **dentro do `for`** (executado uma vez por iteração)?
      Um candidato natural para "uma vez por thread" é a preparação da
      semente do gerador de números aleatórios de cada thread — pense
      em por que cada thread precisa de uma semente diferente das
      outras.

18. Dentro do bloco `parallel`, declare uma variável local (por
    exemplo, um contador de acertos "local" à thread), inicializada em
    zero. Como ela é declarada dentro do escopo léxico da região
    paralela, ela já nasce privada a cada thread automaticamente —
    mas pense em como você comprovaria isso, na prática, se tivesse
    dúvida.

19. Dentro do `for`, troque o incremento do contador global (protegido
    por `critical`) pelo incremento do contador **local**. Não deve
    mais existir `critical` dentro do laço.
    - Pense: por que essa mudança já elimina a condição de corrida,
      mesmo sem nenhum `critical` dentro do `for`?

20. Depois do `for` (ainda dentro do bloco `parallel`, mas fora dele),
    adicione um `#pragma omp critical` que soma o contador local no
    contador global (compartilhado).
    - Pense: quantas vezes, no total, essa seção crítica será
      executada agora, comparado com a Etapa 2? Qual é a relação entre
      esse número e a quantidade de threads usadas?

21. Agora, classifique **explicitamente** cada variável usada dentro do
    bloco `parallel`, adicionando a cláusula `default(none)` na
    diretiva `#pragma omp parallel` e, para cada variável referenciada
    lá dentro, decida e adicione a cláusula correta:
    - `N` (só leitura, mesmo valor para todas as threads) → qual
      cláusula?
    - o contador **global** de acertos (escrito, mas protegido por
      `critical`, e precisa ser o mesmo para todas) → qual cláusula?
    - alguma semente ou valor inicial que todas as threads devem
      **herdar** de fora da região paralela, mas cada uma modifica só
      a própria cópia → qual cláusula? (Não é `private` puro — pense
      em qual cláusula preserva o valor inicial.)
    - Tente compilar **antes** de classificar todas as variáveis, só
      para ver a mensagem de erro que o compilador te dá. Leia com
      atenção — ela diz exatamente qual variável falta classificar.

22. **Desafio com `lastprivate`:** adicione uma variável extra (por
    exemplo, um `int` que guarda o identificador da thread,
    `omp_get_thread_num()`) que é atualizada a cada iteração do laço
    dentro do `for`. Adicione a cláusula `lastprivate` para essa
    variável na diretiva `#pragma omp for` (não na `#pragma omp
    parallel` — pense em por que essa cláusula só é válida em
    construtos de work-sharing como `for`).
    - Depois de compilar e rodar, pense: qual valor você espera ver
      impresso para essa variável após o laço terminar? Rode o
      programa várias vezes e confirme se o valor é sempre o mesmo,
      mesmo que a ordem real de término das threads mude a cada
      execução.

23. Recompile, rode várias vezes com diferentes `OMP_NUM_THREADS`, e
    confirme que o resultado de π continua correto e determinístico.
    Meça o tempo de novo e compare com a tabela da Etapa 2.
    - Pense: o ganho de desempenho, se houver, vem de mais
      paralelismo real ou de menos tempo gasto esperando na seção
      crítica? Como você distinguiria as duas causas em uma medição?

---

## Etapa 4 — Testar combinações de cláusulas (para consolidar o entendimento)

Nesta etapa, o objetivo não é mais chegar em π corretamente, e sim
**observar o comportamento de cada cláusula isoladamente**, usando
programinhas pequenos e descartáveis.

24. Escreva um programa mínimo com uma variável `x` inicializada com
    algum valor (por exemplo, `100`) antes de uma região `#pragma omp
    parallel for private(x)`. Dentro do laço, apenas imprima o valor
    de `x` **antes de atribuir qualquer coisa a ela**.
    - Pense, antes de rodar: você espera ver `100` impresso, ou outra
      coisa? Rode e confira sua previsão.

25. Troque `private(x)` por `firstprivate(x)` no mesmo programa (sem
    mudar mais nada) e rode de novo.
    - Compare a saída com a do passo anterior. Escreva, em um
      comentário, a diferença de comportamento entre as duas
      cláusulas.

26. Depois do laço paralelo (de volta à região sequencial), imprima o
    valor de `x` novamente nas duas versões (`private` e
    `firstprivate`).
    - Pense: em qual dos dois casos (ou em nenhum) o valor de `x` fora
      da região paralela é alterado pelo que aconteceu dentro dela?

27. Escreva um terceiro programa pequeno demonstrando `lastprivate`:
    um laço curto (por exemplo, 20 iterações) que atribui a uma
    variável um valor previsível e calculável a partir do índice `i`
    (por exemplo, `i * i`). Rode o programa várias vezes e verifique
    se o valor impresso após o laço é sempre o mesmo — e se ele
    corresponde à fórmula aplicada à **última iteração do laço**
    (índice `N-1`), e não a alguma thread específica.

28. Escreva um quarto programa pequeno só para "quebrar" de propósito:
    use `default(none)` em uma região paralela e **esqueça** de
    classificar alguma variável usada lá dentro. Tente compilar.
    - Leia a mensagem de erro do compilador com atenção. Ela indica o
      nome exato da variável e a linha onde o problema ocorre — é
      assim que `default(none)` ajuda a pegar esse tipo de erro antes
      mesmo de rodar o programa.

---

## Checklist final

Ao terminar, você deve ter três programas funcionando (`v1`, `v2`,
`v3`) e quatro programas pequenos de demonstração. Antes de considerar
concluído, confira:

- [ ] `v1` roda e produz π aproximadamente correto na maioria das
      vezes, mas você entende (mesmo sem necessariamente ter observado
      visualmente) por que ele é incorreto.
- [ ] `v2` sempre produz o mesmo resultado, execução após execução.
- [ ] `v3` também sempre produz o mesmo resultado, e é mais rápido que
      `v2` (ou, no mínimo, você entende por que deveria ser mais
      rápido, mesmo que o hardware disponível não deixe isso evidente).
- [ ] Você consegue explicar, sem olhar para nenhum código, a
      diferença entre `shared`, `private`, `firstprivate` e
      `lastprivate` usando um exemplo próprio (não precisa ser o de
      π).
- [ ] Você consegue explicar por que `default(none)` é considerada boa
      prática em programas OpenMP não triviais.