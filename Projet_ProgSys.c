//
// Created by mamour on 24/03/17.
//

#include "Projet_ProgSys.h"
#include "Utils.c"


int main(int argc, char **argv) {

    /**
     * Nombre de processus cree
     */
    nombreDeProcessus = controleArguments(argc, argv[1]);

    /**
     * Joue le role de P0
     */
    pidMainProc = getpid();

    char forwardPipe[nombreDeProcessus][10];
    char backwardPipe[nombreDeProcessus][10];
    pid_t pid_fils[nombreDeProcessus];
    int desc1, desc2;
    int desc3, desc4;
    int messageAller = 1;
    int messageRetour = 8;
    int numeroProc = 0;


    /**
     * Creation des tubes par P0
     */
    for (int i = 0; i < nombreDeProcessus; i++) {
        sprintf(forwardPipe[i], "tube_%d", i);
        sprintf(backwardPipe[i], "tubeInv_%d", i);
        mkfifo(forwardPipe[i], S_IRUSR | S_IWUSR);
        mkfifo(backwardPipe[i], S_IRUSR | S_IWUSR);
    }


    /**
     * Creation des processus par P0
     */
    for (int i = 0; i < nombreDeProcessus; i++) {
        if (fork() == 0) {
            numeroProc++;
            pid_fils[i] = getpid();
        } else
            break;
    }

    /**
     * Les processus se positionnent pour recevoir
     * le signal SIGUSR1
     */
    if (getpid() != pidMainProc) {
        printf("Processus [%d] : en attente de SIGUSR1\n", getpid());
        pid_fils[numeroProc - 1] = getpid();
        sleep(2);
    }

    if (getpid() != pidMainProc) {
        if (signal(SIGUSR1, handler_tubesDispo) == SIG_ERR) {
            perror("Erreur lors du positionnement");
            exit(EXIT_FAILURE);
        }
    }

    /**
     * P0 envoit le signal SIGUSR1 a tous
     * les processus
     * ----Probleme a ce niveau----
     */
    if (getpid() == pidMainProc) {
        if (kill(pid_fils[numeroProc - 1], SIGUSR1) == -1) {
            perror("Erreur Lors de l'envoi du signal");
            exit(EXIT_FAILURE);
        }
    }


    /**
     * Communication bidirectionnelle entre les
     * processus crees
     */
    if (getpid() != pidMainProc) {
        if (numeroProc == 1) {
            desc2 = open(forwardPipe[0], O_WRONLY);
            desc1 = open(forwardPipe[nombreDeProcessus - 1], O_RDONLY);
            write(desc2, &messageAller, sizeof(int));
            printf("Serveur %d : valeur envoyée %d.\n", getpid(), messageAller);
            read(desc1, &messageAller, sizeof(int));
            sleep(3);
            printf("Serveur %d : valeur lue %d.\n", getpid(), messageAller);
            close(desc2);
            close(desc1);

            desc4 = open(backwardPipe[0], O_WRONLY);
            desc3 = open(backwardPipe[nombreDeProcessus - 1], O_RDONLY);
            write(desc4, &messageRetour, sizeof(int));
            printf("Serveur %d : valeur envoyée %d.\n", getpid(), messageRetour);
            read(desc3, &messageRetour, sizeof(int));
            printf("Serveur %d : valeur lue %d.\n", getpid(), messageRetour);
            close(desc4);
            close(desc3);
            fflush(stdout);
        } else {
            desc1 = open(forwardPipe[numeroProc - 2], O_RDONLY);
            desc2 = open(forwardPipe[numeroProc - 1], O_WRONLY);
            read(desc1, &messageAller, sizeof(int));
            printf("Client %d : valeur lue %d.\n", getpid(), messageAller);
            messageAller *= 2;
            write(desc2, &messageAller, sizeof(int));
            printf("Client %d : valeur envoyée %d.\n", getpid(), messageAller);
            close(desc1);
            close(desc2);

            sleep((unsigned int) numeroProc);
            desc3 = open(backwardPipe[nombreDeProcessus - numeroProc], O_RDONLY);
            desc4 = open(backwardPipe[nombreDeProcessus - numeroProc + 1], O_WRONLY);
            read(desc3, &messageRetour, sizeof(int));
            printf("Client %d : valeur lue %d.\n", getpid(), messageRetour);
            messageRetour *= 3;
            write(desc4, &messageRetour, sizeof(int));
            printf("Client %d : valeur envoyée %d.\n", getpid(), messageRetour);
            close(desc3);
            close(desc4);
        }
    }

    /**
     * Destruction des tubes par P0
     */
    for (int i = 0; i < nombreDeProcessus; i++) {
        unlink(forwardPipe[i]);
    }

    for (int i = 0; i < nombreDeProcessus; i++) {
        unlink(backwardPipe[i]);
    }


    return EXIT_SUCCESS;
}