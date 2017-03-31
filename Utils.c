//
// Created by mamour on 24/03/17.
//

#include "Projet_ProgSys.h"


int controleArguments(int argc, char *argv) {
    int nbproc = 0;
    if (argc == 1) {
        printf("Erreur d'execution: nombre d'arguments incorrect\n");
        printf("\t\tLe programme prend en entree au moins argument : le nombre de "
                       "de processus a creer\n\n");
        printf("\t\tVeuillez donner le nombre de processus a creer : ");
        scanf("%d", &nbproc);
    } else
        nbproc = atoi(argv);
    if (nbproc < 2) {
        do {
            printf("\t\tErreur d'execution: Le nombre de processus doit etre superieure ou "
                           "egale a 2\n");
            printf("\t\tVeuillez entrer le nombre de processus: ");
            scanf("%d", &nbproc);
        } while (nbproc < 2);
    }
    return nbproc;
}

void handler_tubesDispo(int signum) {
    if (signum == SIGUSR1) {
        printf("Processus [%d] : signal SIGUSR1 recu:\n", getpid());
    }
}
