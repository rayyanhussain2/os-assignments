#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

sem_t mutex; //for filling up one of the queues
sem_t bridge;

int rightCount = 0;
int leftCount = 0;

void passing(_Bool isLeft, int id){
    if(isLeft){
        printf("Car ID: %d passing from the left...\n", id);
        sleep(1);
        printf("Car ID: %d passed from the left!\n", id);
    }else{
        printf("Car ID: %d passing from the right...\n", id);
        sleep(1);
        printf("Car ID: %d passed from the right!\n", id);
    }
}

void* right(void* args){
    //Maybe put mutual exclusion around accessing the bridge and increment counter - Getting deadlocks
    //A major flaw is that this is not sequential. So we are allowing all the left ones to pass then all the right ones
    //A situation could be if there were cars on the bridge which were left and there comes a right and a left.
    //the idea here is to let the 5 cars on the bridge pass, then the right one, then the left one.
    //but then who enters the queue? the right or the left
    //If we allow the right one to enter then every time we wake it up, we will have to put it back to sleep
    //which would again go back to the queue
    //and if we detect its a left one and wake that one up we just allow it
    //This is the same logic used here
    //If we dont put anything waiting and put both of them into the while loop, then theres no point of semaphores.
    while (leftCount != 0){
    }

    sem_wait(&bridge);      
    rightCount += 1; 

    //cannot put mutex around passing because 5 cars can pass at once. Defeats purpose
    passing(0, *(int*) args);

    sem_post(&bridge);
    rightCount -= 1;
}

void* left(void* args){
    while (rightCount != 0) {
    }

    sem_wait(&bridge);
    leftCount += 1;

    passing(1, *(int*) args);
    
    sem_post(&bridge);
    leftCount -= 1;
}



int main(){
    sem_init(&mutex, 0, 1);
    sem_init(&bridge, 0, 5);

    int leftNum, rightNum;
    printf("Enter the number of cars from left: ");
    scanf("%i", &leftNum);

    printf("Enter the number of cars from right: ");
    scanf("%i", &rightNum);

    int carID[leftNum + rightNum];
    //creating threads
    pthread_t threadsArrayLeft[leftNum];
    for(int i = 0; i < leftNum; i++){
        carID[i] = i;
        pthread_create(&threadsArrayLeft[i], NULL, left, (void*) &carID[i]);
    }
    
    pthread_t threadsArrayRight[rightNum];
    for(int i = 0; i < rightNum; i++){
        carID[leftNum + i] = leftNum + i;
        pthread_create(&threadsArrayRight[i], NULL, right, (void*) &carID[i + leftNum]);
    }

    //joining threads
    for(int i = 0; i < leftNum; i++){
        pthread_join(threadsArrayLeft[i], NULL);
    }
    
    for(int i = 0; i < rightNum; i++){
        pthread_join(threadsArrayRight[i], NULL);
    }

    sem_destroy(&bridge);
    sem_destroy(&mutex);

    return 0;
}