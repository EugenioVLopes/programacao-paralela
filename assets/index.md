DCA3703
PROGRAMAÇÃO
Prof. Samuel Xavier
de Souza
PARALELA

HISTÓRICO E
| • Objetivos desse | tópico |     |     |     |
| ----------------- | ------ | --- | --- | --- |
EVOLUÇÃO DA  • Reconhecer a necessidade de maior conhecimento do
| hardware para alcançar |     | alto desempenho |     |     |
| ---------------------- | --- | --------------- | --- | --- |
PROGRAMAÇÃO
| • Reconhecer             | a necessidade | da programação | paralela | para  |
| ------------------------ | ------------- | -------------- | -------- | ----- |
| alcançar alto desempenho |               |                |          |       |
2

HISTÓRICO E
• O gargalo de von Neumman e a memória cache
EVOLUÇÃO DA • Localidade temporal e espacial
• Row-major vs. column-major
PROGRAMAÇÃO
Tarefa 1:
Implemente duas versões da
multiplicação de matriz por vetor (MxV)
em C: uma com acesso à matriz por
linhas (laço interno variando coluna) e
outra por colunas (laço interno variando
linha). Meça o tempo de execução de
cada versão com uma função apropriada
e execute testes com diferentes
tamanhos de matriz. Identifique a partir
de que tamanho os tempos passam a
divergir significativamente e explique por
que isso ocorre, relacionando suas
observações com o uso da memória
cache e o padrão de acesso à memória.
3

HISTÓRICO E
• ILP – Paralelismo ao nível de instrução
EVOLUÇÃO DA • Pipeline
• Vetorização
PROGRAMAÇÃO
4

HISTÓRICO E
• Paralelismo ao nível de instrução
EVOLUÇÃO DA • Pipelining,
• Vetorização
PROGRAMAÇÃO
5

HISTÓRICO E
• Paralelismo ao nível de instrução
EVOLUÇÃO DA • Pipelining,
• Vetorização
PROGRAMAÇÃO
Tarefa 2:
Implemente três laços em C para
investigar os efeitos do paralelismo ao
nível de instrução (ILP): 1) inicialize um
vetor com um cálculo simples; 2) some
seus elementos de forma acumulativa,
criando dependência entre as iterações;
e 3) quebre essa dependência utilizando
múltiplas variáveis. Compare o tempo de
execução das versões compiladas com
diferentes níveis de otimização (O0, O2,
O3) e analise como o estilo do código e
as dependências influenciam o
desempenho.
6

HISTÓRICO E
| • Por que algumas | aplicações | requerem | maior | poder |
| ----------------- | ---------- | -------- | ----- | ----- |
EVOLUÇÃO DA
| computacional | continuamente? |     |     |     |
| ------------- | -------------- | --- | --- | --- |
PROGRAMAÇÃO
7

HISTÓRICO E
• Simulações científicas para descoberta de novas drogas
EVOLUÇÃO DA
PROGRAMAÇÃO
Silva, DyonatanFonseca. "Investigaçãoin silico e in vitro do efeitoantiparasitárioda drogafebrifuginaem
Trypanosoma cruzi." (2020).
8

HISTÓRICO E
• Modelagem de sistemas para previsão de tempo
EVOLUÇÃO DA
PROGRAMAÇÃO
Silva, DyonatanFonseca. "Investigaçãoin silico e in vitro do efeitoantiparasitárioda drogafebrifuginaem
Trypanosoma cruzi." (2020).
9

HISTÓRICO E
• Treinamentos de modelos de IA
EVOLUÇÃO DA
PROGRAMAÇÃO
Silva, DyonatanFonseca. "Investigaçãoin silico e in vitro do efeitoantiparasitárioda drogafebrifuginaem
Trypanosoma cruzi." (2020).
10

HISTÓRICO E
• As limitações físicas da fabricação de processadores sequenciais
mais rápidos
EVOLUÇÃO DA
PROGRAMAÇÃO
11

