#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mt19937ar.h"

#define TIME_STEP 1 //in months


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
    char sexe;            // M: Male, F: Female
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
void updateStats(Srabbit* rabbit) {
    int rabbitAge = rabbit->age;

    rabbitAge += TIME_STEP;

    //update maturaty
    if (rabbitAge >= 5 && rabbitAge<=8) {

        float randNB = genrand_real1() * 100;
        if (randNB <= (rabbitAge - 4) * 25) {

            rabbit->mature = 1;
            rabbit->srvRate = 60;
        }
    }
    if (rabbit->status != 0) { //not dead
        if ((rabbit->age / 12) >= 11) {  //is 11 years old or more

            rabbit->srvRate = (rabbit->srvRate >= 10) ? (rabbit->srvRate - 10) : 0;
        }
    }
    else { //dead
        rabbit->status = 0;
    }
}

int giveBirth(Srabbit*  rabbit) {
    int prctg = 25;
    
    if (rabbit->sexe == 'F' && rabbit->mature == 1 && rabbit->pregnant == 0) {
        if (rabbit->nbLitters < rabbit->nbLittersY) {

            rabbit->nbLitters += 1;
            float randNB = genrand_real1() * 100;
            while (1){
                if (randNB <= prctg) {
                    return 2 + prctg / 25;
                }
                else {
                    prctg += 25;
                }
            }
        }

    }
}

void createRabbitsList();

void sim(int N) {  //number of months
    Srabbit* rabbit = malloc(sizeof(Srabbit*));
    int population = 2;
    int newKittens;

    //first 2 rubbits
    *rabbit = (Srabbit){ 'M', 1, 0, 0, 0, 0, 35, &(Srabbit) { 'F', 1, 0, 0, 0, 0, 35, NULL } };

    for (int i = 0; i < N; i += TIME_STEP) {
        //run of all the existing rabbits
        for (int j = 0; j < population; j++) {
            if (rabbit->status != 0) {  // if not already dead

                newKittens = giveBirth(rabbit);
                updateStats(rabbit);

                rabbit = rabbit->nextRabbit;
            }
        }
    }

}



int main()
{
    int gen;
    printf("Nombre de géneration ?\n");
    scanf("%d", &gen);
    printf(fibonacci(gen));
    return 0;
}


