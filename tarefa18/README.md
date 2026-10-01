# Tarefa 18 — soma de vetores na CPU e GPU

Executado no NPAD: job 2138518, nó `gpunode1`, partição `gpu-8-v100`.
CPU Intel Xeon E5-2683 v4, GPU Tesla V100-SXM2-16GB, NVIDIA HPC SDK 24.11.
Cinco execuções por versão, oito threads, zero erros e offload confirmado.
Relatório: [PDF](relatorio.pdf) · [fonte LaTeX](relatorio.tex).

Fonte: [tutorial adaptado pelo NPAD](https://github.com/NPAD-UFRN/openmp-tutorial).
No PDF `omp_GPGPU_prog_SC23.pdf` disponível nesse repositório, os exercícios
de soma paralela na CPU e GPU estão nas páginas 29 e 50, respectivamente,
em vez de 27 e 48 do enunciado.

## Implementação

Os dois programas derivam de `vadd.c`, de Tim Mattson (2017), preservando
N = 10.000.000, quatro vetores de float e tolerância de 10⁻⁷ para o erro ao
quadrado. A CPU usa `parallel for` nos três laços e `reduction(+:err)` na
verificação. A versão GPU altera somente o laço de soma para `target teams loop`,
com mapeamento implícito dos vetores. Inicialização e verificação continuam
paralelas na CPU nas duas versões. Os vetores continuam
na pilha, como no original.

A medição usa `clock_gettime(CLOCK_MONOTONIC)`, conforme o padrão do projeto,
com seis casas decimais. Cada programa imprime inicialização, soma,
verificação e a soma dos três intervalos. Na versão GPU, o intervalo da soma inclui
as transferências implícitas da região target e sua sincronização;
não são tempos isolados de kernel. Antes da medição, uma região target
confirma o dispositivo e inicializa o runtime GPU. Esse custo inicial fica
fora dos tempos apresentados. Não há região persistente `target data`.

## Execução no NPAD

O job carrega `compilers/nvidia/nvhpc/24.11`. Para repetir:

```bash
cd tarefa18
sbatch vadd.slurm
squeue -u "$USER"
# Ao terminar, substitua JOBID pelo número retornado por sbatch:
cat vadd-JOBID.out
```

O job solicita uma GPU e oito CPUs no mesmo nó e executa cinco vezes cada
programa. A partição `gpu-8-v100` foi escolhida para este experimento;
confirme a disponibilidade com `sinfo` e ajuste a partição e a solicitação
de GPU se o NPAD indicar outra configuração. O módulo carregado precisa
estar acessível também no ambiente do job.

Usamos o mesmo compilador e `-O3` nas duas versões; `-mp=multicore` habilita
OpenMP na CPU e `-mp=gpu` habilita offload, conforme o
[manual NVIDIA](https://docs.nvidia.com/hpc-sdk/compilers/hpc-compilers-user-guide/).
Não basta compilar o original com `gcc -fopenmp` para garantir suporte GPU.

## Comparação e registro de progresso

Preserve `vadd-JOBID.out`, incluindo identificação do nó, CPU, GPU,
compilador, hashes, threads e as cinco medições. Exija zero erros e a
mensagem de que target executou na GPU em todas as execuções.
Medianas das cinco medições preservadas em [vadd-2138518.out](vadd-2138518.out):

| Intervalo | CPU (s) | GPU (s) | Speedup CPU/GPU |
|---|---:|---:|---:|
| Inicialização | 0,020963 | 0,020564 | 1,019 |
| Soma (Compute time) | 0,003680 | 0,041005 | 0,090 |
| Verificação | 0,002175 | 0,002007 | 1,084 |
| Total dos intervalos | 0,026641 | 0,064948 | 0,410 |

Speedup = tempo CPU / tempo GPU. Valor acima de 1 favorece a GPU.
A CPU foi mais rápida na soma e no total. O intervalo GPU inclui cópias
implícitas `tofrom` dos três vetores; os custos de cópia e kernel não foram
medidos separadamente.

Problemas identificados na preparação e soluções incorporadas:

- **Limite de pilha:** os quatro vetores ocupam aproximadamente 160 MB.
  O job aumenta a pilha com `ulimit -s unlimited` e a pilha OpenMP com
  `OMP_STACKSIZE=256M`, para permitir executar o código original.
- **Execução target na CPU:** o job define `OMP_TARGET_OFFLOAD=MANDATORY`
  e o programa verifica `omp_is_initial_device()`. Se não houver offload,
  encerra com erro e os tempos não devem entrar na comparação GPU.
- **Suporte do compilador:** a configuração padrão do tutorial NPAD usa
  GCC com `-fopenmp`. A preparação escolhe NVIDIA HPC SDK com `-mp=gpu`;
  o módulo foi encontrado e utilizado com sucesso no NPAD.
- **Numeração dos slides:** o PDF atual difere do enunciado; os exercícios
  foram localizados pelo título e pelo conteúdo.

A tentativa anterior, job 2129455, não executou na GPU (`na_gpu=0`) e foi
preservada em `tentativa-anterior/`, sem entrar na comparação. O job 2138508
parou no código 14 de `nvidia-smi`, com aviso de infoROM corrompida. A consulta
passou a registrar o aviso e continuar, mantendo a verificação do dispositivo
pelo programa. O job 2138518 concluiu com sucesso; isso não corrige o aviso
de hardware. A saída da falha está em [vadd-2138508.out](vadd-2138508.out).

## Referências de colegas

Foram consultados os relatórios de Kiev Luiz Freitas Guedes
(`../assets/PP___Tarefa_18.pdf`) e Lucas Augusto da Silva Cardoso
(`../assets/Programação Paralela - Relatório 18.pdf`) para orientar a
estrutura do experimento: inicialização e verificação na CPU, offload
somente da soma. Seus tempos e problemas não são resultados deste trabalho.
Tempos iguais entre versões não comprovam, por si só, execução na CPU;
a confirmação aqui usa `omp_is_initial_device()`. O custo das transferências
está incluído no tempo da soma, mas sua contribuição isolada não foi medida.