HISTÓRICO E
• As limitações físicas da fabricação de processadores sequenciais
mais rápidos
EVOLUÇÃO DA
PROGRAMAÇÃO
Tarefa 3:
Implemente um programa em C que
calcule uma aproximação de π usando
uma série matemática, variando o
número de iterações e medindo o tempo
de execução. Compare os valores
obtidos com o valor real de π e analise
como a acurácia melhora com mais
processamento. Reflita sobre como esse
comportamento se repete em aplicações
reais que demandam resultados cada vez
mais precisos, como simulações físicas e
inteligência artificial.
12

PROGRAMAS
| • Objetivos desse | tópico |     |     |     |
| ----------------- | ------ | --- | --- | --- |
MULTITAREFAS • Reconhecer a necessidade de maior conhecimento do software
| para alcançar | alto desempenho |             |                 |             |
| ------------- | --------------- | ----------- | --------------- | ----------- |
| • Reconhecer  | a maior         | dificuldade | de se programar | em paralelo |
13

PROGRAMAS
• Quais programas são limitados pela memória (gargalo de
von Neuman) e quais não são?
MULTITAREFAS
| •     | Como o multi-thread | de hardware pode ajudar (ou atrapalhar) |            |
| ----- | ------------------- | --------------------------------------- | ---------- |
| $ gcc | -fopenmp            | meuprog.c                               | -o meuprog |
$ export OMP_NUM_THREADS=2; time meuprog
14

PROGRAMAS
• Quais programas são limitados pela memória (gargalo de
von Neuman) e quais não são?
MULTITAREFAS
• Como o multi-thread de hardware pode ajudar (ou atrapalhar)
Tarefa 4:
Implemente dois programas paralelos em
C com OpenMP: um limitado por
memória, com somas simples em vetores,
e outro limitado por CPU, com cálculos
matemáticos intensivos. Paralelize com
#pragma omp parallel for e
meça o tempo de execução variando o
número de threads. Analise quando o
desempenho melhora, estabiliza ou piora,
e reflita sobre como o multithreading de
hardware pode ajudar em programas
memory-bound, mas atrapalhar em
programas compute-bound pela
competição por recursos.
15

PROGRAMAS
| • Por que é mais | difícil | programar | em paralelo? |
| ---------------- | ------- | --------- | ------------ |
MULTITAREFAS • Sincronização, comunicação e equilíbrio de carga
16

PROGRAMAS
| • Por que é mais | difícil | programar | em paralelo? |
| ---------------- | ------- | --------- | ------------ |
MULTITAREFAS • Sincronização, comunicação e equilíbrio de carga
17

PROGRAMAS
| • Por que é mais | difícil | programar | em paralelo? |
| ---------------- | ------- | --------- | ------------ |
MULTITAREFAS • Sincronização, comunicação e equilíbrio de carga
18

PROGRAMAS
• Por que é mais desafiador programar em paralelo?
MULTITAREFAS
• Sincronização, comunicação e equilíbrio de carga
$ gcc -fopenmp meuprog.c -o meuprog
Tarefa 5:
Implemente um programa em C que
conte quantos números primos
existem entre 2 e um valor máximo n.
depois, paralelize o laço principal
usando a diretiva #pragma omp
parallel for sem alterar a
lógica original. compare o tempo de
execução e os resultados das
versões sequencial e paralela.
observe possíveis diferenças no
resultado e no desempenho, e reflita
sobre os desafios iniciais da
programação paralela, como
correção e distribuição de carga.
19

PROGRAMAÇÃO
| • Objetivos desse | tópico |     |     |
| ----------------- | ------ | --- | --- |
EM MEMÓRIA
| • Conhecer | as ferramentas básicas | de programação | em memória |
| ---------- | ---------------------- | -------------- | ---------- |
compartilhada
COMPARTILHADA
20

