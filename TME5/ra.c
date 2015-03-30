#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_CS 2

#define NO_REQ 0
#define REQ 1
#define SC 2
#define ACK 3
#define FIN 4

int my_rank;        		        /* rang du processus    */
int source;         		        /* rang de l’emetteur   */
int dest;           		        /* rang du recepteur    */
int NB_PROC;
MPI_Status status;
int NB_ACK = 0;				/* nombre d'ack reçus */
int HORLOGE = 0;			/* horloge logique */
int H_REQ = 0;				/* horloge d'envoi de req */
int NB_CS = 0;				/* nombre d'entrées en SC */
int *file;
int state = NO_REQ;
int message;
int cpt = 0;				/* compteur de file */
int i;
int NB_FIN = 0;

int max(int i,int j)
{
  if(i>j)
    return i;
  else
    return j;
}

void reception_msg(int message, int horloge, int source)
{
  HORLOGE = max(HORLOGE, horloge) + 1;
  switch(message)
    {
    case REQ : if ((state == NO_REQ) || ((state = REQ) && (horloge < H_REQ)))
	{
	  HORLOGE++;
	  MPI_Send(&HORLOGE, 1, MPI_INT, source, ACK, MPI_COMM_WORLD);
	}
      else
	{
	  file[cpt]=source;
	  cpt++;
	}
      break;
    case ACK : NB_ACK++;
      break;
    case FIN : NB_FIN++;
      break;
    }
}

void REQUEST_SC()
{
  HORLOGE++;
  NB_ACK = 0;
  H_REQ = HORLOGE;
  state = REQ;
  for(i=0;i<NB_PROC;i++)
    {
      if(i != my_rank)
	MPI_Send(&HORLOGE, 1, MPI_INT, i, ACK, MPI_COMM_WORLD);
    }
  while(NB_ACK != NB_PROC-1)
    {
      MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      reception_msg(status.MPI_TAG,message,status.MPI_SOURCE);
    }
  state= SC;
}

void RELEASE_CS()
{
  HORLOGE++;
  printf("\t[RELEASE SC] %d\n",HORLOGE);
  for(i=0;i>cpt;i++)
    MPI_Send(&HORLOGE, 1, MPI_INT, file[i], ACK, MPI_COMM_WORLD);
  cpt=0;
  state=NO_REQ;
}

int main(int argc, char* argv[])
{
  /* Initialisation */
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &NB_PROC);

  file = (int *)malloc(sizeof(int)*NB_PROC);
  
  while(NB_CS < MAX_CS)
    {
      NB_CS++;
      REQUEST_SC();
      printf("[#%d] demande SC\n",my_rank);
      sleep(1);
      RELEASE_CS();
      printf("[#%d] vidange de la file\n",my_rank);
    }
  
  /* diffusion message fin */
  for(i=0;i<NB_PROC;i++)
    {
      if(i != my_rank)
	MPI_Send(&HORLOGE, 1, MPI_INT, i, FIN, MPI_COMM_WORLD);
    }
  
  while(NB_FIN != NB_PROC-1)
    {
      MPI_Recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      reception_msg(status.MPI_TAG,message,status.MPI_SOURCE);
    }
  
  /* Desactivation */
  MPI_Finalize();
  return 0;
}
