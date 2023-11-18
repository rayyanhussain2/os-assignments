#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

sem_t mutex1; //for filling up one of the queues
sem_t mutex2;
sem_t bridge;
sem_t leftQ;
sem_t rightQ;

int rightCount = 0;
int leftCount = 0;

void passing(_Bool isLeft, int carID){
    if(isLeft){
        printf("Car ID: %d started crossing the bridge from the left...\n", carID);
        sleep(1);
        printf("Car ID: %d finished crossing the bridge from the left!\n", carID);
    }else{
        printf("Car ID: %d started crossing the bridge from the right...\n", carID);
        sleep(1);
        printf("Car ID: %d finished crossing the bridge from the right!\n", carID);
    }
}  

//implement mutual exclusion
void* right(void* args){
    //If there are cars on the bridge in the opposite direction, put to sleep
    //Count is how many cars are there total in queue and on bridge
    sem_wait(&mutex1);
    rightCount += 1;
    if(leftCount > 0){
        sem_post(&mutex1);
        sem_wait(&rightQ);
    }else{
        sem_post(&mutex1);
    }

    //Let it take on the bridge
    sem_wait(&bridge);      

    passing(0, *(int*) args);

    sem_wait(&mutex2);
    rightCount -= 1;
    sem_post(&mutex2);
    sem_post(&bridge);

    //if there are cars left in the queue then you wake them up or else you just leave it as it is
    if(leftCount > 0 && rightCount == 0){
        for(int i = 0; i < leftCount; i++){
            sem_post(&leftQ); // waking up all of them
        }
    }
}

void* left(void* args){
    //If there are cars on the bridge in the opposite direction, put to sleep
    //Count is h   ow many cars are there total in queue and on bridge
    sem_wait(&mutex2);
    leftCount += 1;
    if(rightCount > 0){
        sem_post(&mutex2);
        sem_wait(&leftQ);
    }else{
        sem_post(&mutex2);
    }

    //Let it take on the bridge
    sem_wait(&bridge);      

    passing(1, *(int*) args);

    sem_wait(&mutex2);
    leftCount -= 1;
    sem_post(&mutex2);
    sem_post(&bridge);

    //if there are cars left in the queue then you wake them up or else you just leave it as it is
    if(rightCount > 0 && leftCount == 0){
        for(int i = 0; i < rightCount; i++){
            sem_post(&rightQ); // waking up all of them  //no need for locks as we are waking all of them
        }
    }

}

int main(){
    sem_init(&mutex1, 0, 1);
    sem_init(&mutex2, 0, 1);
    sem_init(&bridge, 0, 5);
    sem_init(&leftQ, 0, 0); // queues for left
    sem_init(&rightQ, 0, 0); // queues for right


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
        pthread_create(&threadsArrayRight[i], NULL, right, (void*) &carID[leftNum + i]);
    }

    //joining threads
    for(int i = 0; i < leftNum; i++){
        pthread_join(threadsArrayLeft[i], NULL);
    }
    
    for(int i = 0; i < rightNum; i++){
        pthread_join(threadsArrayRight[i], NULL);
    }

    sem_destroy(&bridge);
    sem_destroy(&mutex1);
    sem_destroy(&mutex2);
    sem_destroy(&leftQ);
    sem_destroy(&rightQ);

    return 0;
}