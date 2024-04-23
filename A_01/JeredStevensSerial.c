// Jered Stevens
// Compile by typing the following or copying and pasting into terminal:
// gcc -o JeredStevensSerial.exe JeredStevensSerial.c
//
// Run by typing or copy and pasting the following:
// sbatch  JeredStevensSerialScript


#include <time.h>
#include<sys/time.h>
#include <stdio.h>

const int n = 640000;

int main(){
    /* Initializing Variables */
    struct timeval st, et;
    long int total = 0;  
    long int gauss = (0.5)*(n)*(n+1);  
    int a[n];

    /* Filling array with numbers */
    for (int i=1; i <= n; i++){
        a[i-1] = i;
    };

    /* Summation of array
     * Timed Portion   */
    gettimeofday(&st,NULL);
    
    for (int i = 0; i < n; i++){
        total += a[i];
    };

    /* End Timer */
    gettimeofday(&et,NULL);

    int elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);
    printf("Sorting time: %d micro seconds\n",elapsed);
    printf("The total is %ld\n", total);

    return 0;
}

