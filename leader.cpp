// REFERENCE: https://github.com/bethirahul/RPC_and_Chang-Robert-Leader-Election-Algorithm/blob/master/election.c

#include "mpi.h"
#include <iostream>
#include <time.h>

using namespace std;

// CREDIT: https://stackoverflow.com/questions/11720656/modulo-operation-with-negative-numbers
int modulo_Euclidean(int a, int b) {
  int m = a % b;
  if (m < 0) {
    // m += (b < 0) ? -b : b; // avoid this form: it is UB when b == INT_MIN
    m = (b < 0) ? m - b : m + b;
  }
  return m;
}

int main(int argc, char *argv[]){
    int rank, size, initiator;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    sscanf(argv[1], "%d", &initiator);
    if (initiator >= size  || initiator < 0){
        if (rank == 0){
            printf("Invalid num. Initiator must be from 0 to %d.\n", size-1);
        }
        MPI_Finalize();
        return 0;
    }

    srand(time(NULL) + rank);
    int ID, message;
    bool state = 0, leader = 0;

    MPI_Barrier(MPI_COMM_WORLD);
    ID = rand() % 100000000000000;
    printf("Rank: %d, ID: %d \n", rank, ID);
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == initiator){
        printf("Initiator: %d\n", initiator);
        message = ID;
        MPI_Send(&message, 1, MPI_INT, (rank+1) % size, 0, MPI_COMM_WORLD);
        printf("Process %d sent message %d to process %d.\n", rank, message, (rank+1) % size);
    }

    for (int i = 0; i < 2; i++){
        MPI_Recv(&message, 1, MPI_INT, modulo_Euclidean((rank-1), size), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received message %d from process %d.\n", rank, message, modulo_Euclidean((rank-1), size));
        
        if (message == ID){
            leader = 1;
        } else if (message > ID){
            state = 0;
        } else {
            state = 1;
            message = ID;
        }

        MPI_Send(&message, 1, MPI_INT, (rank+1) % size, 0, MPI_COMM_WORLD);
        printf("Process %d sent message %d to process %d.\n", rank, message, (rank+1) % size);

    }
    
    if (leader){
        printf("Rank %d is leader!\n", rank);
    }

    // MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}