PROGRAMAÇÃO
• Parallel e parallel for
EM MEMÓRIA
• Escopo de variáveis
COMPARTILHADA
• Condição de corrida e regiões críticas
Tarefa 6:
Implemente em C a estimativa estocástica
de π. Paralelize com #pragma omp
| parallel | for e explique o resultado  |     |     |
| -------- | --------------------------- | --- | --- |
incorreto. Corrija a condição de corrida
| utilizando o #pragma omp |             | critical    | e   |
| ------------------------ | ----------- | ----------- | --- |
| reestruturando com       | #pragma omp |             |     |
| parallel                 | seguido de  | #pragma omp |     |
for e aplicando as cláusulas private,
| firstprivate, lastprivate |     | e   |     |
| ------------------------- | --- | --- | --- |
shared. Teste diferentes combinações e
explique como cada cláusula afeta o
comportamento do programa. Comente
também como a cláusula default(none)
pode ajudar a tornar o escopo mais claro em
programas complexos.
21

PROGRAMAÇÃO
• #pragma omp single
EM MEMÓRIA
• #pragma omp master
COMPARTILHADA
• #pragma omp barrier
• Cláusula nowait
• Modelo de tarefas
• #pragma omp tasks
• #pragma omp taskwait
22

PROGRAMAÇÃO
• #pragma omp single
EM MEMÓRIA
• #pragma omp master
COMPARTILHADA
• #pragma omp barrier
• Cláusula nowait
Tarefa 7:
• Modelo de tarefas
Implemente um programa em C que cria
uma lista encadeada com nós, cada um,
• #pragma omp tasks
contendo o nome de um arquivo fictício.
Dentro de uma região paralela, percorra
• #pragma omp taskwait
a lista e crie uma tarefa com #pragma
omp task para processar cada nó.
Cada tarefa deve imprimir o nome do
arquivo e o identificador da thread que a
executou. Após executar o programa,
reflita: todos os nós foram processados?
Algum foi processado mais de uma vez
ou ignorado? O comportamento muda
entre execuções? Como garantir que
cada nó seja processado uma única vez
e por apenas uma tarefa?
23

COERÊNCIA DE CACHE
• Objetivos desse tópico
E FALSO
• Entender as vantagens do uso de variáveis compartilhadas e
COMPARTILHAMENTO
aprender a evitar as desvantagens
• Entender o que é falso compartilhamento e aprender a evitá-lo
24

COERÊNCIA DE CACHE
• Variáveis compartilhadas:
E FALSO
• Vantagens para somente leitura ou escrita infrequente (evita
COMPARTILHAMENTO
copias com private e firstprivate)
• Desvantagens para escrita frequente: condição de corrida ou
serialização devido a sincronização
• Coerência de cache com caches privadas
• Write-through vs. Write-back
• Bit de sujo (Dirty bit)
• Bit de inválido (Invalid bit) em múltiplas caches
• Protocolos de coerência de cache
• Snooping
• Baseados em diretórios
• Variáveis compartilhadas: escrita frequente torna cache inútil e
deteriora ainda mais o acesso a memória (dois acessos)
• Falso compartilhamento: torna a cache inútil mesmo sem
compartilhamento de variáveis
25

COERÊNCIA DE CACHE
• Variáveis compartilhadas:
E FALSO
• Vantagens para somente leitura ou escrita infrequente (evita
COMPARTILHAMENTO
copias com private e firstprivate)
• Desvantagens para escrita frequente: condição de corrida ou
Tarefa 8:
serialização devido a sincronização
Implemente estimativa estocástica de π
usando rand() para gerar os pontos.
• Coerência de cache com caches privadas
Cada thread deve usar uma variável
privada para contar os acertos e
• Write-through vs. Write-back
acumular o total em uma variável global
com #pragma omp critical. • Bit de sujo (Dirty bit)
Depois, implemente uma segunda versão
• Bit de inválido (Invalid bit) em múltiplas caches
em que cada thread escreve seus
• Protocolos de coerência de cache
acertos em uma posição distinta de um
vetor compartilhado. A acumulação deve
• Snooping
ser feita em um laço serial após a região
• Baseados em diretórios
paralela. Compare o tempo de execução
das duas versões. Em seguida, substitua
• Variáveis compartilhadas: escrita frequente torna cache inútil e
rand() por rand_r() em ambas e
deteriora ainda mais o acesso a memória (dois acessos)
compare novamente. Explique o
comportamento dos quatro programas • Falso compartilhamento: torna a cache inútil mesmo sem
com base na coerência de cache e nos
compartilhamento de variáveis
efeitos do falso compartilhamento.
26

