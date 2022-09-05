#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Union semun */
union semun {
    int val;                  /* value for SETVAL */
    struct semid_ds *buf;     /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array;    /* array for GETALL, SETALL */
};

int sem_P(int sem_id, int semNum);  //semNum: semaphore number
int sem_V(int sem_id,int semNum);
int sem_Init(int sem_id, int val, int semNum);
void free_resources(int shm_id, int sem_id);