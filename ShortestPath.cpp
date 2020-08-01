/* MPI Program Template */

#include <stdio.h>
#include <bits/stdc++.h> 
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include "mpi.h"

using namespace std;

void bellman(int* start, int* end, int* weight, int* distances, int size);

int main( int argc, char **argv ) {
    int rank, numprocs;
    MPI_Status stat;

    std::fstream inp_file(argv[1], std::ios_base::in);
    std::ofstream output_file;

    int node_count, edge_count;

    inp_file>>node_count;
    inp_file>>edge_count;

    int* start = (int*)malloc(edge_count*sizeof(int));
    int* end = (int*)malloc(edge_count*sizeof(int));
    int* weight = (int*)malloc(edge_count*sizeof(int));
    int* distances = (int*)malloc(node_count*sizeof(int));
    int source_node;

    for (int i = 0; i < edge_count; ++i)
    {
        inp_file>>start[i];
        inp_file>>end[i];
        inp_file>>weight[i];
    }

    inp_file>>source_node;
    source_node -= 1;

    for (int i = 0; i < node_count; ++i)
    {
        distances[i] = 100000;
    }

    distances[source_node] = 0;

    /* start up MPI */
    MPI_Init( &argc, &argv );

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
    
    /*synchronize all processes*/
    MPI_Barrier( MPI_COMM_WORLD );
    double tbeg = MPI_Wtime();

    /* write your code here */
    int child_size, rem;
    child_size = edge_count/numprocs;
    rem = edge_count % numprocs;

    
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

    for (int i = 0; i < node_count; ++i)
    {
        if(rank == 0){
            for (int j = 0; j < numprocs; ++j)
            {
                MPI_Send(&distances[0], node_count, MPI_INT, j, 1001, MPI_COMM_WORLD);
            }
        }
        MPI_Recv(distances, node_count, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
        bellman(&start[send_start_arr[rank]], &end[send_start_arr[rank]], &weight[send_start_arr[rank]], &distances[0], send_counts[rank]);
        MPI_Send(&distances[0], node_count, MPI_INT, 0, 1001, MPI_COMM_WORLD);

        if(rank==0){

            int* temp_dist = (int*)malloc(node_count*sizeof(int));
            
            for (int k = 1; k < numprocs; ++k)
            {
                MPI_Recv(temp_dist, node_count, MPI_INT, k, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
                for (int j = 0; j < node_count; ++j)
                {
                    if(temp_dist[j] < distances[j])
                        distances[j] = temp_dist[j];
                }
            }
        }
    }

    if(rank == 0){
        output_file.open(argv[2], std::ios::app);
        for (int i = 0; i < node_count; ++i)
        {
            output_file<<i+1<<" "<<distances[i]<<endl;
        }
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
    return 0;
}

void bellman(int* start, int* end, int* weight, int* distances, int size){

    for (int i = 0; i < size; ++i)
    {
        if(distances[start[i]] != 100000 && distances[start[i]] + weight[i] < distances[end[i]]){
            distances[end[i]] = distances[start[i]] + weight[i];
        }
        
        if(distances[end[i]] != 100000 && distances[end[i]] + weight[i] < distances[start[i]]){
            distances[start[i]] = distances[end[i]] + weight[i];
        }
    }
}