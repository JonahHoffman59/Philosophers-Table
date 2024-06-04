//Names: Kieran Gilpin, Jonah Hoffman
//Class: CSC 345
//Date: 04/20/24

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int static numOfPhils; // number of phils, passed in as argument[1]
int static numOfTimesToEat; // number of times to eat each, passed in as argument[2]
sem_t *chopsticks; //( 1 = chop is not there, 0 = chop is there)
int *state; //the state of the philosopher (0=hungry, 1=eating, 2=thinking)
int static *phils; //philosopher 0, 1, 2, ..., (numOfPhils-1)
char poss_states[3][10] = {"Hungry", "Eating", "Thinking"};
int *eatcount;
void pickupChopstick(int phils);


// Even Philosopher grabs right chopstick first, Odd Philosopher grabs left chopstick first


// used to check state of philsopher and state of each chopstick
// if philospher is hungry and both left and right are satisifed
// then they should be able to eat now
void test(int phils){

    int getLeft = (phils + numOfPhils - 1) % numOfPhils; // automatically figures out left phil (even if 0)
    int getRight = (phils + 1) % numOfPhils; // automatically figures out right phil (even if 0)

    if(phils%2==0){

        if(state[phils] == 0){ 

                if(state[getLeft] != 1){
                    printf("Philosopher %d is Eating\n", phils);
                    state[phils] = 1;
                    //printf("Line 39 - Even \n");
                    //sem_wait(&chopsticks[getLeft]);
                    eatcount[phils]--;
                    sem_post(&chopsticks[getLeft]);
                    sem_post(&chopsticks[getRight]);
                    test(getLeft);
                    test(getRight);
                }
        }
            
    }    
        
    else if(phils%2==1){

            if(state[phils] == 0){

                if(state[getRight]!=1){
                        printf("Philosopher %d is Eating\n", phils);
                        state[phils] = 1;
                        eatcount[phils]--;
                        sem_post(&chopsticks[getLeft]);
                        sem_post(&chopsticks[getRight]);
                        test(getLeft);
                        test(getRight);
                        
                    }
            }
        
    }    
    sleep(1);
   
}
 


void pickupChopstick(int phils){
    //philosopher becomes hungry on attempted pickup (will stay until eating)
    //sem_wait to test chopsticks availability
    state[phils] = 0;
    printf("Philosopher %d is Hungry\n", phils);
    sleep(1);
    int getLeft = (phils + numOfPhils - 1) % numOfPhils; // automatically figures out left phil (even if 0)
    int getRight = (phils + 1) % numOfPhils; // automatically figures out right phil (even if 0)

    if(phils%2==0){
        test(phils);
        sem_wait(&chopsticks[getLeft]);
        sem_wait(&chopsticks[getRight]);

    }
    else if(phils%2==1){
        test(phils);
        sem_wait(&chopsticks[getRight]);
        sem_wait(&chopsticks[getLeft]);
    }
    sleep(1);
}
   




 // puts chopsticks back down (denotes when philospher is thinking)
void putDownChopstick(int phils){
    //sem_post chopstick
    //test on right and left
    if(state[phils] == 1){
    int getLeft = (phils + numOfPhils - 1) % numOfPhils; // automatically figures out left phil (even if 0)
    int getRight = (phils + 1) % numOfPhils; // automatically figures out right phil (even if 0)
    printf("Philosopher %d is Thinking\n", phils);
    state[phils] = 2;

    test(getLeft);
    test(getRight);
        
    sleep(1);
    }
    else{
        printf("Philosopher %d is Hungry\n", phils);
        sleep(1);
    }
    
        
    
        
        
}

void *philosopher(void *philnum){
    printf("E");
    int id = *((int*)philnum);
    for(int i = 0; i < numOfTimesToEat; i++){
            sleep(1);
            //printf("Line 206 - %d\n", id);
            pickupChopstick(id);
            //printf("Line 208 - %d\n", id);
            sleep(1);
            putDownChopstick(id);
            if(eatcount[id] == 0){
                pthread_exit(philnum);
            }
            
    }
    
}

 // must be a pointer when working with threading
                     // determines first action of a philospher when thread is created

int main(int argc, char *argv[]) {
    srand(time(NULL));
    
    numOfPhils = atoi(argv[1]);

    // memory allocation for chopsticks, state, and philosphers
    chopsticks = malloc(numOfPhils * sizeof(sem_t));
    state = malloc(numOfPhils * sizeof(int));
    phils = malloc(numOfPhils * sizeof(int));

    numOfTimesToEat = atoi(argv[2]);

    eatcount = malloc(numOfPhils*sizeof(int));

    for(int i = 0; i < (numOfPhils-1); i++){
        eatcount[i] = numOfTimesToEat;
    }

    int choices[] = {0,2};
    //initialize the state of every philosopher
    for(int i = 0; i < numOfPhils; i++){
        int z = rand()%2;
        phils[i] = i;
        state[i] = choices[z];
        printf("Philosopher %d is %s\n", i, poss_states[state[i]]);
        sleep(1);
    }

   
    // thread usage
    printf("Num of times to eat is %d, Num of philosophers is %d\n", numOfTimesToEat,numOfPhils);
    pthread_t threads[numOfPhils];
    

        for (int i = 0; i < numOfPhils; i++){
            sem_init(&chopsticks[i], 1, 0);

          
        }
        for (int i = 0; i < numOfPhils; i++){
            printf("line 256\n");
            pthread_create(&threads[i], NULL, philosopher, &phils[i]);
        }
        
        for(int i = 0; i < numOfPhils; i++){
            pthread_join(threads[i], NULL);
        }
    // for threads you will need to incorporate the concept of creating and
    // joining to solve this problem

    

    for(int i = 0; i < numOfPhils; i++){
    sem_destroy(&chopsticks[i]);
    }
    free(chopsticks);
    free(state);
    free(eatcount);
    free(phils);
    printf("Philosophers are done eating\n");
    return 0;
}

