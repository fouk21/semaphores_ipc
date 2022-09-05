/* Christos-Georgios Foukanelis
   1115201900204
   Ergasia 1 - Leitourgika Systhmata
*/


#include "semaphores.h"
#include "clientFunct.h"

int main (int argc, char *argv[]) {

    int shm_id;
    int sem_id;
    int pid;
    void *sh;     //shared memory pointer
    char **data = NULL;  //array that holds each line of the input file as a string
    int status;

    srand(time(NULL)); 

    const int K = atoi(argv[2]);
    const int N = atoi(argv[3]);

    //double *avgTimes = malloc(K * sizeof(double));

    /* Create a new shared memory segment */
    shm_id = shmget(IPC_PRIVATE, 100, IPC_CREAT | 0660);   //memory segment is 100 bytes
    if (shm_id == -1) {
        perror("Shared memory creation");
        exit(EXIT_FAILURE);
    }

    /* Create a new semaphore id */
    sem_id = semget(IPC_PRIVATE, 3, IPC_CREAT | 0660);  
    if (sem_id == -1) {
        perror("Semaphore creation ");
        shmctl(shm_id, IPC_RMID, (struct shmid_ds *) NULL);
        exit(EXIT_FAILURE);
    }

    //initializing semaphores
    if (sem_Init(sem_id,1,0) == -1) {     //set to 1
        free_resources(shm_id, sem_id);   //semaphore 0 blocks other processes from requesting something from the server
        exit(EXIT_FAILURE);
    }


    if (sem_Init(sem_id,0,1) == -1) {    //set to 0
        free_resources(shm_id, sem_id);  //semaphore 1 blocks the child process while the parent is writing the response
        exit(EXIT_FAILURE);
    }
        
    if (sem_Init(sem_id,0,2) == -1) {    //set to 0
        free_resources(shm_id, sem_id);  //semaphore 2 blocks the father from accesing the memory while another child has accessed it
        exit(EXIT_FAILURE);
    }
    

    /* Attach the shared memory segment */
    sh = (void *) shmat(shm_id, NULL, 0);
    if (sh == NULL) {
        perror("Shared memory attach ");
        free_resources(shm_id, sem_id);
        exit(EXIT_FAILURE);
    }

    const char *X = argv[1];     //input file
    char buf[100];               //each line has a maximum of 100 characters
    FILE *fp;
    fp = fopen(X,"rb");
    if (fp == NULL) {
        printf("Error\n");
        perror("fopen source-file");
        return 1;
    }
    int lineCounter = 0;
    while (fgets(buf,sizeof buf,fp) != NULL) {
        lineCounter++;
        data = realloc(data, lineCounter * sizeof(char *));
        data[lineCounter-1] = malloc(sizeof(sizeof(buf) * sizeof(char)));  
        strcpy(data[lineCounter-1],buf);                                 //saving current line
        //printf("line %d: %s",lineCounter,data[lineCounter-1]);

    }
    fclose(fp);
    printf("\n");

    for (int i=0; i < K; i++) {                //creating K child processes
        if ((pid = fork()) == -1) { 
            perror("fork");
            free_resources(shm_id, sem_id);
            exit(EXIT_FAILURE);
        }
        if (!pid) {break;}
    }

    if (pid == 0) { 
        /* Child process */
        if (clientFunct(sem_id,shm_id,lineCounter,N) == -1) {  //child process commands (exec not used)
            free_resources(shm_id, sem_id);
            exit(EXIT_FAILURE);
        }
    } 
    else {
        /* Parent process */
        //printf("# I am the parent process with process id: %d\n", getpid());
        for(int j=0; j < N*K; j++) {
            if (sem_P(sem_id,2) == -1) {         //check if a child has sent you a request
                free_resources(shm_id, sem_id);
                exit(EXIT_FAILURE);
            }
            
            int request = atoi(sh);  //read child request
            //printf("(%d): Server received request: %d\n",getpid(),request);

            strcpy(sh,data[request-1]);  //write response
            //printf("(%d): Server response: %s",getpid(),data[request-1]);
            
            if (sem_V(sem_id,1) == -1) {  //wake up kid
                free_resources(shm_id, sem_id);
                exit(EXIT_FAILURE);
            }
        }
    }

    if (!pid) {
        exit(EXIT_SUCCESS);
    }
    
    int wpid;
    /* Wait for child processes */
    for (int j = 0; j < K; j++) {
        if ((wpid = wait(&status)) == -1) {
            perror("Wait ");
        }
        if (status == 1) {
            free_resources(shm_id,sem_id);
            return 1;
        }
        //printf("# Child (%d) exited with status: %d\n",wpid,status);
    }

    /* Clear recourses */
    free_resources(shm_id,sem_id);

    //free memory
    for (int l = 0; l < lineCounter; l++) {
        free(data[l]);
    }
    free(data);
    
    return 0;
}
