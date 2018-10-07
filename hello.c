#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <mpi.h>

float pi(int N, int subN, int rank)
{
  float result;
  for (long long int i = 0; i < 1000000; i++) {
    // printf("i %lld", i);
    float C = 4 / ((float) N);
    result = 0.0;
    int init = rank * subN;
    int final = init + subN;
    for (int i = init; i < final; i++) {
        result += 1 / (1 + pow(((i - 0.5) / N), 2));
    }

    result *= C;
  }
    return result;
}

int main(void)
{
  MPI_Init(NULL, NULL);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int size; 
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int N = 840;
  int remainder = N % size;
  // printf("Remainder=%d\n", remainder);

  float result;
  double tstart, tstop;
  if (N % size == 0) {
    MPI_Barrier(MPI_COMM_WORLD);  // Line up at the start line
    tstart = MPI_Wtime();         // Fire the gun and start the clock

    float subN = N / size;
    result = pi(N, (int)subN, rank);

    MPI_Barrier(MPI_COMM_WORLD);  // Wait for everyone to finish
    tstop = MPI_Wtime();          // Stop the clock

    if (rank == 0) {
      float temp;
      for (int i = 1; i < size; i++) {
          MPI_Recv(&temp, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          result += temp;
      }
    } else {
      MPI_Send(&result, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    }

  } else {

  }

  printf("Hello World from rank %d\nresult %.20f\ntime %fs\n", rank, result, tstop - tstart);

  MPI_Finalize();
}
