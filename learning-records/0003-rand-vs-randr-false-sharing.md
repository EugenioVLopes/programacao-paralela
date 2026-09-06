# 0003, rand trava no software, vetor colado trava no hardware

Data, 2026-09-06. Tarefa 08.

Contexto, Monte Carlo de pi com 4 versoes, critical contra vetor, rand contra rand_r, N igual a 2M, i7-14700HX.

O que eu vi. V1 e v2 ficam lentos e pioram, de 0,09 para 0,57s. V3 escala 5,5 vezes, de 0,023 para 0,004s. V4 com rand_r mostra MESI puro, com 0,014s em 8 threads. Padding de 64 bytes recupera 2,4 vezes, para 0,006s.

Regra que eu levo. Rand tem estado global com lock, entao nunca roda em laco paralelo. Rand_r com seed privada mais contador privado mais 1 critical escala. Hits por id a cada acerto pede padding ou reduction.

Confusao desfeita. Os arquivos tarefa08a e b tratavam de listas, tema da tarefa 09, nao deste exercicio. Eu removi eles. Os arquivos pi ponto c sao os validos da tarefa 08.
