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
"""
This constant represents the upper bound of the pressure range. 
It is set to 150.00, indicating the maximum pressure value that can be generated.
It generate random temperature and pressure values within the specified ranges. 
"""
int temperature;
int pressure;

void timespec_add_us(struct timespec *t, long us){
	t->tv_nsec += us*1000;
	while (t->tv_nsec > 1000000000){
		t->tv_nsec = t->tv_nsec - 1000000000;// + ms*1000000;
		t->tv_sec += 1;
	}
}

"""
The `timespec_add_us` function is used to add a specified number of microseconds to a `timespec` structure. 

The function takes two parameters:
- `t`: A pointer to a `timespec` structure that represents a point in time.
- `us`: The number of microseconds to add to the `timespec` structure.

The function adds the specified number of microseconds to the `tv_nsec` field of the `timespec` structure. 
If the resulting `tv_nsec` value exceeds 1,000,000,000 (representing one second), the function adjusts the `tv_nsec` value by 
subtracting 1,000,000,000 and increments the `tv_sec` field by 1 to account for the additional second.

This function is used in the provided code to calculate the next point in time for the threads to perform their tasks. 
It ensures that the threads execute at regular intervals by adding a fixed delay of 5 seconds (5000000 microseconds) 
to the current time stored in the `next` `timespec` structure.
"""

void* temp(void *arg){
	struct timespec next;
	int *a = malloc(sizeof(int));

"""
malloc(sizeof(int)): The malloc function is then called to allocate memory of the specified size. 
The sizeof(int) expression is passed as an argument to malloc to allocate memory for a single integer.

int *a: The result of the malloc function, which is a pointer to the allocated memory, is assigned to the integer pointer variable a. 
This allows a to point to the dynamically allocated memory.
"""
	
	clock_gettime(CLOCK_MONOTONIC, &next); // not using the REALTIME (clockid_t clock_id)
	
    """
    The function clock_gettime(CLOCK_MONOTONIC, &next) is used to retrieve the current value of a clock specified by the 
    CLOCK_MONOTONIC constant and store it in the next variable.
    """
    a = &temperature;

	while(1){
		timespec_add_us(&next, 5000000);//every 5 seconds

        """
        The function timespec_add_us(&next, 5000000) is used to add a specified number of microseconds to a timespec structure named next.
        """
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL); // part of the POSIX RT profile
		// ITMER_ABSTIME = flag - used to decided if we want to suspend for a relative amount of 
		// time, or until an absolute point in time.
        """
        The function clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL) is used to suspend the execution of a thread
         until a specified absolute time is reached.
        In the provided code, this function is called to make the thread sleep until the calculated next time stored in the next timespec structure. 
        The CLOCK_MONOTONIC clock is used to measure time, and the TIMER_ABSTIME flag indicates that the next time is an 
        absolute point in time rather than a relative duration.
        By using clock_nanosleep with TIMER_ABSTIME, the code ensures that the thread waits until the specified absolute time is reached before 
        continuing its execution. This allows for precise timing and synchronization of the threads in the program.
        """

		*a = LOWER_TEMP_BOUND +(int)(UPPER_TEMP_BOUND*rand()/(RAND_MAX+1.0));

		printf("(First Thread) Temperature: %d *C\t\t\t",*a);
		printf("Time: %ld s %ld ns \n",next.tv_sec,next.tv_nsec);
	}
	return NULL;
}

"""
The above function, `timespec_add_us`, is used to add a specified number of microseconds to a `timespec` structure. 
It takes a pointer to a `timespec` structure and a long value representing the number of microseconds to add. 
The function increments the `tv_nsec` field of the `timespec` structure by the specified number of microseconds and 
handles the carry-over to the `tv_sec` field if necessary. This function is used to perform time calculations and manipulations in the program.
"""
	
