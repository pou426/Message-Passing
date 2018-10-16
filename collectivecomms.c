/* Around a Ring with non-blocking send or receive */

#include <stdio.h>
#include <mpi.h>
#include <math.h>


/**
 *  Use MPI_Allreduce method to pass information around the ring
 */
void all_reduce(int rank, int* global_sum) {
  int local_value = rank;
  // int global_sum;
  MPI_Allreduce(&local_value, global_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
}


int main(void) {
  MPI_Init(NULL, NULL);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  double tstart, tstop;
  MPI_Barrier(MPI_COMM_WORLD);  // Line up at the start line
  tstart = MPI_Wtime();         // Fire the gun and start the clock

  int global_sum;
  for (int i = 0; i < 10000000; i++) {
    all_reduce(rank, &global_sum);
  }

	MPI_Barrier(MPI_COMM_WORLD);  // Wait for everyone to finish
  tstop = MPI_Wtime();          // Stop the clock
  printf("Rank %d: global_sum=%d time=%fs\n", rank, global_sum, tstop-tstart);

	MPI_Finalize();

  return 0;
}


// PERFORMANCE NOTES
// Isa:tutorial home$ mpirun -n 1 collectivecomms
// Rank 0: global_sum=0 time=0.243627s
// $ mpirun -n 2 collectivecomms
// Rank 0: global_sum=1 time=3.557026s
// Rank 1: global_sum=1 time=3.557032s
// $ mpirun -n 3 collectivecomms
// Rank 0: global_sum=3 time=6.978907s
// Rank 1: global_sum=3 time=6.978840s
// Rank 2: global_sum=3 time=6.978843s
// $ mpirun -n 4 collectivecomms
// Rank 0: global_sum=6 time=7.445849s
// Rank 1: global_sum=6 time=7.445832s
// Rank 2: global_sum=6 time=7.445849s
// Rank 3: global_sum=6 time=7.445837s