MECANISMOS DE
• Objetivos desse tópico
SINCRONIZAÇÃO • Compreender as diferentes aplicações para os vários
mecanismos de sincronização existentes
• Compreender que esses mecanismos têm sobrecargas
diferentes no desempenho dos programas
28

| MECANISMOS DE  | • #pragma omp | critical | (nome) |
| -------------- | ------------- | -------- | ------ |
SINCRONIZAÇÃO
• Locks explícitos
|     | • omp_lock_t | lock |     |
| --- | ------------ | ---- | --- |
• omp_init_lock(&lock)
• omp_set_lock(&lock)
• omp_unset_lock(&lock)
• omp_destroy_lock(&lock)
29

MECANISMOS DE
• #pragma omp critical (nome)
SINCRONIZAÇÃO • Locks explícitos
• omp_lock_t lock
• omp_init_lock(&lock)
Tarefa 9:
Escreva um programa que cria tarefas
• omp_set_lock(&lock)
para realizar N inserções em duas listas
encadeadas, cada uma associada a uma • omp_unset_lock(&lock)
thread. Cada tarefa deve escolher
aleatoriamente em qual lista inserir um • omp_destroy_lock(&lock)
número. Garanta a integridade das listas
evitando condição de corrida e, sempre
que possível, use regiões críticas
nomeadas para que a inserção em uma
lista não bloqueie a outra. Em seguida,
generalize o programa para um número
de listas definido pelo usuário. Explique
por que, nesse caso, regiões críticas
nomeadas não são suficientes e por que
o uso de locks explícitos se torna
necessário.
30

| MECANISMOS DE  | • #pragma omp | atomic |
| -------------- | ------------- | ------ |
SINCRONIZAÇÃO
• A clausula reduction
31

MECANISMOS DE
• #pragma omp atomic
SINCRONIZAÇÃO
• A clausula reduction
Tarefa 10:
Implemente novamente o estimador da
tarefa 8 que usa um contador
| compartilhado e o rand_r |                  | substituindo  |
| ------------------------ | ---------------- | ------------- |
| o #pragma omp            | critical         | pelo          |
| #pragma omp              | atomic. Compare  |               |
essas duas implementações com suas
versões que usam contadores privados.
Agora, compare essas com uma 5ª
versão que utiliza apenas a cláusula
| reduction ao invés das diretivas de  |     |     |
| ------------------------------------ | --- | --- |
sincronização.  Reflita sobre a
aplicabilidade de desses mecanismos em
termos de desempenho e produtividade e
proponha um roteiro para quando utilizar
qual mecanismo de sincronização,
| incluindo critical | nomeadas e locks |     |
| ------------------ | ---------------- | --- |
explícitos.
32

PARTICIONAMENTO
• Objetivos desse tópico:
DE DADOS E • Conhecer os diversos métodos de particionamento de dados e
balanceamento de carga
BALANCEAMENTO
DE CARGA
33

| PARTICIONAMENTO  | • #pragma omp | sections |
| ---------------- | ------------- | -------- |
DE DADOS E
• A cláusula schedule
BALANCEAMENTO
• static
| DE CARGA | • dynamic |     |
| -------- | --------- | --- |
• guided
• chunksize
• A cláusula collapse
|     | • #pragma omp | simd |
| --- | ------------- | ---- |
34

PARTICIONAMENTO
• A equação de Navier-Stokes
DE DADOS E • descreve como um fluido se move, acelera e se deforma por
causa da pressão, da viscosidade e de forças externas.
BALANCEAMENTO
• Aplicações práticas
DE CARGA
• Movimento de ar (vento, turbulência atmosférica).
• Fluxo de água (rios, oceanos, tubulações).
• Aerodinâmica (aviões, carros).
• Medicina (fluxo sanguíneo).
• Climatologia (modelagem do clima).
• Engenharia (motores, bombas, ventilação).
35

