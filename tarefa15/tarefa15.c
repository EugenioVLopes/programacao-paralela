#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define CELLS 1048576
#define STEPS 1000
#define COEFFICIENT 0.2

static void update(double *next, const double *current, int i) {
    next[i] = current[i] + COEFFICIENT *(current[i - 1] - 2.0 * current[i] + current[i + 1]);
}

static void blocking(double *next, double *current, int count, int left, int right) {
    MPI_Send(&current[count], 1, MPI_DOUBLE, right, 0, MPI_COMM_WORLD);
    MPI_Recv(&current[0], 1, MPI_DOUBLE, left, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(&current[1], 1, MPI_DOUBLE, left, 1, MPI_COMM_WORLD);
    MPI_Recv(&current[count + 1], 1, MPI_DOUBLE, right, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (int i = 1; i <= count; ++i) update(next, current, i);
}

static void nonblocking(double *next, double *current, int count, int left, int right) {
    MPI_Request requests[4];
    MPI_Irecv(&current[0], 1, MPI_DOUBLE, left, 0, MPI_COMM_WORLD, &requests[0]);
    MPI_Irecv(&current[count + 1], 1, MPI_DOUBLE, right, 1, MPI_COMM_WORLD, &requests[1]);
    MPI_Isend(&current[count], 1, MPI_DOUBLE, right, 0, MPI_COMM_WORLD, &requests[2]);
    MPI_Isend(&current[1], 1, MPI_DOUBLE, left, 1, MPI_COMM_WORLD, &requests[3]);
    for (int i = 0; i < 4; ++i) MPI_Wait(&requests[i], MPI_STATUS_IGNORE);

    for (int i = 1; i <= count; ++i) update(next, current, i);
}

static void overlap(double *next, double *current, int count, int left, int right) {
    MPI_Request requests[4];
    MPI_Irecv(&current[0], 1, MPI_DOUBLE, left, 0, MPI_COMM_WORLD, &requests[0]);
    MPI_Irecv(&current[count + 1], 1, MPI_DOUBLE, right, 1, MPI_COMM_WORLD, &requests[1]);
    MPI_Isend(&current[count], 1, MPI_DOUBLE, right, 0, MPI_COMM_WORLD, &requests[2]);
    MPI_Isend(&current[1], 1, MPI_DOUBLE, left, 1, MPI_COMM_WORLD, &requests[3]);

    int received_left = 0, received_right = 0;
    for (int i = 2; i < count; ++i) {
        update(next, current, i);
        if (i % 1024 == 0) {
            if (!received_left)
                MPI_Test(&requests[0], &received_left, MPI_STATUS_IGNORE);
            if (!received_right)
                MPI_Test(&requests[1], &received_right, MPI_STATUS_IGNORE);
        }
    }
    if (!received_left) MPI_Wait(&requests[0], MPI_STATUS_IGNORE);
    if (!received_right) MPI_Wait(&requests[1], MPI_STATUS_IGNORE);
    update(next, current, 1);
    update(next, current, count);
    MPI_Wait(&requests[2], MPI_STATUS_IGNORE);
    MPI_Wait(&requests[3], MPI_STATUS_IGNORE);
}

int main(int argc, char **argv) {
    int rank, process_count;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    if (process_count < 2 || process_count > CELLS / 2) {
        if (rank == 0) fprintf(stderr, "Use de 2 a %d processos.\n", CELLS / 2);
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int count = CELLS / process_count + (rank < CELLS % process_count);
    int start = rank * (CELLS / process_count) + (rank < CELLS % process_count ? rank : CELLS % process_count);
    int left = rank == 0 ? MPI_PROC_NULL : rank - 1;
    int right = rank == process_count - 1 ? MPI_PROC_NULL : rank + 1;
    double *a = calloc((size_t)count + 2, sizeof(double));
    double *b = calloc((size_t)count + 2, sizeof(double));
    if (!a || !b) {
        fprintf(stderr, "Falha de alocacao no processo %d.\n", rank);
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    void (*versions[])(double *, double *, int, int, int) = {
        blocking, nonblocking, overlap
    };
    const char *names[] = {"Send/Recv", "Isend/Irecv+Wait", "Isend/Irecv+Test"};
    if (rank == 0) printf("Versao\tTempo(s)\tSoma final\n");
    for (int version = 0; version < 3; ++version) {
        for (int i = 1; i <= count; ++i)
            a[i] = start + i - 1 < CELLS / 2 ? 1.0 : 0.0;
        a[0] = a[count + 1] = 0.0;
        b[0] = b[count + 1] = 0.0;
        double *current = a, *next = b;

        MPI_Barrier(MPI_COMM_WORLD);
        double begin = MPI_Wtime();
        for (int step = 0; step < STEPS; ++step) {
            versions[version](next, current, count, left, right);
            double *temporary = current;
            current = next;
            next = temporary;
        }
        MPI_Barrier(MPI_COMM_WORLD);
        double elapsed = MPI_Wtime() - begin;

        double local_sum = 0.0, total_sum = 0.0;
        for (int i = 1; i <= count; ++i) local_sum += current[i];
        MPI_Reduce(&local_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
        if (rank == 0) printf("%s\t%.6f\t%.6f\n", names[version], elapsed, total_sum);
    }

    free(a);
    free(b);
    MPI_Finalize();
    return EXIT_SUCCESS;
}
