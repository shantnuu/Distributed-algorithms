/* MPI Program Template */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <stdlib.h>
#include "mpi.h"

using namespace std;

void quickSort(int*, int);

int main( int argc, char **argv ) {
    int rank, numprocs;
    
    std::fstream inp_file(argv[1], std::ios_base::in);
    std::ofstream output_file;
    int n,a;
    n=0;
    int* arr = (int*)malloc(100005*sizeof(int));
    while(inp_file>>a){
        arr[n++] = a;
    }

    int *rbuf = (int *)malloc(n*sizeof(int));

    /* start up MPI */
    MPI_Init( &argc, &argv );

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
    
    /*synchronize all processes*/
    MPI_Barrier( MPI_COMM_WORLD );
    double tbeg = MPI_Wtime();

    /* write your code here */

    /********Dividing the array in equal parts*********/
    int child_size, rem;
    child_size = n/numprocs;
    rem = n % numprocs;
    /* Send each subarray to each processor */
    int *sub_arr = (int*)malloc(child_size * sizeof(int));
    int *send_counts = (int*)malloc(numprocs * sizeof(int));
    int *send_start_arr = (int*)malloc(numprocs * sizeof(int));

    for (int i = 0; i < numprocs; ++i)
    {
        if(rem > 0){
            send_counts[i] = child_size+1;
        }
        else{
            send_counts[i] = child_size;
        }
        
        rem--;
    }
    send_start_arr[0] = 0;
    for (int i = 1; i < numprocs; ++i){
        send_start_arr[i] = send_counts[i - 1] + send_start_arr[i-1];
    }
    
    MPI_Scatterv(arr, send_counts, send_start_arr, MPI_INT, sub_arr, child_size + 1, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < send_counts[rank]; ++i)
    {
        cout<<sub_arr[i]<<" ";
    }
    cout<<endl;

    /* QuickSort on each subarray */
    quickSort(sub_arr, child_size);

    /* Wait for the synchronization of all processes. */
    MPI_Barrier( MPI_COMM_WORLD );

    /* Merge the p sorted subarrays */
    MPI_Gatherv(sub_arr, send_counts[rank], MPI_INT, rbuf, send_counts, send_start_arr, MPI_INT, 0, MPI_COMM_WORLD);

    /* Combining the k sorted arrays into a single sorted array using min heap */
    if(rank == 0){
        make_heap(rbuf, rbuf+n);
        sort_heap(rbuf, rbuf+n);
    }

    MPI_Barrier( MPI_COMM_WORLD );
    double elapsedTime = MPI_Wtime() - tbeg;
    double maxTime;
    MPI_Reduce( &elapsedTime, &maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD );
    if ( rank == 0 ) {
        printf( "Total time (s): %f\n", maxTime );
    }

    /* shut down MPI */
    MPI_Finalize();

    if(rank==0){
        output_file.open(argv[2], std::ios::app);
        for (int i = 0; i < n; ++i)
        {
            output_file<<rbuf[i]<<" ";
        }
        output_file.close();
    }

    return 0;
}

void quickSort(int* original_array, int size){

    if(size <= 1){
        return;
    }

    int pivot = 0;
    int *L = (int*)malloc(size * sizeof(int));
    int *R = (int*)malloc(size * sizeof(int));
    int l = 0;
    int r = 0;
    for (int i = 1; i < size; ++i)
    {
        if(original_array[i] <= original_array[pivot]){
            L[l] = original_array[i];
            l++;
        }
        else{
            R[r] = original_array[i];
            r++;
        }
    }
    original_array[l] = original_array[pivot];
    quickSort(L, l);
    quickSort(R, r);
    for (int i = 0; i < l; ++i)
    {
        original_array[i] = L[i];
    }
    for (int i = l+1; i < l+r+1; ++i)
    {
        original_array[i] = R[i-l-1];
    }
}










