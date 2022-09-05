#include "clientFunct.h"
#include "helperFuncts.h"

int clientFunct(int semId, int shmId, int lines, int n) {
    //printf("# Child (%d) created\n",getpid());

    char *mem;
    double startTime, endTime;
    double sum;

    mem = (char *) shmat(shmId, NULL, 0);  //attach shared memory
    if (mem == NULL) {
        perror("Shared memory attach ");
        return -1;
    }

    for (int i=0; i < n; i++) {

        if (sem_P(semId,0) == -1) {  //0 semaphore blocks the other children from writing a request
            return -1;
        }

        int lineReq = (rand() % lines) + 1;
        if (cast_to_string(lineReq,mem) == NULL) {        //write request
            perror("Cast to string ");
            return -1;
        }

        //printf("(%d): Client request %s \n",getpid(),mem); 

        startTime = (double) clock();
        startTime /= CLOCKS_PER_SEC;           //time in seconds

        if (sem_V(semId,2) == -1) {    //wake up father to write respnse
            return -1;
        }

        if (sem_P(semId,1) == -1) {   //wait for father to write response
            return -1;
        }

        endTime = ( ((double) clock()) / CLOCKS_PER_SEC) - startTime;
        sum += endTime;

        //printf("(%d): The elapsed time is %f seconds\n", getpid(),endTime);
        //printf("\n(%d): For request in loop %d Client received: %s",getpid(),i+1,mem);

        if (sem_V(semId,0) == -1) {  //free shared memory resource (unblock other child processes)
            return -1;
        }

    }

    shmdt(mem);     //detach shared memory
    
    printf("->(%d): Average wait time: %f seconds\n",getpid(),( ((double) sum) / ((double) n) ));
    return 0;
}