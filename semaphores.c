#include "semaphores.h"

/* Semaphore P - down operation, using semop */
int sem_P(int sem_id, int semNum) {
    struct sembuf sem_d;
    
    sem_d.sem_num = semNum;
    sem_d.sem_op = -1;
    sem_d.sem_flg = 0;
    if (semop(sem_id, &sem_d, 1) == -1) {
        perror("# Semaphore down (P) operation ");
        return -1;
    }
    return 0;
}

/* Semaphore V - up operation, using semop */
int sem_V(int sem_id,int semNum) {
    struct sembuf sem_d;

    sem_d.sem_num = semNum;
    sem_d.sem_op = 1;
    sem_d.sem_flg = 0;
    if (semop(sem_id, &sem_d, 1) == -1) {
        perror("# Semaphore up (V) operation ");
        return -1;
    }
    return 0;
}

/* Semaphore Init - set a semaphore's value to val */
int sem_Init(int sem_id, int val, int semNum) {
    union semun arg;

    arg.val = val;
    if (semctl(sem_id, semNum, SETVAL, arg) == -1) {
        perror("# Semaphore setting value ");
        return -1;
    }
    return 0;
}

void free_resources(int shm_id, int sem_id) { 
    /* Delete the shared memory segment */
    shmctl(shm_id,IPC_RMID,NULL);          
    /* Delete the semaphores */
    for(int i=0; i < 3; i++) {
        semctl(sem_id,i,IPC_RMID,0);
    }
}