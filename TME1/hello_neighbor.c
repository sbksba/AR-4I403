#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>


#define SIZE 128


int main(int argc, char * argv[]){

	int nbTask;
	int myRank;
	char message[SIZE];
	int tag = 0;
	
	MPI_Status status;
	
	MPI_Init(& argc, & argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nbTask);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	
	
	sprintf(message, "Hello Neighbor from %d", myRank); 
	
	
	if (myRank != 0)
	{	
		MPI_Ssend(message, strlen(message) + 1, MPI_CHAR, (myRank + 1) % nbTask, tag, 
			MPI_COMM_WORLD);
	
		MPI_Recv(message, SIZE, MPI_CHAR, (myRank - 1) % nbTask, tag, 
			MPI_COMM_WORLD, &status);
		printf("[%d] %s\n", myRank, message);
	}
	else
	{ 
		MPI_Recv(message, SIZE, MPI_CHAR, nbTask-1, tag, MPI_COMM_WORLD, &status);
		printf("[%d] %s\n", myRank, message);
		
		MPI_Ssend(message, strlen(message) + 1, MPI_CHAR, (myRank + 1) % nbTask, tag, 
			MPI_COMM_WORLD);		
	}	
	MPI_Finalize();
	
	return 0;
}
