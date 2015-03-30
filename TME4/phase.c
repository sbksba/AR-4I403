#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define TAGINIT    0
#define TAGCALCULMIN 1
#define NB_SITE 6

#define DIAMETRE 5		/* !!!!! valeur a initialiser !!!!! */

int nb_voisins_in;
int nb_voisins_out;
int voisins_in[2];
int voisins_out[2];
int min_local;

int *RCount;
int SCount;

int condition_Send()
{
  int i;
  
  for(i=0;i<nb_voisins_in; i++)
    if(!((RCount[i] >= SCount) && (SCount < DIAMETRE)))
      return 0;
   
  return 1;
}

int condition_Decision()
{
  int i;
  
  for(i=0 ; i<nb_voisins_in ; i++)
    if(!(RCount[i] >= DIAMETRE))
      return 0;

  return 1;
}

void calcul_min(rang)
{
  int i;

  SCount = 0;
  
  MPI_Status status;
  
  /* Recuperation des valeurs */
  MPI_Recv(&nb_voisins_in, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
  MPI_Recv(&nb_voisins_out, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
  MPI_Recv(voisins_in, nb_voisins_in, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
  MPI_Recv(voisins_out, nb_voisins_out, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
  MPI_Recv(&min_local, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status);
  
  /*printf("in: %d | %d\n", voisins_in[0], voisins_in[1]);*/

  RCount = (int *) malloc(nb_voisins_in * sizeof(int));
  for(i=0;i<nb_voisins_in;i++) RCount[i] = 0;
  
  printf("[INIT] #%d initialised\n", rang);
  
  int tmp_min;
  while(1)
    {
      /* Si la condition de decision est valide on sort de la boucle */
      if(condition_Decision()) {break;}
      
      /* Si la condition d envoie est valide alors on envoie a tous ces voisins */
      if(condition_Send())
	{
	  for(i=0 ; i < nb_voisins_out ; i++)
	    {
	      MPI_Send(&min_local, 1, MPI_INT, voisins_out[i], TAGCALCULMIN, MPI_COMM_WORLD); 
	    }
	  
	  SCount++;
	}
      else
	{
	  MPI_Recv(&tmp_min, 1, MPI_INT, MPI_ANY_SOURCE, TAGCALCULMIN, MPI_COMM_WORLD, &status);
	  
	  for(i=0;i<nb_voisins_in;i++)
	    {
	      if(voisins_in[i] == status.MPI_SOURCE) break;
	    }
	  
	  min_local = tmp_min < min_local ? tmp_min : min_local;
	  printf("\t[MIN] #%d : min -> %d\n", rang, min_local);

	  RCount[i]++;
	}    
    }
  
  printf("\n-- [DECIDE] #%d find %d as min --\n", rang, min_local);
}

void simulateur(void)
{
  int i;
  
  /* nb_voisins_in[i] est le nombre de voisins entrants du site i */
  /* nb_voisins_out[i] est le nombre de voisins sortants du site i */
  int nb_voisins_in[NB_SITE+1] = {-1, 2, 1, 1, 2, 1, 1};
  int nb_voisins_out[NB_SITE+1] = {-1, 2, 1, 1, 1, 2, 1};

  int min_local[NB_SITE+1] = {-1, 4, 7, 1, 6, 2, 9};

  /* liste des voisins entrants */
  int voisins_in[NB_SITE+1][2] = {{-1, -1},
				  {4, 5}, {1, -1}, {1, -1},
				  {3, 5}, {6, -1}, {2, -1}};
                               
  /* liste des voisins sortants */
  int voisins_out[NB_SITE+1][2] = {{-1, -1},
				   {2, 3}, {6, -1}, {4, -1},
				   {1, -1}, {1, 4}, {5,-1}};

  for(i=1; i<=NB_SITE; i++){
    MPI_Send(&nb_voisins_in[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
    MPI_Send(&nb_voisins_out[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
    MPI_Send(voisins_in[i], nb_voisins_in[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
    MPI_Send(voisins_out[i], nb_voisins_out[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);    
    MPI_Send(&min_local[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD); 
  }
}

/******************************************************************************/

int main (int argc, char* argv[])
{
  int nb_proc,rang;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nb_proc);
  
  if (nb_proc != NB_SITE+1) {
    printf("Nombre de processus incorrect ! [CORRECT = %d]\n", NB_SITE+1);
    MPI_Finalize();
    exit(2);
  }
  
  MPI_Comm_rank(MPI_COMM_WORLD, &rang);
  
  if (rang == 0) {
    simulateur();
  } else {
    calcul_min(rang);
  }
  
  MPI_Finalize();
  return 0;
}
