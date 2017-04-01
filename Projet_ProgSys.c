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
    char messageAller[100] = "processus1";
    char messageRetour[100];
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
            write(desc2, messageAller, sizeof(messageAller));
            printf("Processus[%d] : valeur envoyée %s.\n", getpid(), messageAller);
            read(desc1, messageAller, sizeof(messageAller));
            //sleep(3);
            printf("Processus[%d] : valeur lue %s.\n\n\n", getpid(), messageAller);
            close(desc2);
            close(desc1);
            //Fin Parcours premier sens

            //Debut parcours sens inverse
            sleep(4);
            strcpy(messageRetour, messageAller);
            desc4 = open(backwardPipe[0], O_WRONLY);
            desc3 = open(backwardPipe[nombreDeProcessus - 1], O_RDONLY);
            write(desc4, messageRetour, sizeof(messageRetour));
            //printf("Processus[%d] : valeur envoyée %s.\n", getpid(), messageRetour);
            read(desc3, messageRetour, sizeof(messageRetour));
            printf("Processus[%d] : valeur lue %s.\n", getpid(), messageRetour);
            close(desc4);
            close(desc3);
            fflush(stdout);
            //Fin parcours sens inverse pour P1
        } else {
            //Debut parcours premier sens
            desc1 = open(forwardPipe[numeroProc - 2], O_RDONLY);
            desc2 = open(forwardPipe[numeroProc - 1], O_WRONLY);
            read(desc1, messageAller, sizeof(messageAller));
            printf("Processus[%d] : valeur lue %s.\n", getpid(), messageAller);
            char str[10];
            strcat(messageAller, "processus");
            sprintf(str, "%d", numeroProc);
            strcat(messageAller, str);
            write(desc2, messageAller, sizeof(messageAller));

            /**
             * Le processus Pn ecrit dans le fichier
             * sharedInfo.txt les messages partages
             * par les processus
             */
            if (numeroProc == nombreDeProcessus) {
                int fd;
                fd = open("sharedInfo.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
                write(fd, messageAller, strlen(messageAller) * sizeof(char));
                close(fd);
            }
            printf("Processus[%d] : valeur envoyée %s.\n", getpid(), messageAller);
            close(desc1);
            close(desc2);
            //Fin parcours premier sens

            //Debut parcours sens inverse
            sleep(4);
            desc3 = open(backwardPipe[nombreDeProcessus - numeroProc], O_RDONLY);
            desc4 = open(backwardPipe[nombreDeProcessus - numeroProc + 1], O_WRONLY);
            read(desc3, messageRetour, sizeof(messageRetour));
            printf("Processus[%d] : valeur lue %s.\n", getpid(), messageRetour);

            /**
             * Chaque processus Pi enleve son
             * information et envoie le reste
             * a Pi-1
             */
            char buf[100];
            for (int i = 0; i < strlen(messageRetour) - 10; i++) {
                buf[i] = messageRetour[i];
            }
            strcpy(messageRetour, buf);

            write(desc4, messageRetour, sizeof(messageRetour));
            printf("Processus[%d] : valeur envoyée %s.\n", getpid(), messageRetour);
            close(desc3);
            close(desc4);
            //fin parcours sens inverse
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