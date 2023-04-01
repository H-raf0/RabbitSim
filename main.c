#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mt19937ar.h"

#define TIME_STEP 1 //1 month, modifing it may result in errors


/*
|------------------------------------------------------------------------------|
|fibonnaci                                                                     |
|use : simulate the production of rabbits using fibonnaci methode.             |
|                                                                              |
|input : N, number of months to simulate.                                      |
|                                                                              |
|output : the number of rabbits after N months.                                |
|------------------------------------------------------------------------------|
*/
int fibonnaci(int N) {

    if (N == 0 || N == 1) {
        return 2;
    }
    else {
        return fibonnaci(N - 1) + fibonnaci(N - 2);
    }
}


/*
|------------------------------------------------------------------------------|
|Srabbit                                                                       |
|use : a structure that represent one rabbit.                                  |
|------------------------------------------------------------------------------|
*/
typedef struct Rabbit {
    char sex;            // M: Male, F: Female
    int status;           // 0:dead ou 1:alive
    int age;              // 0:just born, >0 age in months
    int mature;           // 0:no, x1:yes(adult) x is the age it become adult
    int pregnant;         // 0:no, 1:yes
    int nbLittersY;       // number of litters a female rabbit must have in a year
    int nbLitters;        // number of litters made in a year
    int srvRate; // survive rate <= 100%, <0 means has been used, >=0 will be used (is positif when created or updated)
    struct Rabbit* nextRabbit;  // next rabbit

}Srabbit;

/*
|------------------------------------------------------------------------------|
|freeMem                                                                       |
|use : free the memory allocated by all the rabbits used in the linked list.   |
|                                                                              |
|input : rabbit, the head of the linked list.                                  |
|------------------------------------------------------------------------------|
*/
void freeMem(Srabbit** rabbit) {
    Srabbit* currentRabbit = *rabbit;
    Srabbit* saveP;

    while (currentRabbit != NULL) {
        saveP = currentRabbit;  // save the adress of the memory to free
        currentRabbit = currentRabbit->nextRabbit;  // jump to the next one
        free(saveP);  // free the saved memory adress
    }
}

/*
|------------------------------------------------------------------------------|
|generateAvgLitters                                                            |
|use : generate the average litters in a year for a female rabbit.             |
|                                                                              |
|input : rabbit, the rabbit that needs an average.                             |
|------------------------------------------------------------------------------|
*/
void generateAvgLitters(Srabbit* rabbit) {
    float randNB = genrand_real1();
    if (randNB <= 40) rabbit->nbLittersY = 6;                      //           6:60%              
    else if (randNB <= 60) rabbit->nbLittersY = 5;                 //       5:20%   7:20%
    else if (randNB <= 80) rabbit->nbLittersY = 7;                 //   4:10%           8:10%
    else if (randNB <= 90) rabbit->nbLittersY = 4;
    else if (randNB <= 100) rabbit->nbLittersY = 8;
}