PARTICIONAMENTO • A equação de Navier-Stokes
DE DADOS E • descreve como um fluido se move, acelera e se deforma por
causa da pressão, da viscosidade e de forças externas.
BALANCEAMENTO DE
• Aplicações práticas
CARGA
• Movimento de ar (vento, turbulência atmosférica).
Tarefa 11:
Escreva um código que simule o movimento • Fluxo de água (rios, oceanos, tubulações).
de um fluido ao longo do tempo usando a
• Aerodinâmica (aviões, carros).
equação de Navier-Stokes, considerando
apenas os efeitos da viscosidade.
• Medicina (fluxo sanguíneo).
Desconsidere a pressão e quaisquer forças
externas. Utilize diferenças finitas para
• Climatologia (modelagem do clima).
discretizar o espaço e simule a evolução da
velocidade do fluido no tempo. Inicialize o • Engenharia (motores, bombas, ventilação).
fluido parado ou com velocidade constante e
verifique se o campo permanece estável. Em
seguida, crie uma pequena perturbação e
observe se ela se difunde suavemente. Após
validar o código, paralelize-o com OpenMP e
explore o impacto das cláusulas schedule
e collapse no desempenho da execução
paralela.
36

AVALIAÇÃO DE
• Objetivos desse tópico:
DESEMPENHO E o Entender a diferença entre desempenho e escalabilidade
o Aprender a identificar pontos críticos e gargalos de
ESCALABILIDADE
escalabilidade
PARALELA
37

AVALIAÇÃO DE • Desempenho vs Escalabilidade
DESEMPENHO E
ESCALABILIDADE
PARALELA
• O desempenho está relacionado à rapidez com que a tarefa
é executada
• A escalabilidade está relacionada à eficiência com que a
tarefa é executada
38

AVALIAÇÃO DE • Desempenho é o inverso do Tempo, isto é:
o Desempenho = 1/Tempo
DESEMPENHO E
ESCALABILIDADE
• Eficiência é quanto Trabalho é realizado por Recurso, isto é:
PARALELA o Eficiência = Trabalho x Desempenho / Recursos, ou
o Eficiência = Trabalho / (Tempo x Recursos)
39

AVALIAÇÃO DE
• A primeira tarefa é sequencial e a segunda é paralela.
DESEMPENHO E Ambas performam 100% de eficiência.
ESCALABILIDADE
PARALELA
41

AVALIAÇÃO DE
• Ampliar os recursos por si só muitas vezes prejudica a
DESEMPENHO E eficiência.
ESCALABILIDADE
PARALELA
42

AVALIAÇÃO DE
• Máquinas maiores, como os supercomputadores, exigem
DESEMPENHO E problemas maiores para manter a eficiência.
ESCALABILIDADE
PARALELA
• Sobrecarga de paralelização, dependências, serialização,
etc. também prejudicam a eficiência.
43

AVALIAÇÃO DE
• A eficiência pode variar de acordo com o escalonamento de
DESEMPENHO E recursos e de problemas
ESCALABILIDADE
PARALELA
44

AVALIAÇÃO DE
• Modelos de Speedup
DESEMPENHO E
o Modelo de Amdahl
ESCALABILIDADE
PARALELA
• Speedup(p) = T(1)/T(p) 45

AVALIAÇÃO DE
• Modelos de Speedup
DESEMPENHO E
o Modelo de Gustafson
ESCALABILIDADE
PARALELA
• Eficiência(p) = S(p)/p 46

AVALIAÇÃO DE
• Análise de Eficiência Paralela e Escalabilidade
DESEMPENHO E
ESCALABILIDADE
PARALELA
• Escalável: mantém a eficiência para mais recursos com um
problema maior de qualquer fator
47

