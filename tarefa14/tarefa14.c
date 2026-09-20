#include <mpi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EXPECTED_PROCESS_COUNT 2
#define SENDER_RANK 0
#define RECEIVER_RANK 1
#define NUM_ITER 10000

static const int MESSAGE_SIZES[] = {
    8, 64, 512, 4096, 32768, 262144, 1048576
};

int main(int argc, char **argv) {
    int rank;
    int process_count;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);

    if (process_count != EXPECTED_PROCESS_COUNT) {
        if (rank == SENDER_RANK) {
            fprintf(stderr, "Este programa requer exatamente 2 processos.\n");
        }
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    const int size_count = sizeof(MESSAGE_SIZES) / sizeof(MESSAGE_SIZES[0]);

    if (rank == SENDER_RANK) printf("Tamanho(bytes)\tTempo_total(s)\tTempo_medio(us)\n");

    for (int index = 0; index < size_count; ++index) {
        const int message_size = MESSAGE_SIZES[index];
        char *buffer = malloc((size_t)message_size);
        if (buffer == NULL) {
            fprintf(stderr, "Falha ao alocar %d bytes no processo %d.\n", message_size, rank);
            MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
        }
        memset(buffer, 'a', (size_t)message_size);

        MPI_Barrier(MPI_COMM_WORLD);
        const double start = MPI_Wtime();

        for (int iteration = 0; iteration < NUM_ITER; ++iteration) {
            if (rank == SENDER_RANK) {
                MPI_Send(buffer, message_size, MPI_CHAR, RECEIVER_RANK, 0, MPI_COMM_WORLD);
                MPI_Recv(buffer, message_size, MPI_CHAR, RECEIVER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else {
                MPI_Recv(buffer, message_size, MPI_CHAR, SENDER_RANK, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(buffer, message_size, MPI_CHAR, SENDER_RANK, 0, MPI_COMM_WORLD);
            }
        }

        const double total_seconds = MPI_Wtime() - start;

        if (rank == SENDER_RANK) printf("%d\t\t%.9f\t%.3f\n", message_size, total_seconds, total_seconds * 1e6 / NUM_ITER);

        free(buffer);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
