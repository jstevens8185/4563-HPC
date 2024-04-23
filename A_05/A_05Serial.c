/****************************************************
 * Name:    Jered Stevens
 * File:    A_05.c
 * 
 * This program estimates PI with the Monte Carlo method.
 *  This method is done by "drawing" a radius 1
 *  circle inside of a 2x2 square. The resulting ratio
 *  of circle to square is PI/4. Next, the program 
 *  randomly casts a very large number of "darts" at 
 *  the square and circle and calculates how many darts
 *  land in the circle. Then the program divides the
 *  number that landed in the circle by the total number
 *  of darts thrown, giving us an approximation of 
 *  PI/4. That number is then multiplied by 4, resulting
 *  in a very close approximation of PI.
 *
 * To compile
 *  - Navigate in terminal to destination folder
 *  - Run the following command
 *      gcc -g -Wall -o A_05 A_05.c
 *
 * - Run the following command
 *      ./A_05
 ****************************************************/

#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

///////////////////////////////////////////////////////
//  const long long int numThrows
//
//      Number of darts to be thrown. Does not have to 
//      be a power of 2 for serial implementation.
//      Larger numbers result in more accurate results.
///////////////////////////////////////////////////////

#define numThrows 33554432 // 2^25

int main(int argc, char *argv[])
{
    srand(time(NULL));
    long long int numInCircle = 0;
    struct timeval start, end;
    double stopwatch;
    gettimeofday(&start, NULL);
    for (long long int toss = 0; toss < numThrows; toss++){
     
    
        double x = -1 + ((double)rand()/(double)RAND_MAX)*2;
        double y = -1 + ((double)rand()/(double)RAND_MAX)*2;
        double distanceSquared = x*x + y*y;
        if (distanceSquared <= 1)
        {
            numInCircle++;
        }
    }
    gettimeofday(&end, NULL);
    stopwatch = ((end.tv_sec*1000000 + end.tv_usec) - (start.tv_sec*1000000 + start.tv_usec));


    double piEstimate = 4.0*numInCircle/numThrows;
    printf("estimate of pi is: %.17lf\n", piEstimate);
    printf("Execution time is: %lf usec", stopwatch);
}
