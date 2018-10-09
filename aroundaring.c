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

	// Replace irecv_ring() with isend_ring()
  int value = irecv_ring(rank, size);
  printf("Rank %d: value=%d\n", rank, value);

	MPI_Finalize();

  return 0;
}