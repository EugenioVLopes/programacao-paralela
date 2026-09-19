# Tarefa 13 — afinidade de threads

A campanha reutiliza exatamente `../tarefa12/v4_numa.c` e a matriz PaScal da Tarefa 12. Ela executa somente as quatro configurações ainda não medidas:

| JSON | `OMP_PROC_BIND` | `OMP_PLACES` |
| --- | --- | --- |
| `v4_numa_false.json` | `false` | não definido |
| `v4_numa_close_cores.json` | `close` | `cores` |
| `v4_numa_close_threads.json` | `close` | `threads` |
| `v4_numa_spread_threads.json` | `spread` | `threads` |

A configuração `spread + cores` já pertence à Tarefa 12 e não é repetida nem incluída na análise nova.

## Resultado

O job 2107610 executou em `r2n19.npad.internal` e produziu quatro JSONs completos, com 49 pontos cada. O relatório usa somente os quatro gráficos `Scalability — Whole Program (Region 0)` exportados pelo PaScal Viewer.

- [Relatório em PDF](relatorio.pdf)
- [Código-base](../tarefa12/v4_numa.c)
- [Resultados brutos](runs/pascal-2107610/data/)

A Tarefa 12 usou `r2n04`. Os dois nós registraram o mesmo modelo de CPU e a mesma topologia, mas não são a mesma máquina física; o relatório declara essa limitação e não compara numericamente as duas tarefas.

## Reexecutar no NPAD

Mantenha `tarefa12/` e `tarefa13/` lado a lado no NPAD. Dentro de `tarefa13/`, defina o ambiente do PaScal e submeta:

```bash
cd ~/tarefa13
export PASCAL_ENV="$HOME/tools/pascal-releases-master/env.sh"
sbatch --export=ALL afinidade.slurm
```

A saída fica em `runs/pascal-JOB_ID/`. Um diretório existente com o mesmo identificador faz o job falhar em vez de sobrescrever dados. Para reproduzir literalmente o enunciado, acrescente novamente `#SBATCH --nodelist=r2n04` ao arquivo antes da submissão.