/*
|------------------------------------------------------------------------------|
|updateStats                                                                   |
|use : used to update stats of each rabbit every month.                        |
|                                                                              |
|input : rabbit, the memory address of the rabbit.                             |
|------------------------------------------------------------------------------|
*/
void updateStats(Srabbit* rabbit) {

    rabbit->age += TIME_STEP;
    int rabbitAge = rabbit->age;
    int prctg = 25;

    //update maturaty
    if (rabbitAge >= 5 && rabbitAge <= 8 && rabbit->mature == 0) { // if not mature and have an age between 5 and 8

        float randNB = genrand_real1() * 100;
        // update the mature value of the rabbit with 1x when x is between 5 and 8 and represent the age a rabbit become mature
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
    if (rabbit->sex == 'F' && rabbit->mature > 0 && (rabbit->age % 12 == rabbit->mature / 10)) { // if a rabbit is a mature female that become recently mature or after X years from being adult

        rabbit->nbLitters = 0; 
        generateAvgLitters(rabbit); // will get a new average for the new year
    }
    if (rabbit->status != 0) { //not dead
        if ((rabbit->age / 12) >= 11) {  //is 11 years old or more

            rabbit->srvRate = (abs(rabbit->srvRate) >= 10) ? (abs(rabbit->srvRate) - 10) : 0;
        }
    }
}

/*
|------------------------------------------------------------------------------|
|giveBirth                                                                     |
|use : give the number of kittens a rabbit must give in a litter.              |
|                                                                              |
|input : cRabbit, the rabbit that will produce kittens.                        |
|                                                                              |
|output : the number the kittens produced.                                     |
|------------------------------------------------------------------------------|
*/
int giveBirth(Srabbit* cRabbit) {
    if (cRabbit->sex == 'F' && cRabbit->mature % 10 == 1) { //a mature female rabbit
        int prctg = 25;

        if (cRabbit->nbLitters < cRabbit->nbLittersY) { // gived litters less then her avgerage


            if (cRabbit->pregnant == 1) { // if pregnant
                float randNB = genrand_real1() * 100;
                // return nb of kittens [3,6]
                while (1) {
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
/*
|------------------------------------------------------------------------------|
|generate_sex                                                                  |
|use : Compute the value of PI with the Monte Carlo method.                    |
|                                                                              |
|output : a random sex (M for male or F for female).                           |
|------------------------------------------------------------------------------|
*/
char generate_sex() {
    return (genrand_real1() * 100 <= 50) ? 'M' : 'F';
}

/*
|------------------------------------------------------------------------------|
|createRabbitsList                                                             |
|use : created a linked list of a specific number of rabbits.                  |
|                                                                              |
|input : head, the head of the generated list.                                 |
|        nb,   the number of rabbits to add.                                   |
|                                                                              |
|output : the last element of the list created.                                |
|------------------------------------------------------------------------------|
*/
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
/*
|------------------------------------------------------------------------------|
|sim                                                                           |
|use : Compute the value of PI with the Monte Carlo method.                    |
|                                                                              |
|input : startNB, number of rabbits to start the simulation with.              |
|        N, number of months                                                   |
|                                                                              |
|output : the number of the alive population at the end of the experiment.     |
|------------------------------------------------------------------------------|
*/
int* sim(int startNB, int N) {
    Srabbit* rabbit = (Srabbit*)malloc(sizeof(Srabbit));
    Srabbit* currentRabbit;
    Srabbit* headListRabbit;

    int population = startNB;
    int addedPopulation = 0;
    int newKittens;
    int deadRabbits = 0;
    float progress;

    //first startNB rubbits
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


        for (int j = 0; j < population; j++) {

            // to print the progress of the calculations
            if (j % 10000 == 0 && j != 0) {

                // transforme i to a percentage and prints it
                progress = j * 100.0 / population;
                printf("month %d / %d :  progress : %.2f %%\r", i + 1, N, progress);
            }

            if (currentRabbit->status != 0) {  // if not already dead
                updateStats(currentRabbit); //update the rabbit stats

                if (currentRabbit->srvRate >= 0) {
                    if (genrand_real1() * 100 <= (100 - currentRabbit->srvRate)) { // test if the rabbit ill die or no
                        currentRabbit->status = 0; // if dead
                        deadRabbits++;
                        if (j != (population - 1)) { //to avoid getting last pointer which is equal to NULL
                            currentRabbit = currentRabbit->nextRabbit;
                        }
                        continue;
                    }
                    currentRabbit->srvRate = currentRabbit->srvRate * -1; // to avoid testing the chance of surviving with migh lead to the death of all of them 
                }
                newKittens = giveBirth(currentRabbit); // how much kittens the current rabbit must give
                addedPopulation += newKittens;  // add this number to addedPopulation to add it to the population later

                tempRabbit = createRabbitsList(newKittens, &tempRabbit);  // created a list for the new born rabbits
            }


            if (j != (population - 1)) { //to avoid getting last pointer which is equal to NULL
                currentRabbit = currentRabbit->nextRabbit;
            }


        }
        printf("progress finished                     \r");

        if (headListRabbit->nextRabbit != NULL) {
            currentRabbit->nextRabbit = headListRabbit->nextRabbit; // linking the new generation to the old one
            population += addedPopulation; // adding the new rabbits
            addedPopulation = 0;
        }
        free(headListRabbit);
    }

    population -= deadRabbits;
    
    freeMem(&rabbit);

    int* tab = malloc(2 * sizeof(int));
    if (tab == NULL) {
        printf("error creating tab");
        return 1;
    }
    tab[0] = population;
    tab[1] = deadRabbits;
    return tab;
}

/*
|------------------------------------------------------------------------------|
|multiSim                                                                      |
|use : to simuate sim multiple times.                                          |
|                                                                              |
|input : startNB, number of rabbits to start the simulation with.              |
|        N, number of months.                                                  |
|        times, number of simulations.                                         |
|------------------------------------------------------------------------------|
*/
void multiSim(int startNB, int N, int times) {
    int* tab;
    int population = 0, deadRabbits = 0;

    for (int i = 0; i < times; i++) {
        printf("->simulation number %d\n", i + 1);
        tab = sim(startNB, N);

        // print results of each simulation in the output.txt file
        printf("\nresults:\n   alive population : %d\n   dead rabbits : % d\n\n",
            tab[0], tab[1]);

        population += tab[0];
        deadRabbits += tab[1];
        free(tab);
    }

    population /= times;
    deadRabbits /= times;

    printf("\n>the average<\ninput:\n   start number: %d\n   months : %d\n   number of simulations %d\nresults:\n   alive population : %d\n"
        "   dead rabbits : % d\n",
        startNB, N, times, population, deadRabbits);
}

int main()
{
    //int gen;
    //printf("Number of generations :");scanf("%d", &gen);
    //printf(fibonacci(gen));
    //sim(2, 100);
    multiSim(100, 140, 3);
    return 0;
}
