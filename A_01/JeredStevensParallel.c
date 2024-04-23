// Jered Stevens
//
// Compile by typing or copying and pasting the following:
// mpicc -o JeredStevensParallel.exe JeredStevensParallel.c
//
// Run by typing or copying and pasting the following:
// mpicc -o JeredStevensParallel.exe JeredStevensParallel.c


#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<sys/time.h>
const int numberToSum = 640000;




int main(int argc, char *argv[])
{
    int commSz;
    int myRank;
    int elementsPerProcess;
    int numElementsReceived;
    struct timeval st, et;

    /* Start up MPI */
    MPI_Init(NULL, NULL); 

    /* Get the number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &commSz); 

    /* Get my rank among all the processes */
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank); 

    // Process 0 creates and populates array of 640000 ints from 1 - 640000
    if (myRank == 0){
        int* numbersArray;
        unsigned long int* summation;
        int index;
        elementsPerProcess = numberToSum / commSz;
        unsigned long int total = 0;

        // set all values in array to 0
        numbersArray = (int*)calloc(numberToSum, sizeof(int));
        summation = (unsigned long int*)calloc(commSz, sizeof(unsigned long));

        // Populate array with ints 1 - 640,000
        for (int i = 0; i < numberToSum; i++){
            numbersArray[i] = i+1;
        };


        // Start Timer
        gettimeofday(&st,NULL);

        // Process 0 sends portion of array to each mpi process
        for (int i = 1; i < commSz; i++){
            // Calculate starting point for each process
            index = i * elementsPerProcess;

            // Send # of elements of array to sum
            MPI_Send(&elementsPerProcess , 1 , MPI_INT , i , 0 , MPI_COMM_WORLD);
            // Send Address of array to be worked on
            MPI_Send(&numbersArray[index], elementsPerProcess, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
            // Process 0 sums its portion of array
            for(int i = 0; i < elementsPerProcess; i++){
                summation[0] += numbersArray[i];
            };
            
            unsigned long int temp;
            for (int i = 1; i < commSz; i++){
                // Receive local sum from each process and save into summation
                MPI_Recv(&temp, 1, MPI_UNSIGNED_LONG, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                summation[i] = temp;
            };
            // for(int i = 0; i < commSz; i++){
            //     printf("value at summation[%i] is: %ld", i, summation[i]);
            // };
            
            for(int i = 0; i < commSz; i++){
                total += summation[i];
            }

            // End Timer
            gettimeofday(&et,NULL);

            // Create variable for elapsed time in microSeconds
            int elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);
            
            // Print Results
            printf("Sorting time: %d micro seconds\n",elapsed);
            printf("The total is: %ld\n", total);
        // All other processes receive and sum their portion of array
        }else{
            // local array to store sent items
            int* localNumbersArray;

            // Receive number of elements to be worked on
            MPI_Recv(&numElementsReceived , 1 , MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // set all values in array to 0
            localNumbersArray = (int*)calloc(numElementsReceived, sizeof(int));

            // Receive portion of array to be processed
            MPI_Recv(localNumbersArray, numElementsReceived , MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // All other process sum their portion of array
            unsigned long int localSum = 0;
            for (int i = 0; i < numElementsReceived; i++){
                localSum += localNumbersArray[i];
            };

            // Other processes send their local sum to process 0
            MPI_Send(&localSum , 1, MPI_UNSIGNED_LONG, 0, 0, MPI_COMM_WORLD);
    };
        



    MPI_Finalize();
    return 0;

}