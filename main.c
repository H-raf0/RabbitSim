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
    char sexe;  // M: Male, F: Female
    int status; //  0:desd ou 1:alive
    int age;    // 0:just born, -1 dead? (age in months)
    int mature; // 0:no, 1:yes(adult)
    Srabbit* nextRabbit;

}Srabbit;


//used to update age every time step
void updateAge(Srabbit* nextRabbit) {

    nextRabbit->age += 1;
}


void sim(int N) {  //number of months
    Srabbit* rabbit = malloc(sizeof(Srabbit*));
    int population = 2;

    //first 2 rubbits
    *rabbit = (Srabbit){ 'M', 1, 0, 0, &(Srabbit) { 'F', 1, 0, 0, NULL } };

    for (int i = 0; i < N; i += TIME_STEP) {
        //run of all the existing rabbits
        for (int j = 0; j < population; j++) {

            //code

            updateAge(rabbit);
            rabbit = rabbit->nextRabbit;
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


