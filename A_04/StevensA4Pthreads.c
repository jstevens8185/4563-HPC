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
 *  the default number of threads is 14.
 ****************************************************/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

const int N = 30000;
int a[N];
int b[N];
int c[N];
int d[N];
int e[N];
int f[N];
int dotProduct = 0;
int threadCount = 14;
int flag = 0;
pthread_mutex_t mutex;

void *DotProduct(void *args);
void *ComputeC(void *args);
void *ComputeF(void *args);

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
    int localN;
};

int main(int argc, char *argv[])
{
    // initialize mutex
    pthread_mutex_init(&mutex, NULL);

    // initialize values for integer arrays
    for (int i = 0; i < N; i++)
    {
        a[i] = 1;
        b[i] = 2;
        d[i] = 5;
        e[i] = 10;
    }

    // initialize summation variables
    int sumC = 0;
    int sumF = 0;

    // long for 64 bit systems
    long thread;

    pthread_t *threadHandles;

    // get # of threads from command line if any
    if (argv[1])
    {
        threadCount = strtol(argv[1], NULL, 10);
    }

    if (threadCount < 14)
    {
        printf("Needs at least 14 threads to function correctly\nSetting threadCount to 14\n\n");
    }

    threadHandles = malloc(threadCount * sizeof(pthread_t));

    // Compute C
    for (thread = 0; thread < 4; thread++)
    {
        struct threadArgs *t_args = malloc(sizeof(struct threadArgs));
        t_args->myRank = thread;
        t_args->localN = N / 4;
        pthread_create(&threadHandles[thread], NULL, ComputeC, (void *)t_args);
    }

    // Compute F
    for (thread = 4; thread < 8; thread++)
    {
        struct threadArgs *t_args = malloc(sizeof(struct threadArgs));
        t_args->myRank = thread - 4;
        t_args->localN = N / 4;
        pthread_create(&threadHandles[thread], NULL, ComputeF, (void *)t_args);
    }

    // Synchronize
    for (thread = 0; thread < threadCount; thread++)
    {
        pthread_join(threadHandles[thread], NULL);
    }

    // calculate sum of arrays c and f sequentially
    for (int i = 0; i < N; i++)
    {
        sumC += c[i];
        sumF += f[i];
    }

    printf("Summation of Elements in array c is equal to: %d\n", sumC);
    printf("Summation of Elements in array f is equal to: %d\n", sumF);

    // Compute DotProduct
    for (thread = 8; thread < 14; thread++)
    {
        struct threadArgs *t_args = malloc(sizeof(struct threadArgs));
        t_args->myRank = thread - 8;
        t_args->localN = N / 6;
        pthread_create(&threadHandles[thread], NULL, DotProduct, (void *)t_args);
    }

    // synchronize
    for (thread = 0; thread < threadCount; thread++)
    {
        pthread_join(threadHandles[thread], NULL);
    }

    printf("The Dot Product between c and f is: %d\n", dotProduct);

    pthread_mutex_destroy(&mutex);
    free(threadHandles);
    pthread_exit(NULL);
    return 0;
}

/*****************************************
 * Function Name: ComputeC
 *
 * Parameters: void* args
 *
 * Description: Computes the difference between two arrays, 'b' and 'a', and stores
 * the result in array 'c'. The computation is performed in block distribution
 * where each thread computes a segment of the array based on its rank and
 * the number of total segments.
 *
 * Returns: NULL
 ******************************************/
void *ComputeC(void *args)
{

    struct threadArgs *t_args = (struct threadArgs *)args;
    int start = t_args->localN * t_args->myRank;
    int end = start + t_args->localN;

    for (int i = start; i < end; i++)
    {
        c[i] = b[i] - a[i];
    }

    return NULL;
} /* ComputeC */

/*****************************************
 * Function Name: ComputeF
 *
 * Parameters: void* args
 *
 * Description: Computes the division of elements between two arrays, 'e' and 'd',
 * and stores the result in array 'f'. This computation is also performed in a block
 * distributed manner with each thread responsible for a segment of the array. The
 * division accounts for block distribution and the specific segment is determined
 * by the thread's rank.
 *
 * Returns: NULL
 ******************************************/
void *ComputeF(void *args)
{

    struct threadArgs *t_args = (struct threadArgs *)args;
    int start = t_args->localN * t_args->myRank;
    int end = start + t_args->localN;

    for (int i = start; i < end; i++)
    {
        f[i] = e[i] / d[i];
    }

    return NULL;
} /* ComputeF */

/*****************************************
 * Function Name: DotProduct
 *
 * Parameters: void* args
 *
 * Description: Calculates the dot product of corresponding
 * elements in arrays 'c' and 'f' over a segment determined
 * by the thread's rank. This function sums up the products
 * of the elements in the given segment and adds this local
 * sum to a global dot product using a mutex to ensure safety.
 *
 * Additional behavior: After computing the local dot product,
 * threads synchronize based on their rank to ensure orderly
 * output. Each thread prints its result once it is its
 * turn, as determined by the shared 'flag' variable.
 *
 * Returns: NULL
 ******************************************/
void *DotProduct(void *args)
{

    struct threadArgs *t_args = (struct threadArgs *)args;
    int start = t_args->localN * t_args->myRank;
    int end = start + t_args->localN;
    int localDotProduct = 0;
    for (int i = start; i < end; i++)
    {
        localDotProduct += c[i] * f[i];
    }
    // mutex for global dot product here
    pthread_mutex_lock(&mutex);
    dotProduct += localDotProduct;
    pthread_mutex_unlock(&mutex);

    // enforce order of rank here so that output looks nice
    while (flag != t_args->myRank)
        ;
    printf("I am thread #%ld and my local sum is: %d\n", t_args->myRank, localDotProduct);
    flag++;
    return NULL;
} /* DotProduct */