#include<stdio.h>		
#include<time.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<pthread.h>
#include<sched.h>

#define LOWER_TEMP_BOUND 0
#define UPPER_TEMP_BOUND 50.00
#define LOWER_PRESURE_BOUND 80
#define UPPER_PRESURE_BOUND 150.00

int temperature;
int pressure;

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER;

void timespec_add_us(struct timespec *t, long us){
	t->tv_nsec += us*1000;
	while (t->tv_nsec > 1000000000){
		t->tv_nsec = t->tv_nsec - 1000000000;// + ms*1000000;
		t->tv_sec += 1;
	}
}

void* temp(void *arg){
	struct timespec next;
	int *a = malloc(sizeof(int));
	
	clock_gettime(CLOCK_MONOTONIC, &next); // not using the REALTIME (clockid_t clock_id)
	a = &temperature;

    while(1){
        pthread_mutex_lock(&lock1);
        timespec_add_us(&next, 5000000);//every 5 seconds
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL); // part of the POSIX RT profile
		// ITMER_ABSTIME = flag - used to decided if we want to suspend for a relative amount of 
		// time, or until an absolute point in time.
		*a = LOWER_TEMP_BOUND +(int)(UPPER_TEMP_BOUND*rand()/(RAND_MAX+1.0));

		printf("(First Thread) Temperature: %d *C\t\t\t",*a);
		printf("Time: %ld s %ld ns \n",next.tv_sec,next.tv_nsec);
        pthread_mutex_unlock(&lock2);
    }
    return NULL;
}

void* pres(void *arg){
	struct timespec next;
	int *a = malloc(sizeof(int));
	clock_gettime(CLOCK_MONOTONIC, &next);
	a = &pressure;

    while(1){
        pthread_mutex_lock(&lock2);
		timespec_add_us(&next, 5000000);//every 5 seconds
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
		*a = LOWER_PRESURE_BOUND +(int)(UPPER_PRESURE_BOUND*rand()/(RAND_MAX+1.0));
		
		printf("(Second Thread) Pressure: %d kPa\t\t\t",*a);
		printf("Time: %ld s %ld ns \n",next.tv_sec,next.tv_nsec);
        pthread_mutex_unlock(&lock3);
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
        pthread_mutex_lock(&lock3);
		timespec_add_us(&next, 5000000);//add 5 seconds	
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
		// for(j=0;j<100000;j++);//delay a bit for late start
		printf("(Third Thread) Temperature: %d *C and Pressure: %d kPa\t",*localtemp,*localpres);	
		printf("Time: %ld s %ld ns \n\n",next.tv_sec,next.tv_nsec);
        pthread_mutex_unlock(&lock1);
    }
    return NULL;
}

int main(int ac, char **av){
    pthread_t tid1,tid2,tid3;
    pthread_attr_t my_attr;
    struct sched_param param1, param2, param3;

    pthread_attr_init(&my_attr);
    pthread_attr_setinheritsched(&my_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&my_attr, SCHED_FIFO);

    param1.sched_priority = 3;
    param2.sched_priority = 2;
    param3.sched_priority = 1;

    pthread_mutex_lock(&lock2);
    pthread_mutex_lock(&lock3);

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

    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);
    pthread_mutex_destroy(&lock3);

    return 0;
}
'''
In this code, three mutex locks are created. Initially, lock2 and lock3 are locked in the main function. 
This means that the threads associated with these locks will wait until the locks are unlocked. 
The temp thread (First Thread) is the only one that can start because lock1 is not locked. 
At the end of its execution, it unlocks lock2, allowing the pres thread (Second Thread) to execute. 
This thread then unlocks lock3, allowing the dis thread (Third Thread) to execute. 
This thread finally unlocks lock1, allowing the temp thread to execute again. This cycle continues, ensuring the order of execution.
'''