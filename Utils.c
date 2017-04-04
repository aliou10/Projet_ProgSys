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

void handlerPipe(int signum) {
    if (signum == SIGUSR1) {
        printf("Processus [%d] : signal SIGUSR1 recu tubes disponibles:\n", getpid());
    }
}

void handlerSegment(int signum) {
    if (signum == SIGUSR2) {
        printf("Processus [%d] : signal SIGUSR1 recu clés disponibles:\n", getpid());
        sleep(2);
    }
}

void handlerTerminaison(int signum) {
    if (signum == SIGUSR2) {
        pid = adressePidFils;
        printf("Processus [%d] : signal SIGUSR2 recu de [%d] : mort de [%d]\n",
               pid[numeroProc - 1], pid[numeroProc], pid[numeroProc + 1]);
    }
}

void logproc(int numero, int pid, char *message) {
    char log[13];
    sprintf(log, "logproc_%d.txt", numero);
    int fd = open(log, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    char num[1];
    char pidproc[10];
    sprintf(num, "%d", numero);
    sprintf(pidproc, "%d", pid);
    write(fd, "Numero: ", 9);
    write(fd, &num, sizeof(char));
    write(fd, "\n", 1);
    write(fd, "Pid: ", 6);
    write(fd, &pidproc, strlen(pidproc) * sizeof(char));
    write(fd, "\n", 1);
    write(fd, "Information: ", 13);
    write(fd, message, strlen(message));
    close(fd);
}

void rmKEYS() {
    int shmid;
    int shmidpid;

    /**
     * Recuperation des segments de memoire partagee
     */
    if ((shmid = shmget(CLE, 0, 0)) == -1) {
        perror("Erreur lors de la recuperation du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
    if ((shmidpid = shmget(CLE1, 0, 0)) == -1) {
        perror("Erreur lors de la recuperation du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }

    /**
     * Suppression des segments de memoire partagee
     */
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("Erreur lors de la suppression du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
    if (shmctl(shmidpid, IPC_RMID, 0) == -1) {
        perror("Erreur lors de la suppression du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
}
