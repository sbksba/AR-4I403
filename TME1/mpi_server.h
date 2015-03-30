#ifndef MPI_SERVER
#define MPI_SERVER
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <mpi.h>


typedef struct server {
	pthread_t listener;
	pthread_mutex_t mutex;
	
	void (*callback) (int tag, int source);
} server;

void start_server(void (*callback) (int tag, int source));

void destroy_server();

pthread_mutex_t * getMutex();

#endif 
	
