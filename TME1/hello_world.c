#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char * argv[]){

	int nbTask;
	int myRank;
	
	MPI_Init(& argc, & argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nbTask);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	
	printf("Je suis le numéro %d parmi %d processus \n", myRank, nbTask);
	
	MPI_Finalize();
	
	return 0;
}
