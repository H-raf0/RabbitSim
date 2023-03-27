#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mt19937ar.h"

#define TIME_STEP 1 //1 month, modifing it may result in erorrs


int fibonnaci(int N) {

    if (N == 0) {
        return 2;
    }
    else if (N == 1) {
        return 4;
    }
    else {
        return fibonnaci(N - 1) + fibonnaci(N - 2);
    }
}

typedef struct Rabbit {
    char sex;            // M: Male, F: Female
    int status;           // 0:dead ou 1:alive
    int age;              // 0:just born, -1 dead? (age in months)
    int mature;           // 0:no, 1:yes(adult)
    int pregnant;         // 0:no, 1:yes
    int nbLittersY;       // kittens must have in a year
    int nbLitters;        // kittens made so far
    unsigned int srvRate; // survive rate <= 100%
    Srabbit* nextRabbit;  // next rabbit

}Srabbit;


//used to update age every time step
int updateStats(Srabbit* rabbit) {
    int rabbitAge = rabbit->age;

    rabbit->age += TIME_STEP;

    //update maturaty
    if (rabbitAge >= 5 && rabbitAge<=8) {

        float randNB = genrand_real1() * 100;
        if (randNB <= (rabbitAge - 4) * 25) {

            rabbit->mature = 1;
            rabbit->srvRate = 60;
        }
    }
    else if (rabbit->age % 12 == 0) {

        generateAvgLitters(rabbit);
    }
    if (rabbit->status != 0) { //not dead, probably this line is useless
        if ((rabbit->age / 12) >= 11) {  //is 11 years old or more

            rabbit->srvRate = (rabbit->srvRate >= 10) ? (rabbit->srvRate - 10) : 0;
            
            if (rabbit->srvRate == 0) rabbit->status = 0;
        }
    }
    return rabbit->status;
}

void generateAvgLitters(Srabbit* rabbit) {
    float randNB = genrand_real1();
    if (randNB <= 40) rabbit->nbLittersY = 6;                      //           6:60%              
    else if (randNB <= 60) rabbit->nbLittersY = 5;                 //       5:20%   7:20%
    else if (randNB <= 80) rabbit->nbLittersY = 7;                 //   4:10%           8:10%
    else if (randNB <= 90) rabbit->nbLittersY = 4;
    else if (randNB <= 100) rabbit->nbLittersY = 8;
}

int giveBirth(Srabbit*  rabbit) {
    int prctg = 25;

    if (rabbit->sex == 'F' && rabbit->mature == 1) { //a mature not dead female rabbit

        if (rabbit->nbLitters < rabbit->nbLittersY && rabbit->pregnant == 0) { // did less then her avg and not pregnant

            rabbit->nbLitters += 1;
            if (rabbit->pregnant == 1) { // if pregnant
                float randNB = genrand_real1() * 100;
                while (1) {
                    // return nb of kittens [3,6]
                    if (randNB <= prctg) {
                        rabbit->pregnant = 0;
                        return 2 + prctg / 25;
                    }
                    else {
                        prctg += 25;
                    }
                }
            }
            rabbit->pregnant = 1;
        }
    }
}

char generate_sex() {
    return (genrand_real1() * 100 <= 50) ? 'M' : 'F';
}

Srabbit* createRabbitsList(int nb, Srabbit** head) {
    
    Srabbit *rabbit = malloc(sizeof(Srabbit));
    Srabbit *nextRabbit = rabbit;

    *rabbit = (Srabbit){ generate_sex(), 1, 0, 0, 0, 0, 35, NULL};
    for (int i = 0; i < nb - 1; i++) {

        nextRabbit = nextRabbit->nextRabbit;
        nextRabbit = &(Srabbit) { generate_sex(), 1, 0, 0, 0, 0, 35, NULL };
    }

    (*head)->nextRabbit = rabbit; // first one in the list
    return nextRabbit; // last one created
}

void sim(int N) {  //number of months
    Srabbit* rabbit = malloc(sizeof(Srabbit));
    Srabbit* currentRabbit = malloc(sizeof(Srabbit));
    Srabbit* tempRabbit = malloc(sizeof(Srabbit));
    Srabbit* headListRabbit = malloc(sizeof(Srabbit));
    int population = 2;
    int addedPopulation = 0;
    int newKittens;

    //first 2 rubbits
    *rabbit = (Srabbit){ 'M', 1, 0, 0, 0, 0, 35, &(Srabbit) { 'F', 1, 0, 0, 0, 0, 35, NULL } };
    //to save the adress of 'rabbit' since it is the first rabbit ever created
    currentRabbit = rabbit;


    //life cycle
    for (int i = 0; i < N; i += TIME_STEP) {

        //temporary rabbit used to connect new generations to the old ones
        *tempRabbit = (Srabbit){ '?', 0, 0, 0, 0, 0, 0, NULL };
        //head of the new generation linked list
        headListRabbit = tempRabbit;
        //run of all the existing rabbits
        for (int j = 0; j < population; j++) {
            if (currentRabbit->status != 0) {  // if not already dead

                if (updateStats(currentRabbit)) continue;

                addedPopulation += (newKittens = giveBirth(currentRabbit));
                tempRabbit = createRabbitsList(newKittens, tempRabbit);

                if(j != population - 1) currentRabbit = currentRabbit->nextRabbit; //to avoid getting last pointer which is equal to NULL
            }
        }
        currentRabbit->nextRabbit = headListRabbit->nextRabbit; // linking the new generation to the old one
        population += addedPopulation; // adding the new rabbits
    }

}



int main()
{
    int gen;
    printf("Number of generations :");scanf("%d\n", &gen);
    //printf(fibonacci(gen));
    return 0;
}


