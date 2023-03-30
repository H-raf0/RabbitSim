#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mt19937ar.h"

#define TIME_STEP 1 //1 month, modifing it may result in errors

FILE* flogs;

int fibonnaci(int N) {

    if (N == 0 || N == 1) {
        return 2;
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

void freeMem(Srabbit** rabbit) {
    Srabbit* currentRabbit = *rabbit;
    Srabbit* saveP;

    while (currentRabbit != NULL) {
        saveP = currentRabbit;
        currentRabbit = currentRabbit->nextRabbit;
        free(saveP);
    }
}

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
    if (rabbitAge >= 5 && rabbitAge <= 8 && rabbit->mature == 0) {

        float randNB = genrand_real1() * 100;
        while (rabbit->mature == 0) {
            if (randNB <= prctg) {
                rabbit->mature = 1 + 10 * (rabbitAge - 1 + prctg / 25);
                rabbit->srvRate = 60;
            }
            else {
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

int giveBirth(Srabbit* cRabbit) {
    int prctg = 25;

    if (cRabbit->sex == 'F' && cRabbit->mature % 10 == 1) { //a mature not dead female rabbit

        if (cRabbit->nbLitters < cRabbit->nbLittersY) { // did less then her avg and not pregnant


            if (cRabbit->pregnant == 1) { // if pregnant
                float randNB = genrand_real1() * 100;
                while (1) {
                    // return nb of kittens [3,6]
                    if (randNB <= prctg) {
                        cRabbit->pregnant = 0;
                        return 2 + prctg / 25;
                    }
                    else {
                        prctg += 25;
                    }
                }
            }
            else {
                cRabbit->nbLitters += 1;
                cRabbit->pregnant = 1;
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

            Srabbit* newRabbit = (Srabbit*)malloc(sizeof(Srabbit));

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
    Srabbit* rabbit = (Srabbit*)malloc(sizeof(Srabbit));
    Srabbit* currentRabbit;
    Srabbit* headListRabbit;
    FILE* fp;

    int population = startNB;
    int addedPopulation = 0;
    int newKittens;
    int deadRabbits = 0;
    float progress;

    flogs = fopen("LOGS.txt", "a");
    if (flogs == NULL) {
        fprintf(flogs, "Failed to open the file `LOGS.txt`\n");
        return 1;
    }

    fp = fopen("output.txt", "w");
    if (flogs == NULL) {
        fprintf(flogs, "Failed to open the file `output.txt`\n");
        return 1;
    }


    //first 2 rubbits
    *rabbit = (Srabbit){ generate_sex(), 1, 0, 0, 0, 0, 0, 35, NULL };
    createRabbitsList(startNB - 1, &rabbit);

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
        printf("month %d / %d :\n", i + 1, N);


        for (int j = 0; j < population; j++) {


            if (j % 10000 == 0 && j != 0) {

                // transforme i to a percentage and prints it
                progress = j * 100.0 / population;
                printf("progress : %.2f %%\r", progress);
            }

            if (currentRabbit->status != 0) {  // if not already dead
                updateStats(currentRabbit);

                //if (currentRabbit->srvRate == 0) { currentRabbit->status = 0;  deadRabbits++; }

                if (currentRabbit->srvRate >= 0) {
                    if (genrand_real1() * 100 <= (100 - currentRabbit->srvRate)) {
                        currentRabbit->status = 0;
                        deadRabbits++;
                        //population--;
                        if (j != (population - 1)) { //to avoid getting last pointer which is equal to NULL
                            currentRabbit = currentRabbit->nextRabbit;
                        }
                        continue;
                    }
                    currentRabbit->srvRate = currentRabbit->srvRate * -1;
                }
                newKittens = giveBirth(currentRabbit);
                addedPopulation += newKittens;

                tempRabbit = createRabbitsList(newKittens, &tempRabbit);
            }


            if (j != (population - 1)) { //to avoid getting last pointer which is equal to NULL
                currentRabbit = currentRabbit->nextRabbit;
            }


        }
        printf("progress finished  \n");
        system("cls");
        if (headListRabbit->nextRabbit != NULL) {
            currentRabbit->nextRabbit = headListRabbit->nextRabbit; // linking the new generation to the old one
            population += addedPopulation; // adding the new rabbits
            addedPopulation = 0;
        }
    }

    population -= deadRabbits;
    printf("\nresults:\npopulation : %d\n"
        "dead rabbits : % d\n",
        population, deadRabbits);
    
    fprintf(fp, "population : %d\n"
        "dead rabbits : % d\n",
        population, deadRabbits);
    
    fclose(fp);
    fclose(flogs);
    freeMem(&rabbit);
    return population;
}

int print_pointers(Srabbit* rabbit) {
    FILE* fp;
    int n = 0;
    Srabbit* currentRabbit = rabbit;

    fp = fopen("pointers.txt", "w");
    if (flogs == NULL) {
        fprintf(flogs, "Failed to open the file `output.txt`\n");
        return 1;
    }

    while (currentRabbit != NULL) {
        fprintf(fp, "\nID : %d\nsex : %c\nstatus : %d\nage : %d\nmature : %d\npregnant : %d\nnbLittersY : %d\nnbLitters : %d\nsrvRate : %d\nnextRabbit : %p\n",
            n, currentRabbit->sex, currentRabbit->status, currentRabbit->age, currentRabbit->mature, currentRabbit->pregnant,
            currentRabbit->nbLittersY, currentRabbit->nbLitters, abs(currentRabbit->srvRate), currentRabbit->nextRabbit);
        n++;
        currentRabbit = currentRabbit->nextRabbit;
    }
    fprintf(fp, "\nEND\n");
    fclose(fp);
    return n;
}

int main()
{
    //int gen;
    //printf("Number of generations :");scanf("%d", &gen);
    //printf(fibonacci(gen));
    sim(10, 120);
    //fibonnaci(5);
    return 0;
}
