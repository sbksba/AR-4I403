#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAGINIT    0
#define TAGPRE     1
#define TAGPOST    2
#define NB_SITE 6

int voisins[3], *recu = NULL;
int minLocal = -1, minRecu = -1, nbVoisins = 0;
int nbRecus = 0, dernierRecu = -1;

void simulateur(void)
{
  int i;
  int nb_voisins[NB_SITE+1] = {-1, 2, 3, 2, 1, 1, 1};
  int min_local[NB_SITE+1] = {-1, 3, 11, 8, 14, 5, 17};

  int voisins[NB_SITE+1][3] = {{-1, -1, -1},
			       {2, 3, -1}, {1, 4, 5}, 
			       {1, 6, -1}, {2, -1, -1},
			       {2, -1, -1}, {3, -1, -1}};
                               
  for(i=1; i<=NB_SITE; i++)
    {
      MPI_Send(&nb_voisins[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
      MPI_Send(voisins[i],nb_voisins[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
      MPI_Send(&min_local[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD); 
    }
}

void initArbre()
{
  MPI_Status status;

  /* RECV DATA */
  MPI_Recv(&nbVoisins, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  MPI_Recv(voisins, nbVoisins, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  MPI_Recv(&minLocal, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  recu = (int*) malloc( sizeof(int)*nbVoisins );
}

void calcul_min(int rang)
{
  MPI_Status status;  
  int i; 
  
  for(i=0; i<nbVoisins; i++)
    recu[i] = -1;
  
  /* RECV (nbVoisins-1) messages */
  while(nbRecus < nbVoisins - 1)
    {
      MPI_Recv(&minRecu, 1, MPI_INT, MPI_ANY_SOURCE, TAGPRE, MPI_COMM_WORLD, &status);
      nbRecus++;
      
      for(i=0; i<nbVoisins; i++){
	if(status.MPI_SOURCE == voisins[i])
	  {
	    recu[i] = minRecu;
	    break;
	  }
      }
      if(minRecu < minLocal) minLocal = minRecu;
    }

  /* SEARCH du seul voisin de qui on a pas reçu de valeur */
  i=0;
  while(i<nbVoisins && recu[i] != -1) i++;

  /* SEND au dernier voisin de notre valeur locale */
  MPI_Send(&minLocal, 1, MPI_INT, voisins[i], TAGPRE, MPI_COMM_WORLD);

  /* WAIT un message d'un voisin */
  MPI_Recv(&minRecu, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
  dernierRecu = status.MPI_SOURCE;

  /* Si TAGPRE : message avant décision, envoyé du dernier voisin, lui et nous décideur */
  if(status.MPI_TAG == TAGPRE)
    {
      nbRecus++;
      for(i=0; i<nbVoisins; i++)
	{
	  if(status.MPI_SOURCE == voisins[i])
	    {
	      recu[i] = minRecu;
	      break;
	    }
	}
      printf("[DECIDEUR] #%d (last : #%d)\n", rang, dernierRecu);
    }
  
  if(minRecu < minLocal) minLocal = minRecu;

  /* DIFFUSION */
  for(i=0; i<nbVoisins; i++)
    {
      if(voisins[i] != dernierRecu)
	MPI_Send(&minLocal, 1, MPI_INT, voisins[i], TAGPOST, MPI_COMM_WORLD);
    }

  printf("[END] #%d : min = %d\n", rang, minLocal);
}

int main (int argc, char* argv[])
{
  int nb_proc,rang;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
  
  if (nb_proc != NB_SITE+1)
    {
      printf("Nombre de processus incorrect !\n");
      MPI_Finalize();
      exit(2);
    }
  
  MPI_Comm_rank(MPI_COMM_WORLD, &rang);
  
  if (rang == 0)
    simulateur();
  else
    {
      initArbre();
      calcul_min(rang);
    }
  
  MPI_Finalize();
  return 0;
}
