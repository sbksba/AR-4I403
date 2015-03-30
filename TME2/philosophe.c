#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>
#include <time.h>

#define NB_MEALS 3
#define WANNA_CHOPSTICKS 99
#define DONE_EATING

#define THINKING 4
#define HUNGRY 5
#define EATING 6

int rand_a_b(int a, int b) {return rand()%(b-a) +a;}

void manageMess(int message, MPI_Status status)
{
  
}

int main(int argc, char * argv[]){

  int nb;
  int myRank;
  int message;
  int tag = 0;
  int nbr = 0;
  int etat_g = 0;
  int etat_d = 0;
  int bag_g = 0;
  int bag_d = 0;
  int mon_etat = 4;
  MPI_Status status;

  MPI_Init(& argc, & argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nb);
  MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

  while(nbr != NB_MEALS && g != NB_MEALS && d != NB_MEALS){
    
    srand(getpid());
    printf("[PHI #%d] THINKING.\n",myRank);
    sleep(rand_a_b(0, 3));
    
    printf("[PHI #%d] HUNGRY.\n",myRank);
    MPI_Send(WANNA_CHOPSTICKS, 1, MPI_INT, (myRank+1) % nbTask, tag, MPI_COMM_WORLD);
    MPI_Send(WANNA_CHOPSTICKS, 1, MPI_INT, (myRank-1) % nbTask, tag, MPI_COMM_WORLD);
    
    while (bag_g = 0 && bag_d = 0)
      {
	MPI_Recv(message, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
	
	if (status.MPI_SOURCE == (myRank+1)%nbTask)
	  {
	    bag_g = 1;
	    
	  }
	else if (status.MPI_SOURCE == (myRank-1) % nbTask)
	  {
	    bag_d = 1;
	  }
      }

    printf("[PHI #%d] EATING.\n",myRank);
    sleep(rand_a_b(0, 3));
    
    nbr++;
  }

  MPI_Finalize();
}
