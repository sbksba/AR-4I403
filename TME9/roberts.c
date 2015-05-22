#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define ELU      0
#define INIT     1
#define TAG     10
#define TAG_ELU 11
#define N       10
   
int initiateur;
int etat = 0; //0 si pas battu
              //1 si battu
int elu = 0;

int roberts(int rang)
{
  int msg;
  MPI_Status status;
  int i;

  MPI_Recv(&msg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  
  // EN COURS D'ELECTION
  if (status.MPI_TAG == TAG)
    {
      if (initiateur == 1) //Si je suis initiateur
	{
	  if (msg > rang)
	    {
	      MPI_Send(&msg, 1, MPI_INT, (rang + 1) % N, TAG, MPI_COMM_WORLD);
	    }
	  else if (msg == rang)
	    {
	      elu = 1;
	      MPI_Send(&msg, 1, MPI_INT, (rang + 1) % N, TAG_ELU, MPI_COMM_WORLD);
	    }
	}
      else { // Si je ne suis pas initiateur
	etat = 1; // je suis battu
	MPI_Send(&msg, 1, MPI_INT, (rang + 1) % N, TAG, MPI_COMM_WORLD); // Je l'envoie à mon successeur
      }
    }
  //ELECTION FINIE ENVOI DE ELU
  else if (status.MPI_TAG = TAG_ELU)
    {
      if (msg == rang)
	{
	  printf("[#%d] #%d a été elu\n",rang, msg);
	  return TAG_ELU;
	}
      else
	{
	  MPI_Send(&msg, 1, MPI_INT, (rang + 1) % N, TAG_ELU, MPI_COMM_WORLD);
	  printf("[#%d] #%d a été elu\n",rang, msg);
	  return TAG_ELU;
	}
    }
}

int estInitiateur()
{
  int init;
  srand(getpid());
  init = rand()%2;
  
  return init;
}

int main(int argc, char **argv)
{
  int nb_proc, rang;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
  MPI_Comm_rank(MPI_COMM_WORLD, &rang);
 
  if (nb_proc != N)
    {
      printf("ERREUR NB ARGUMENTS !!!\n");
      MPI_Finalize();
      return EXIT_FAILURE;
    }

  initiateur = estInitiateur();
  
  if (initiateur)
    {
      printf("\t[#%d] initiateur\n",rang);
      MPI_Send(&rang, 1, MPI_INT, (rang + 1) % N, TAG, MPI_COMM_WORLD);
    }

  while (roberts(rang) != TAG_ELU);

  MPI_Finalize();

  return EXIT_SUCCESS;
}
