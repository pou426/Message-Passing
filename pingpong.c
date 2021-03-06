// Two processes (rank 0 and rank 1) repeatedly pass a message back and forth.
#include <stdio.h>
#include <mpi.h>

void mpi_ping_pong(int nmsg, int rank) {
	int nturns = 100000;
  int temp_nturns = nturns;

  int buf[nmsg];
  for (int i = 0; i < nmsg; i++) {
  	buf[i] = i;
  }

  double tstart, tstop;
  MPI_Barrier(MPI_COMM_WORLD);  // Line up at the start line
  tstart = MPI_Wtime();         // Fire the gun and start the clock
  if (rank == 0) {
  	// MPI_Ssend(buf, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
  	while (nturns > 0) {
  		MPI_Ssend(buf, nmsg, MPI_INT, 1, 0, MPI_COMM_WORLD);
  		// printf("rank [0]: Ping.\n");
  		MPI_Recv(&buf, nmsg, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  		nturns--;
  	}
  }

  if (rank == 1) {
  	while (nturns > 0) {
  		MPI_Recv(&buf, nmsg, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  		MPI_Ssend(buf, nmsg, MPI_INT, 0, 0, MPI_COMM_WORLD);
  		// printf("rank [1]: Pong.\n");
  		nturns--;
  	}
  }

  MPI_Barrier(MPI_COMM_WORLD);  // Wait for everyone to finish
  tstop = MPI_Wtime();          // Stop the clock

  if (rank == 0) {
  	printf("Rank %d: Ping-pong with %d turns, msg size %d, takes %fs\n", rank, temp_nturns, nmsg, tstop-tstart);
  }
}

int main(void) {
  MPI_Init(NULL, NULL);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int size; 
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int nmsg[] = {0, 100, 1000, 10000};
  for (int i = 0; i < sizeof(nmsg) / sizeof(nmsg[0]); i++) {
		mpi_ping_pong(nmsg[i], rank);
  }

  MPI_Finalize();

  return 0;
}