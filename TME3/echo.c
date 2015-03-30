#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TAGINIT 0
#define DOWN 99
#define UP 98
#define NB_SITE 6

void simulateur (void) {
  int i;
  
  //nb_voisins[i] est le nb de voisins du site i
  int nb_voisins[NB_SITE+1] = {-1, 3, 3, 2, 3, 5, 2};
  int min_local[NB_SITE+1] = {-1, 12, 11, 8, 14, 5, 17};

  //liste des voisins
  int voisins[NB_SITE+1][5] = {{-1, -1, -1, -1, -1},
			       {2, 5,3 , -1, -1}, {4,1,5,-1,-1},
			       {1, 5, -1, -1, -1}, {6, 2, 5, -1, -1},
			       {1, 2, 6, 4, 3}, {4, 5,-1, -1, -1}};

  for(i=1; i<=NB_SITE; i++){
    MPI_Send(&nb_voisins[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
    MPI_Send(voisins[i], nb_voisins[i], MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
    MPI_Send(&min_local[i], 1, MPI_INT, i, TAGINIT, MPI_COMM_WORLD);
  }
}



int convertir_id (int nb_voisins, int* voisins, int id_logique) {
  int i ;

  for (i = 0 ; i < nb_voisins ; i ++)
    if (voisins [i] == id_logique)
      return i ;

  exit (1) ;
}


int dernier_voisin (int nb_voisins, int* voisins, int* recus) {
  int i ;
 
  for (i = 0 ; i < nb_voisins ; i ++)
    if (recus [i] == 0)
      return voisins [i] ;

  exit (1) ;
}


void calcul_min (int rang){
  int nb_voisins, buf, min_local, *voisins, *recus, *fils, nb = 0, i, pere = -1 ;
  MPI_Status status ;

  MPI_Recv (&nb_voisins, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status) ;
  voisins = (int*) malloc (sizeof (int) * nb_voisins) ;
  MPI_Recv (voisins, nb_voisins, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status) ;
  MPI_Recv (&min_local, 1, MPI_INT, 0, TAGINIT, MPI_COMM_WORLD, &status) ;
  recus = (int*) malloc (sizeof (int) * nb_voisins) ;
  fils = (int*) malloc (sizeof (int) * nb_voisins) ;

  for (i = 0 ; i < nb_voisins ; i ++) {
    recus [i] = 0 ;
    fils [i] = 0 ;
  }

  if (rang == 2) { /* P initiateur */
    for (i = 0 ; i < nb_voisins ; i ++)
      MPI_Send (&min_local, 1, MPI_INT, voisins [i], DOWN, MPI_COMM_WORLD) ;

    /* J'attend mes voisins */
    while (nb != nb_voisins) {
      MPI_Recv (&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status) ;
      recus [convertir_id (nb_voisins, voisins, status.MPI_SOURCE)] = 1 ;
      min_local = (buf < min_local) ? buf : min_local ;
      
      if (status.MPI_TAG == UP)
	fils [convertir_id (nb_voisins, voisins, status.MPI_SOURCE)] = 1 ;

      nb ++ ;
    }

    printf ("proc %d LE MINIMUM EST %d\n", rang, min_local) ;
  }

  else { /* P non initiateur */
    while (nb != nb_voisins) {     
      MPI_Recv (&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status) ;
      recus [convertir_id (nb_voisins, voisins, status.MPI_SOURCE)] = 1 ;
      min_local = (buf < min_local) ? buf : min_local ;

      if (status.MPI_TAG == UP)
	fils [convertir_id (nb_voisins, voisins, status.MPI_SOURCE)] = 1 ;
      
      if (pere == -1) {
	pere = status.MPI_SOURCE ;
	for (i = 0 ; i < nb_voisins ; i ++)
	  if (voisins [i] != pere)
	    MPI_Send (&min_local, 1, MPI_INT, voisins [i], DOWN, MPI_COMM_WORLD) ;
      }
      
      nb ++ ;
    }

    MPI_Send (&min_local, 1, MPI_INT, pere, UP, MPI_COMM_WORLD) ;
  }

  

  printf ("%d se termine et le minimum est %d!\n", rang, min_local) ;
  free (fils) ;
  free (recus) ;
  free (voisins) ;
}



int main (int argc, char **argv){
  int nb_proc, rang ;
  MPI_Status status ;
  
  MPI_Init (&argc, &argv) ;
  MPI_Comm_size (MPI_COMM_WORLD, &nb_proc) ;


  if (nb_proc != NB_SITE+1) {
    printf("Nb de processus incorrect !\n") ;
    MPI_Finalize();
    exit(2);
  }
  

  MPI_Comm_rank(MPI_COMM_WORLD, &rang) ;

  if(rang == 0) {
    simulateur();
  } else {
    calcul_min(rang);
  }


  MPI_Finalize () ;
  return 0;
}
