#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<pthread.h>
#include<sched.h>
#include<semaphore.h>

#define LOWER_TEMP_BOUND 0
#define UPPER_TEMP_BOUND 50.00
#define LOWER_PRESURE_BOUND 80
#define UPPER_PRESURE_BOUND 150.00

int temperature;
int pressure;

sem_t sem1, sem2, sem3;

void timespec_add_us(struct timespec *t, long us){
    t->tv_nsec += us*1000;
    while (t->tv_nsec > 1000000000){
        t->tv_nsec = t->tv_nsec - 1000000000;
        t->tv_sec += 1;
    }
}

void* temp(void *arg){
    struct timespec next;
    int *a = malloc(sizeof(int));

    clock_gettime(CLOCK_MONOTONIC, &next);
    a = &temperature;

    while(1){
        sem_wait(&sem1);
        timespec_add_us(&next, 5000000);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
        *a = LOWER_TEMP_BOUND +(int)(UPPER_TEMP_BOUND*rand()/(RAND_MAX+1.0));

        printf("(First Thread) Temperature: %d *C\t\t\t",*a);
        printf("Time: %ld s %ld ns \n",next.tv_sec,next.tv_nsec);
        sem_post(&sem2);
    }
    return NULL;
}

void* pres(void *arg){
    struct timespec next;
    int *a = malloc(sizeof(int));
    clock_gettime(CLOCK_MONOTONIC, &next);
    a = &pressure;

    while(1){
        sem_wait(&sem2);
        timespec_add_us(&next, 5000000);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
        *a = LOWER_PRESURE_BOUND +(int)(UPPER_PRESURE_BOUND*rand()/(RAND_MAX+1.0));

        printf("(Second Thread) Pressure: %d kPa\t\t\t",*a);
        printf("Time: %ld s %ld ns \n",next.tv_sec,next.tv_nsec);
        sem_post(&sem3);
    }
    return NULL;
}

void* dis(void *arg){
    struct timespec next;
    int *localtemp = malloc(sizeof(int));
    int *localpres = malloc(sizeof(int));

    clock_gettime(CLOCK_MONOTONIC, &next);
    localtemp = &temperature;
    localpres = &pressure;

    while(1){
        sem_wait(&sem3);
        timespec_add_us(&next, 5000000);
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
        printf("(Third Thread) Temperature: %d *C and Pressure: %d kPa\t",*localtemp,*localpres);
        printf("Time: %ld s %ld ns \n\n",next.tv_sec,next.tv_nsec);
        sem_post(&sem1);
    }
    return NULL;
}

int main(int ac, char **av){
    pthread_t tid1,tid2,tid3;
    pthread_attr_t my_attr;
    struct sched_param param1, param2, param3;

    sem_init(&sem1, 0, 1);
    sem_init(&sem2, 0, 0);
    sem_init(&sem3, 0, 0);

    pthread_attr_init(&my_attr);
    pthread_attr_setinheritsched(&my_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&my_attr, SCHED_FIFO);

    param1.sched_priority = 3;
    param2.sched_priority = 2;
    param3.sched_priority = 1;

    pthread_attr_setschedparam(&my_attr, &param1);
    pthread_create((&tid1), &my_attr, &temp, NULL);
    pthread_attr_setschedparam(&my_attr, &param2);
    pthread_create((&tid2), &my_attr, &pres, NULL);
    pthread_attr_setschedparam(&my_attr, &param3);
    pthread_create((&tid3), &my_attr, &dis, NULL);
    pthread_attr_destroy(&my_attr);

    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);

    sem_destroy(&sem1);
    sem_destroy(&sem2);
    sem_destroy(&sem3);

    return 0;
}

"""
In the given program, semaphores are used to synchronize the execution of threads and control access to shared resources. 

There are three semaphores defined: `sem1`, `sem2`, and `sem3`. 

- `sem1` is used to synchronize the `temp` thread and the `dis` thread. 
It ensures that the `dis` thread waits until the `temp` thread has completed its task before accessing the shared resources.
- `sem2` is used to synchronize the `temp` thread and the `pres` thread. 
It ensures that the `pres` thread waits until the `temp` thread has completed its task before accessing the shared resources.
- `sem3` is not used in the provided code.

Here's how the semaphores are used in the program:

1. In the `temp` thread, after printing the temperature, `sem_post(&sem2)` is called to signal the `pres` thread that it can proceed.
2. In the `pres` thread, `sem_wait(&sem2)` is called to wait until the `temp` thread signals that it has completed its task. 
Once the semaphore is available, the `pres` thread can access the shared resources (in this case, the `pressure` variable).
3. The `dis` thread does not directly use semaphores in the provided code.

By using semaphores, the program ensures that the threads execute in a synchronized manner, preventing race conditions and ensuring proper access to shared resources.
"""