AVALIAÇÃO DE
• Análise de Eficiência Paralela e Escalabilidade
DESEMPENHO E
ESCALABILIDADE
PARALELA
• Fracamente Escalável: Mantém a eficiência mesmo quando o
tamanho do problema cresce menos do que os recursos
48

AVALIAÇÃO DE
• Análise de Eficiência Paralela e Escalabilidade
DESEMPENHO E
ESCALABILIDADE
PARALELA
• Fortemente Escalável: Mantém a eficiência mesmo sem
aumentar o tamanho do problema
49

AVALIAÇÃO DE
• Avaliação da escalabilidade de um programa com tabelas
DESEMPENHO E
ESCALABILIDADE
PARALELA
Escalável? Fracamente? Fortemente?
A análise ainda é possível com um número pequeno de núcleos e
taxas quadraticamente crescentes
50

AVALIAÇÃO DE
• Avaliação da escalabilidade de um programa com tabelas
DESEMPENHO E
ESCALABILIDADE
PARALELA
1/4 da tabela do slide anterior com taxas lineares
51

AVALIAÇÃO DE
• Visualização da escalabilidade de um programa
DESEMPENHO E
ESCALABILIDADE
PARALELA
52

AVALIAÇÃO DE
• Visualização da escalabilidade de um programa
DESEMPENHO E
ESCALABILIDADE
PARALELA
53

AVALIAÇÃO DE
• Visualização da escalabilidade de um programa
DESEMPENHO E
ESCALABILIDADE
PARALELA
Tarefa 12:
Avalie a escalabilidade do seu código de
Navier-Stokes utilizando algum nó de
computação do NPAD. Procure identificar
gargalos de escalabilidade e reporte o seu
progresso em versões sucessivas da
evolução do código otimizado. Comente
sobre a escalabilidade, a escalabilidade
fraca e a escalabilidade fortes das versões.
54

ARQUITETURAS • Objetivos desse tópico:
• Identificar as diversas classes de arquiteturas paralelas de
PARALELAS
hardware definidas pela Taxonomia de Flynn
• Compreender o impacto e aprender a configurar as diversas
formas de afinidade de threads
55

| ARQUITETURAS  | • Single Instruction | Single Data |
| ------------- | -------------------- | ----------- |
PARALELAS
56

| ARQUITETURAS  | • Single Instruction | Multiple | Data |
| ------------- | -------------------- | -------- | ---- |
PARALELAS
57

| ARQUITETURAS  | • Multiple | Instruction | Single Data |
| ------------- | ---------- | ----------- | ----------- |
PARALELAS
58

| ARQUITETURAS  | • Multiple | Instruction | Multiple | Data |
| ------------- | ---------- | ----------- | -------- | ---- |
PARALELAS
59

ARQUITETURAS • Arquiteturas típicas MIMD de memória compartilhada:
PARALELAS
60

ARQUITETURAS • Hierarquia de cache em sistemas NUMA:
PARALELAS
61

• Hierarquia de cache em sistemas NUMA:
62

ARQUITETURAS • Afinidade de threads com OpenMP
• Exemplos com definição de políticas:
PARALELAS
• export OMP_PROC_BIND=TRUE; ou
• #pragma omp parallel proc_bind(spread)
Tarefa 13: • Valores comuns:
Avalie como a escalabilidade do seu código
• false: sem afinidade (pode migrar entre núcleos).
de Navier-Strokes muda ao utilizar os
diversos tipos de afinidades de threads
• true: thread fica presa ao núcleo onde foi criada.
suportados pelo sistema operacional e pelo
OpenMP no mesmo nó de computação do • close: threads próximas da master thread, mas distribuídas.
NPAD que utilizou para a tarefa 12.
• spread: threads espalhadas pelos núcleos (boa para usar
múltiplos sockets).
• master: todas seguem a thread 0 (pode forçar várias threads
no mesmo núcleo).
• Para mais controle, use OMP_PLACES, por exemplo:
• OMP_PLACES=cores
• OMP_PLACES="{0:8:1},{8:8:1}"
63