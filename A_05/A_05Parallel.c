/********************************************
 * Name:    Jered Stevens
 * File:    StevensA4Pthreads.c
 *
 * To compile
 *  - Navigate in terminal to destination folder
 *  - Run the following command
 *      gcc -g -Wall -o dotProduct StevensA4Pthreads.c -lpthread
 *
 * - Run the following command
 *      ./dotProduct <NUMBER OF THREADS>
 *
 * If number of threads is not specified,
 *  the default number of threads is 16.
 ****************************************************/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>


#define numThrows 33554432 // 2^25
int threadCount = 16;
double xThrows[numThrows];
double yThrows[numThrows];
double distance[numThrows];
double hits = 0;
pthread_mutex_t mutex;


bool isPow2(long long int);
void* ThrowDarts(void *args);
void* FindDistance(void* args);
void* FindHits(void* args);
/*****************************************
 * Struct Name: threadArgs
 *
 * Elements: long myRank, int localN
 *
 * Description: Package to be sent to each
 *      thread
 ******************************************/
struct threadArgs
{
    long myRank;
    double localN;
};

int main(int argc, char *argv[])
{

    // long for 64 bit systems
    long thread;

    // initialize mutex
    pthread_mutex_init(&mutex, NULL);

    pthread_t *threadHandles;

    // get # of threads from command line if any
    if (argv[1])
    {
        int arguments = strtol(argv[1], NULL, 10);
        if (isPow2(arguments)){
            threadCount = arguments;
        }else{
            printf("Thread count entered is invalid\n");
            printf("Thread count must be a power of 2.\n");
            printf("Setting thread count to default value of 16.\n");
            threadCount = 16;
        }
    }

    threadHandles = malloc(threadCount * sizeof(pthread_t));

    struct timeval start, end;
    double stopwatch;
    gettimeofday(&start, NULL);

    // Throw Darts
    for (thread = 0; thread < threadCount; thread++)
    {
        struct threadArgs *t_args = malloc(sizeof(struct threadArgs));
        t_args->myRank = thread;
        t_args->localN = numThrows / threadCount;
        pthread_create(&threadHandles[thread], NULL, ThrowDarts, (void *)t_args);
    }

    // Synchronize threads
    for (thread = 0; thread < threadCount; thread++)
    {
        pthread_join(threadHandles[thread], NULL);
    }

    // Find dart distances from origin
    for (thread = 0; thread < threadCount; thread++)
    {
        struct threadArgs *t_args = malloc(sizeof(struct threadArgs));
        t_args->myRank = thread;
        t_args->localN = numThrows / threadCount;
        pthread_create(&threadHandles[thread], NULL, FindDistance, (void *)t_args);
    }

    // Synchronize threads
    for (thread = 0; thread < threadCount; thread++)
    {
        pthread_join(threadHandles[thread], NULL);
    }

    // Count total hits in circle
    for (thread = 0; thread < threadCount; thread++)
    {
        struct threadArgs *t_args = malloc(sizeof(struct threadArgs));
        t_args->myRank = thread;
        t_args->localN = numThrows / threadCount;
        pthread_create(&threadHandles[thread], NULL, FindHits, (void *)t_args);
    }

    // Synchronize threads
    for (thread = 0; thread < threadCount; thread++)
    {
        pthread_join(threadHandles[thread], NULL);
    }

    gettimeofday(&end, NULL);
    stopwatch = ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec));

    double piEstimate = 4.0*hits/(double)numThrows;
    printf("estimate of pi is: %.17lf\n", piEstimate);
    printf("Execution time is: %lf usec", stopwatch);



    free(threadHandles);
    pthread_exit(NULL);
    return 0;
}

/*****************************************
 * Function Name: ThrowDarts
 *
 * Parameters: void* args
 *
 * Description: Casts darts onto a 2x2 square
 *
 * Returns: NULL
 ******************************************/
void *ThrowDarts(void *args)
{
    
    struct threadArgs *t_args = (struct threadArgs *)args;
    double start = t_args->localN * t_args->myRank;
    double end = start + t_args->localN;
    

    for (long long int toss = start; toss < end; toss++){
     
        double x = -1 + ((double)rand()/(double)RAND_MAX)*2;
        double y = -1 + ((double)rand()/(double)RAND_MAX)*2;
        xThrows[toss] = x;
        yThrows[toss] = y;
    }

    return NULL;
} /* ThrowDarts */

/*****************************************
 * Function Name: FindDistance
 *
 * Parameters: void* args
 *
 * Description: Calculates distance from origin
 *                  of the 2x2 board
 *
 * Returns: NULL
 ******************************************/
void* FindDistance(void* args)
{
    struct threadArgs *t_args = (struct threadArgs *)args;
    double start = t_args->localN * t_args->myRank;
    double end = start + t_args->localN;
    for(long long int i = start; i < end; i++){
        distance[i] = xThrows[i] * xThrows[i] + yThrows[i] * yThrows[i];
    }
    return NULL;
}

/*****************************************
 * Function Name: FindHits
 *
 * Parameters: void* args
 *
 * Description: Calculates how many darts 
 *                  landed in the circle
 *
 * Returns: NULL
 ******************************************/
void* FindHits(void* args)
{
    struct threadArgs *t_args = (struct threadArgs *)args;
    double start = t_args->localN * t_args->myRank;
    double end = start + t_args->localN;
    double localHits = 0;

    for (long long int i = start; i < end; i++){
        if(distance[i] <= 1){
            localHits++;
        }
    }

    pthread_mutex_lock(&mutex);
    hits += localHits;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

bool isPow2(long long int arguments){
    if (!(arguments & arguments-1)){
        return true;
    }
    else {
        return false;
    }

    printf("Error evaluating isPow2(). Returning false\n");
    printf("Please restart program\n\n");
    return false;
};
