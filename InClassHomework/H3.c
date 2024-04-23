//////////////////////////////////////
//  Jered Stevens 
//
//  To compile run the following command:
//  mpicc -o ICH H3.c
//
//  Then run the following command to execute, replace * with # of processes:
//  mpi run -n * ICH
//////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(){

    int commSz;
    int myRank;

    // Start up MPI
    MPI_Init(NULL, NULL);

    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &commSz);

    // Get my rank among all the processes
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    int receive;
    int send;
    send = myRank*111;

    if(myRank == 0){
        send = 191;
        MPI_Send(&send , 1 , MPI_INT, 1 , 0 , MPI_COMM_WORLD);
    }

    for (int i = 1; i < commSz-1; i++){
        if(myRank == i){
            MPI_Recv(&receive, 1, MPI_INT, (myRank-1) , 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            send = send + receive;
            MPI_Send(&send , 1 , MPI_INT , (myRank+1) , 0 , MPI_COMM_WORLD);
        }
    }
    
    //}
    if (myRank == commSz-1){
        MPI_Recv(&receive , 1 , MPI_INT , (myRank-1) , 0 , MPI_COMM_WORLD , MPI_STATUS_IGNORE);
        send = send + receive;
        MPI_Send(&send , 1 , MPI_INT , 0 , 0 , MPI_COMM_WORLD);
    }
    if(myRank == 0){
        MPI_Recv(&receive , 1 , MPI_INT , commSz-1 , 0 , MPI_COMM_WORLD , MPI_STATUS_IGNORE);
        printf("final number is: %d\n", receive);
    }

    MPI_Finalize();

}