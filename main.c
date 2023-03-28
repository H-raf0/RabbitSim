#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mt19937ar.h"

#define TIME_STEP 1 //1 month, modifing it may result in errors

void check(void* pointer) {

    if (pointer == NULL) {
        printf("error creating pointer\n");
    }
}

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
    int mature;           // x0:no, x1:yes(adult) x1: at age x become mature 
    int pregnant;         // 0:no, 1:yes
    int nbLittersY;       // kittens must have in a year
    int nbLitters;        // kittens made so far
    int srvRate; // survive rate <= 100%, <0 means has been used, >=0 will be used (is positif when created ot updated)
    struct Rabbit* nextRabbit;  // next rabbit

}Srabbit;

void generateAvgLitters(Srabbit* rabbit) {
    float randNB = genrand_real1();
    if (randNB <= 40) rabbit->nbLittersY = 6;                      //           6:60%              
    else if (randNB <= 60) rabbit->nbLittersY = 5;                 //       5:20%   7:20%
    else if (randNB <= 80) rabbit->nbLittersY = 7;                 //   4:10%           8:10%
    else if (randNB <= 90) rabbit->nbLittersY = 4;
    else if (randNB <= 100) rabbit->nbLittersY = 8;
}

//used to update age every time step
void updateStats(Srabbit* rabbit) {

    rabbit->age += TIME_STEP;
    int rabbitAge = rabbit->age;
    int prctg = 25;

    //update maturaty
    if (rabbitAge >= 5 && rabbitAge<=8 && rabbit->mature == 0) {

        float randNB = genrand_real1() * 100;
        while (rabbit->mature == 0) {
            if (randNB <= prctg) {
                rabbit->mature = 1 + 10 * (rabbitAge - 1 + prctg/25);
                rabbit->srvRate = 60;
            }
            else{
                prctg += 25;
            }
        }
    }
    if (rabbit->mature > 0 && (rabbit->age % 12 == rabbit->mature / 10)) {

        rabbit->nbLitters = 0;
        generateAvgLitters(rabbit);
    }
    if (rabbit->status != 0) { //not dead, probably this line is useless
        if ((rabbit->age / 12) >= 11) {  //is 11 years old or more

            rabbit->srvRate = (abs(rabbit->srvRate) >= 10) ? (abs(rabbit->srvRate) - 10) : 0;
            
            //if (rabbit->srvRate == 0) rabbit->status = 0;
        }
    }
}

int giveBirth(Srabbit*  rabbit) {
    int prctg = 25;

    if (rabbit->sex == 'F' && rabbit->mature%10 == 1) { //a mature not dead female rabbit

        if (rabbit->nbLitters < rabbit->nbLittersY) { // did less then her avg and not pregnant

            
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
            else {
                rabbit->nbLitters += 1;
                rabbit->pregnant = 1;
            }
        }
    }
    return 0;
}

char generate_sex() {
    return (genrand_real1() * 100 <= 50) ? 'M' : 'F';
}

Srabbit* createRabbitsList(int nb, Srabbit** head) {
    if (nb != 0) {
        Srabbit* rabbit = (Srabbit*)malloc(sizeof(Srabbit));
        Srabbit* saveFirst = (Srabbit*)malloc(sizeof(Srabbit));

        *rabbit = (Srabbit){ generate_sex(), 1, 0, 0, 0, 0, 0, 35, NULL };
        saveFirst = rabbit;
        for (int i = 0; i < nb - 1; i++) {

            Srabbit* newRabbit = (Srabbit*) malloc(sizeof(Srabbit));
            *newRabbit = (Srabbit){ generate_sex(), 1, 0, 0, 0, 0, 0, 35, NULL };
            rabbit->nextRabbit = newRabbit;
            rabbit = rabbit->nextRabbit;
        }

        (*head)->nextRabbit = saveFirst; // first one in the list
        return rabbit; // last one created
    }
    return *head;
}

int sim(int startNB, int N) {  //number of months
    Srabbit* rabbit = (Srabbit*) malloc(sizeof(Srabbit));
    Srabbit* currentRabbit = (Srabbit*) malloc(sizeof(Srabbit));
    Srabbit* headListRabbit = (Srabbit*) malloc(sizeof(Srabbit));
    int population = startNB;
    int addedPopulation = 0;
    int newKittens;
    int deadRabbits = 0;
    float progress;

    //first 2 rubbits
    *rabbit = (Srabbit){ generate_sex(), 1, 0, 0, 0, 0, 0, 35, NULL};
    createRabbitsList(startNB-1, &rabbit);
    
    //life cycle
    for (int i = 0; i < N; i += TIME_STEP) {

        //to save the adress of 'rabbit' since it is the first rabbit ever created
        currentRabbit = rabbit;
        //temporary rabbit used to connect new generations to the old ones
        Srabbit* tempRabbit = (Srabbit*)malloc(sizeof(Srabbit));
        *tempRabbit = (Srabbit){ '?', 0, 0, 0, 0, 0, 0, 0, NULL };
        //head of the new generation linked list
        headListRabbit = tempRabbit;
        //run of all the existing rabbits

        progress = 0;
        printf("month %d / %d :\n", i, N);
        for (int j = 0; j < population; j++) {
            
            if (j % 10000 == 0 && j != 0) {

                // transforme i to a percentage and prints it
                progress = j * 100.0 / population;
                printf("progress : %.2f %%\r", progress);
            }

            /*
            printf("population : %d\nsex : %c, status : %d, age : %d, mature : %d, pregnant : %d, nbLittersY : %d, nbLitters : %d, srvRate : %d\n",
                population, currentRabbit->sex, currentRabbit->status, currentRabbit->age, currentRabbit->mature, currentRabbit->pregnant,
                currentRabbit->nbLittersY, currentRabbit->nbLitters, abs(currentRabbit->srvRate));
            */

            if (currentRabbit->status != 0) {  // if not already dead

                updateStats(currentRabbit);

                //if (currentRabbit->srvRate == 0) { currentRabbit->status = 0;  deadRabbits++; }

                if (currentRabbit->srvRate >= 0) {
                    if (genrand_real1() * 100 <= (100 - currentRabbit->srvRate)) {
                        currentRabbit->status = 0;
                        deadRabbits++;
                        population--;
                        continue;
                    }
                    currentRabbit->srvRate = currentRabbit->srvRate * -1;
                }

                addedPopulation += (newKittens = giveBirth(currentRabbit));

                tempRabbit = createRabbitsList(newKittens, &tempRabbit);
            }
            
            if (j != population - 1) { currentRabbit = currentRabbit->nextRabbit; } //to avoid getting last pointer which is equal to NULL

            
        }
        printf("    progress finished\n");
        system("cls");
        if (headListRabbit->nextRabbit != NULL) {
            //printf("break point: %p\n%p", currentRabbit, headListRabbit);
            currentRabbit->nextRabbit = headListRabbit->nextRabbit; // linking the new generation to the old one
        }
        population += addedPopulation; // adding the new rabbits
        addedPopulation = 0;
    }

    population -= deadRabbits;
    printf("population : %d\n"
           "dead rabbits : % d",
            population, deadRabbits);
    return population;
}



int main()
{
    //int gen;
    //printf("Number of generations :");scanf("%d", &gen);
    //printf(fibonacci(gen));
    sim(100, 240);
    return 0;
}


