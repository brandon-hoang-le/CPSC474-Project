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
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(time(NULL) + rank);
    int ID, message, trips = 0;
    bool state = 0, leader = 0, running = 1;

    MPI_Barrier(MPI_COMM_WORLD);
    ID = rand() % 10000000000000;
    printf("Rank: %d, ID: %d \n", rank, ID);
    MPI_Barrier(MPI_COMM_WORLD);

    if (rank == 0){
        message = ID;
        MPI_Send(&message, 1, MPI_INT, (rank+1) % size, 0, MPI_COMM_WORLD);
        printf("Process %d sent message %d to process %d.\n", rank, message, (rank+1) % size);
    }

    for (int i = 0; i < 2; i++){
        MPI_Recv(&message, 1, MPI_INT, (rank-1) % size, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if (leader || message == -1)
            printf("Process %d received dummy message %d from process %d.\n", rank, message, modulo_Euclidean(rank-1, size));
        else
            printf("Process %d received message %d from process %d.\n", rank, message, modulo_Euclidean(rank-1, size));

        if (message > ID){
            state = 0;
        } else if (message == ID){
            leader = 1;
        } else {
            state = 1;
            if (message == -1)
                ID = message;
            else
                message = ID;
        }

        if (leader || message == -1) {
            message = -1;
            MPI_Send(&message, 1, MPI_INT, (rank+1) % size, 0, MPI_COMM_WORLD);
            printf("Process %d sent dummy message %d to process %d.\n", rank, message, (rank+1) % size);
        }
        else{
            MPI_Send(&message, 1, MPI_INT, (rank+1) % size, 0, MPI_COMM_WORLD);
            printf("Process %d sent message %d to process %d.\n", rank, message, (rank+1) % size);
        }
    }
    
    if (leader){
        printf("Rank %d is leader!\n", rank);
    }

    // MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}