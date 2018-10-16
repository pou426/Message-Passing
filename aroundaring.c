/* Around a Ring with non-blocking send or receive */

#include <stdio.h>
#include <mpi.h>
#include <math.h>


/**
 * Using non-blocking synchronous send.
 * Returns the total sum of its own value plus received values  
 */
int issend_ring(int rank, int size) {
  int value = pow((rank + 1), 2);  // the process's own value
  int sendBuf = value;  					 // first, sends its own rank
  int receiveBuf;									 // initialise empty receive buffer
  int dest, from;  								 // right and left neighbours

  for (int i = 0; i < size-1; i++) {
	  MPI_Request request;
	  dest = (rank + 1) % size;  		// to its right neighbour
		MPI_Issend(&sendBuf, 1, MPI_INTEGER, dest, 0, MPI_COMM_WORLD, &request);

		from = (rank - 1) % size;			// from its left neighbour
		MPI_Recv(&receiveBuf, 1, MPI_INTEGER, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  	// sum its own value up with the received value
		value += receiveBuf;

		// Block until sending is done
		MPI_Wait(&request, MPI_STATUS_IGNORE);

		// Update send buffer to what it has received
		sendBuf = receiveBuf;
  }

  return value;
}

/**
 * Using non-blocking receive.
 * Returns the total sum of its own value plus received values  
 */
int irecv_ring(int rank, int size) {
  int value = pow((rank + 1), 2);  // the process's own value
  int sendBuf = value;  					 // first, sends its own rank
  int receiveBuf;									 // initialise empty receive buffer
  int dest, from;  								 // right and left neighbours

	for (int i = 0; i < size-1; i++) {
		MPI_Request request;
		from = (rank - 1) % size;
		MPI_Irecv(&receiveBuf, 1, MPI_INTEGER, from, 0, MPI_COMM_WORLD, &request);

		dest = (rank + 1) % size;
		MPI_Ssend(&sendBuf, 1, MPI_INTEGER, dest, 0, MPI_COMM_WORLD);

		// Block until receiving is done
		MPI_Wait(&request, MPI_STATUS_IGNORE);

  	// sum its own value up with the received value
		value += receiveBuf;

		// Update send buffer to what it has received
		sendBuf = receiveBuf;
	}

	return value;
}

int main(void) {
  MPI_Init(NULL, NULL);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  double tstart, tstop;
  MPI_Barrier(MPI_COMM_WORLD);  // Line up at the start line
  tstart = MPI_Wtime();         // Fire the gun and start the clock
  int value;
  // Put it in a loop to make it last longer
  for (int i = 0; i < 10000000; i++) {
  	// Replace irecv_ring() with isend_ring()
  	value = irecv_ring(rank, size);
  }
	MPI_Barrier(MPI_COMM_WORLD);  // Wait for everyone to finish
  tstop = MPI_Wtime();          // Stop the clock
  printf("Rank %d: value=%d time=%fs\n", rank, value, tstop-tstart);

	MPI_Finalize();

  return 0;
}


// PERFORMANCE NOTES
// $ mpirun -n 1 aroundaring
// Rank 0: value=1 time=0.060934s
// $ mpirun -n 2 aroundaring
// Rank 0: value=5 time=6.747883s
// Rank 1: value=5 time=6.747876s
// $ mpirun -n 3 aroundaring
// Rank 0: value=14 time=15.333231s
// Rank 1: value=14 time=15.333233s
// Rank 2: value=14 time=15.333259s
// $ mpirun -n 4 aroundaring
// Rank 1: value=30 time=35.051162s
// Rank 2: value=30 time=35.051238s
// Rank 3: value=30 time=35.051163s
// Rank 0: value=30 time=35.051246s


