#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

// Etats
#define THINKING 0
#define HUNGRY 1
#define EATING 2

//Messages
#define ACQUIRE 0
#define RELEASE 1

#define NB_MEALS 3

int main(int argc, char* argv[])
{
  int my_rank;        /* rang du processus    */
  int NB_PHILOSOPHES;    /* nombre de processus  */
  int source;         /* rang de l’emetteur   */
  int dest;           /* rang du recepteur    */
  int tag = THINKING;    /* etat du philosophe */
  int state = THINKING;
  int message = RELEASE;
  int i = 0;/* compteur pr les états */
  int NB_EATING = 0;    
  int suivant,precedent,cpt,p;
  MPI_Status status;

  /* Initialisation */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &NB_PHILOSOPHES);

  if (my_rank == 0) precedent = NB_PHILOSOPHES;
  else precedent = my_rank-1;
  if (my_rank == NB_PHILOSOPHES-1) suivant = 0;
  else suivant = my_rank+1;

  while(NB_EATING < NB_MEALS)
    {
      /*MPI_Recv(message, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);*/
      switch(state)
	{
	case HUNGRY :
	  /* On envoie les Send */
	  printf("[HUNGRY] proc #%d\n",my_rank);
	  if (source == suivant)
	    {
	      if (tag == HUNGRY)
		{
		  MPI_Send(message, 1, MPI_INT, precedent, state, MPI_COMM_WORLD);
		  MPI_Recv(message, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
		  if (message == ACQUIRE) message = RELEASE;
		  else
		    {
		      message = ACQUIRE;
		      state = EATING;
		    }
		  MPI_Send(message, 1, MPI_INT, suivant, state, MPI_COMM_WORLD);
		}
	    }
	  else
	    {
	      if (tag == THINKING) { 
		MPI_Send(message, 1, MPI_INT, suivant, state, MPI_COMM_WORLD);
		MPI_Recv(message, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
		if (tag == EATING)  message = RELEASE;
		else
		  {
		    message = ACQUIRE;
		    state = EATING;
		  }
		MPI_Send(message, 1, MPI_INT, precedent, state, MPI_COMM_WORLD);
	      }
	      if(tag == RELEASE)
		{
		  MPI_Recv(message, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
		}
	      if(!state == EATING)
		state = THINKING;
	    }
	  break;
	  
	case THINKING :
	  printf("[THINKING] proc #%d\n",my_rank);
	  for(i = 0; i< 5; i++)
	    {
	      MPI_Recv(message, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
	      if(tag == HUNGRY)
		{
		  tag == RELEASE;
		  MPI_Send(message, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
		}
	    }
	  state = HUNGRY;
	  break;
	  
	case EATING :
	  printf("[EATING] proc #%d\n",my_rank);
	  cpt++;
	  state = THINKING;
	  break;
	  
	default : break;
	}
    }

  sprintf(message, "Coucou du processus  %d!",  my_rank);

  if (my_rank < p-1)
    dest = my_rank+1;
  else
    dest = 0;
    
  // MPI_Send(message, strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
  MPI_Ssend(message, strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    
  if (my_rank == 0)
    source = p-1;
  else
    source = my_rank-1;

  MPI_Recv(message, 100, MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
  printf("%s du rank %d\n", message, status.MPI_SOURCE);

  /* Desactivation */
  MPI_Finalize();
  return 0;
}