void* pres(void *arg){
	struct timespec next;
	int *a = malloc(sizeof(int));
	clock_gettime(CLOCK_MONOTONIC, &next);
	a = &pressure;

	while(1){
		timespec_add_us(&next, 5000000);//every 5 seconds
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
		*a = LOWER_PRESURE_BOUND +(int)(UPPER_PRESURE_BOUND*rand()/(RAND_MAX+1.0));
		
		printf("(Second Thread) Pressure: %d kPa\t\t\t",*a);
		printf("Time: %ld s %ld ns \n",next.tv_sec,next.tv_nsec);
	}
	return NULL;
}	

void* dis(void *arg){
	struct timespec next;
	int *localtemp = malloc(sizeof(int));
	int *localpres = malloc(sizeof(int));

    """
    The purpose of this allocation is to create a memory space that can be used to store a local copy of the temperature value. 
    By assigning the address of the allocated memory to localtemp, the dis thread can access and read the temperature value indirectly 
    through the pointer localtemp. This allows the dis thread to display the temperature value without directly accessing the global temperature variable.
    """

	clock_gettime(CLOCK_MONOTONIC, &next);
	localtemp = &temperature;
	localpres = &pressure;	

	while(1){
		timespec_add_us(&next, 5000000);//add 5 seconds	
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
		// for(j=0;j<100000;j++);//delay a bit for late start
		printf("(Third Thread) Temperature: %d *C and Pressure: %d kPa\t",*localtemp,*localpres);	
		printf("Time: %ld s %ld ns \n\n",next.tv_sec,next.tv_nsec);
	}	
	return NULL;
}

int main(int ac, char **av){
	pthread_t tid1,tid2,tid3;
	pthread_attr_t my_attr;
	struct sched_param param1, param2, param3;

	pthread_attr_init(&my_attr);
	pthread_attr_setinheritsched(&my_attr, PTHREAD_EXPLICIT_SCHED);
    """
    pthread_attr_setinheritsched(&my_attr, PTHREAD_EXPLICIT_SCHED);: This line sets the inheritance of the scheduling attributes for the thread to explicit. 
    It ensures that the thread's scheduling attributes are explicitly defined and not inherited from the calling thread.
    """
	pthread_attr_setschedpolicy(&my_attr, SCHED_FIFO);
    """
    pthread_attr_setschedpolicy(&my_attr, SCHED_FIFO);: This line sets the scheduling policy of the thread to First-In-First-Out (FIFO). 
    The FIFO policy means that threads with higher priority will be executed before threads with lower priority.
    """

	param1.sched_priority = 3;
	param2.sched_priority = 2;
	param3.sched_priority = 1;

	pthread_attr_setschedparam(&my_attr, &param1);
    """
    pthread_attr_setschedparam(&my_attr, &param1);: This line sets the scheduling parameters for the thread. 
    The param1 structure contains the scheduling priority for the thread. It specifies the priority level at which the thread should be scheduled.
    """

	pthread_create((&tid1), &my_attr, &temp, NULL);
    """
    pthread_create((&tid1), &my_attr, &temp, NULL);: This line creates a new thread with the specified attributes. 
    It creates a thread identified by tid1 using the attributes specified in my_attr. The thread executes the temp function, which is passed as an argument. 
    The last argument, NULL, is used to pass any additional arguments to the thread function (in this case, no additional arguments are passed).
    """

	pthread_attr_setschedparam(&my_attr, &param2);
	pthread_create((&tid2), &my_attr, &pres, NULL);
	pthread_attr_setschedparam(&my_attr, &param3);
	pthread_create((&tid3), &my_attr, &dis, NULL);
	pthread_attr_destroy(&my_attr);
    """
    pthread_attr_destroy(&my_attr);: This line destroys the thread attributes object my_attr. It releases any resources associated with the attributes object.    
    """

	pthread_join(tid1,NULL);
    """
    pthread_join(tid1,NULL);: This line waits for the thread with the ID tid1 to complete its execution. 
    The pthread_join function blocks the calling thread until the specified thread terminates. 
    In this case, the NULL argument is used to discard the return value of the thread.
    """

	pthread_join(tid2,NULL);
	pthread_join(tid3,NULL);
	return 0;
}

