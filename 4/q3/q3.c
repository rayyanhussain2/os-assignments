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

void passing(_Bool isLeft){
    if(isLeft){
        printf("Car ID: %lld passing from the left...\n", (unsigned long long)pthread_self());
        sleep(1);
        printf("Car ID: %lld passed from the left!\n", (unsigned long long)pthread_self());
    }else{
        printf("Car ID: %lld passing from the right...\n", (unsigned long long)pthread_self());
        sleep(1);
        printf("Car ID: %lld passed from the right!\n", (unsigned long long)pthread_self());
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
    sem_wait(&bridge);      1

    passing(0);

    rightCount -= 1;
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
    sem_wait(&mutex1);
    leftCount += 1;
    if(rightCount > 0){
        sem_post(&mutex1);
        sem_wait(&leftQ);
    }else{
        sem_post(&mutex1);
    }

    //Let it take on the bridge
    sem_wait(&bridge);      

    passing(1);

    leftCount -= 1;
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
        pthread_create(&threadsArrayLeft[i], NULL, left, NULL);
    }
    
    pthread_t threadsArrayRight[rightNum];
    for(int i = 0; i < rightNum; i++){
        carID[leftNum + i] = leftNum + i;
        pthread_create(&threadsArrayRight[i], NULL, right, NULL);
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

    return 0;
}