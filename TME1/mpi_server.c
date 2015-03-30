#include "mpi_server.h"

#define SIZE 256
#define TAG 10

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


static server the_server;

int f = 0;

void *thread_server() {
	int flag = 0;
	MPI_Status status;
	
	while(1){
		pthread_mutex_lock(&the_server.mutex);
		MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
		pthread_mutex_unlock(&the_server.mutex);
		
		if (flag != 0)
		{
			printf("%d : You've got a message.\n", status.MPI_SOURCE);
			the_server.callback(status.MPI_TAG, status.MPI_SOURCE);
			pthread_cond_signal(&cond);
			printf("SIGNAL\n");
			
		}
	}
	
	pthread_exit((void *)0);
}

void start_server(void (*callback) (int tag, int source)){
	the_server.callback = callback;
	the_server.mutex = mutex;
	
	if (pthread_create(&(the_server.listener), NULL, thread_server,
		NULL) != 0){
		perror("pthread_create");
		exit(1);
	}	
}

void destroy_server(){
	the_server.listener = -1;
	the_server.callback = 0;
	pthread_mutex_destroy(&the_server.mutex);
}

pthread_mutex_t * getMutex(){
	return &the_server.mutex;
}

void callback(int tag, int source){
	char message[SIZE];
	MPI_Status status;
	pthread_mutex_lock(getMutex());

	MPI_Recv(message, SIZE, MPI_CHAR, source , tag, MPI_COMM_WORLD, &status);

	pthread_mutex_unlock(getMutex());
	printf("message recu = %s\n",message);
}

int main(int argc, char *argv[]){
	int msg;
	int rien;
	int my_rank;
	int tag = 0;
	
	char message[100];
	strcpy(message, "Hello!");

	MPI_Init_thread(&argc, &argv, MPI_THREAD_SERIALIZED , &rien);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	start_server(callback);

	MPI_Send(message, strlen(message) + 1, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
	
	printf("BEFORE COND\n");
	pthread_cond_wait(&cond, &mutex);
	//sleep(5);
	printf("AFTER  COND\n");
	
	MPI_Finalize();
	destroy_server();


	return 0;
}
