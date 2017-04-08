//
// Created by mamour on 24/03/17.
//

#include "Projet_ProgSys.h"
#include "Utils.c"


/**
 * Compteur utilisé pour forcer
 * l'ordre d'entree des processus lors du
 * parcours en sens inverse
 */
int cpt = 1;
int g = 1;

int main(int argc, char **argv) {
    /**
     * Nombre de processus cree
     */
    nombreDeProcessus = controleArguments(argc, argv[1]);

    /**
     * Joue le role de P0
     */
    pidMainProc = getpid();

    /**
     * Tubes utilisés pour assurer
     * la communication dans les deux sens
     */
    char forwardPipe[nombreDeProcessus][10];
    char backwardPipe[nombreDeProcessus][10];

    /**
     * Descripteurs associés aux tubes
     */
    int desc1, desc2;
    int desc3, desc4;

    int shmid, shmidpid;

    /**
     * Messages qui seront
     * partagés
     */
    char messageAller[100] = "processus1";
    char messageRetour[100];

    long int *temp[nombreDeProcessus],
            tmp[100],
            pub[nombreDeProcessus][100],
            priv[nombreDeProcessus][100];
    long int n[nombreDeProcessus];


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
     * Création du segment de mémoire
     * partagée pour les clés publiques
     * par P0 et attachement
     */
    if ((shmid = shmget((key_t) CLE, 1000,
                        S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if (errno == EEXIST)
            fprintf(stderr, "Le segment de memoire partagee (cle=%d) existe deja\n",
                    CLE);
        else
            perror("Erreur lors de la creation du segment de memoire ");
        exit(EXIT_FAILURE);
    }
    if ((adresseSegmentPublicKeys = shmat(shmid, NULL, 0)) == (void *) -1) {
        perror("Erreur lors de l'attachement du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
    publicKey = adresseSegmentPublicKeys;



    /**
     * Le processus P0 met aussi en place un segment
     * de memoire partager pour les pid des
     * processus.Ceci faciletera l'envoie de
     * signaux
     */
    if ((shmidpid = shmget((key_t) CLE1, 1000,
                           S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if (errno == EEXIST)
            fprintf(stderr, "Le segment de memoire partagee (cle=%d) existe deja\n",
                    CLE);
        else
            perror("Erreur lors de la creation du segment de memoire ");
        exit(EXIT_FAILURE);
    }
    if ((adressePidFils = shmat(shmidpid, NULL, 0)) == (void *) -1) {
        perror("Erreur lors de l'attachement du segment de memoire partagee ");
        exit(EXIT_FAILURE);
    }
    pid = adressePidFils;

    /**
     * Creation des processus par P0
     */
    for (int i = 0; i < nombreDeProcessus; i++) {
        if (fork() == 0) {
            numeroProc++;
            n[numeroProc - 1] = generate(pub[numeroProc - 1], priv[numeroProc - 1]);
            /**
             * Chaque processus s'attache au segment de memoire
             * et y depose sa cle publique
             */
            if (shmat(shmid, NULL, 0) == (void *) -1) {
                perror("Erreur lors de l'attachement du segment de memoire partagee ");
                exit(EXIT_FAILURE);
            }
            *publicKey = pub[numeroProc - 1][0];
            ++publicKey;
            /**
             * Le processus depose son pid dans le
             * segment des pid
             */
            *pid = getpid();
            ++pid;

            /**
             * Les n-1 processus attendent la
             * notification du nieme processus
             * leur signalant la disponibilité
             * de toutes les cles publiques
             */
            if (numeroProc != nombreDeProcessus) {
                printf("Processus[%d]  attente disponibilite segment\n", getpid());
                if (signal(SIGUSR2, handlerSegment) == SIG_ERR) {
                    perror("Erreur jdj\n");
                    exit(EXIT_FAILURE);
                }
            }
            /**
             * Le processus Pn apres avoir deposé
             * sa clé publique envoie un signal
             * SIGUSR2 au n-1 processus
             */
            if (numeroProc == nombreDeProcessus) {
                pid = adressePidFils;
                for (int j = 0; j < nombreDeProcessus - 1; j++) {
                    if (kill(pid[j], SIGUSR2) == -1) {
                        printf("Erreur ggg\n");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        } else
            break;
    }


    /**
     * Les processus se positionnent pour recevoir
     * le signal SIGUSR1 leur notifiant la disponibilité
     * des tubes
     */
    if (getpid()) {
        sleep(5);
        if (getpid() != pidMainProc) {
            printf("Processus [%d] : attente disponibilité des tubes\n", getpid());
            if (signal(SIGUSR1, handlerPipe) == SIG_ERR) {
                perror("Erreur lors du positionnement");
                exit(EXIT_FAILURE);
            }
        } else {
            sleep(5);
            for (int i = 0; i < nombreDeProcessus; i++) {
                if (kill(pid[i], SIGUSR1) == -1) {
                    perror("Erreur ici");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    sleep(10);

    /**
     * Communication bidirectionnelle entre les
     * processus crees
     */

    /**
     * Parcours premier sens
     */
    if (getpid() != pidMainProc) {
        if (numeroProc == 1) {
            publicKey = adresseSegmentPublicKeys;
            pid = adressePidFils;
            printf("\n\n***** Parcours premier sens *****\n\n");
            desc2 = open(forwardPipe[0], O_WRONLY);
            desc1 = open(forwardPipe[nombreDeProcessus - 1], O_RDONLY);
            temp[numeroProc - 1] = encrypt(messageAller,
                                           publicKey[numeroProc], n[numeroProc - 1], tmp);
            int fd = open("mytemp", O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0644);
            for (int i = 0; i < strlen(messageAller); i++) {
                char bf[10];
                sprintf(bf, "%ld", temp[numeroProc - 1][i]);
                write(fd, bf, strlen(bf));
                write(fd, "\n", 1);
            }
            write(desc2, messageAller, sizeof(messageAller));
            printf("Processus[%d] : message crypté envoyé %s.\n", getpid(), messageAller);
            logproc(numeroProc, getpid(), messageAller);
            read(desc1, messageAller, sizeof(messageAller));
            close(desc2);
            close(desc1);
        }
        if (numeroProc != 1) {
            pid = adressePidFils;
            publicKey = adresseSegmentPublicKeys;
            desc1 = open(forwardPipe[numeroProc - 2], O_RDONLY);
            desc2 = open(forwardPipe[numeroProc - 1], O_WRONLY);
            read(desc1, messageAller, sizeof(messageAller));
            long int tep[10];
            for (int i = 0; i < strlen(messageAller); i++)
                tep[i] = atoi(readLine("mytemp", i));
            decrypt(messageAller, priv[numeroProc - 1][0], n[numeroProc - 2], tep);
            printf("Processus[%d] : valeur decryptee lue %s.\n", getpid(), messageAller);
            char str[10];
            strcat(messageAller, "processus");
            sprintf(str, "%d", numeroProc);
            strcat(messageAller, str);
            if (numeroProc == nombreDeProcessus)
                temp[numeroProc - 1] = encrypt(messageAller,
                                               publicKey[0], n[numeroProc - 1], tmp);
            if (numeroProc != nombreDeProcessus)
                temp[numeroProc - 1] = encrypt(messageAller,
                                               publicKey[numeroProc], n[numeroProc - 1], tmp);
            int fd1 = open("mytemp", O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0644);
            for (int i = 0; i < strlen(messageAller); i++) {
                char bf[10];
                sprintf(bf, "%ld", temp[numeroProc - 1][i]);
                write(fd1, bf, strlen(bf));
                write(fd1, "\n", 1);
            }
            char test[10];
            strcpy(test, "processus");
            strcat(test, str);

            /**
             * Pour l'instant le processus ne log que ses
             * infos personnelles pas toutes les traces de
             * son execution
             */
            logproc(numeroProc, getpid(), test);
            write(desc2, messageAller, sizeof(messageAller));

            /**
             * Le processus Pn ecrit dans le fichier
             * sharedInfo.txt les messages partagés
             * par les processus
             */
            if (numeroProc == nombreDeProcessus) {
                int fd;
                fd = open("sharedInfo.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
                long int tep1[10];
                for (int i = 0; i < strlen(messageAller); i++)
                    tep1[i] = atoi(readLine("mytemp", i));
                decrypt(messageAller, priv[numeroProc - 1][0], n[numeroProc - 2], tep1);
                write(fd, messageAller, strlen(messageAller) * sizeof(char));
                printf("Processus[%d] : ecriture sur fichier des infos : %s\n\n\n",
                       getpid(), messageAller);
                close(fd);
            }
            if (numeroProc != nombreDeProcessus)
                printf("Processus[%d] : valeur envoyée %s.\n", getpid(), messageAller);
            close(desc1);
            close(desc2);
        }
    }

    /**
     * Parcours sens inverse
     */
    if (getpid() != pidMainProc) {
        strcpy(messageRetour, messageAller);
        if (numeroProc == 1) {
            desc4 = open(backwardPipe[0], O_WRONLY);
            desc3 = open(backwardPipe[nombreDeProcessus - 1], O_RDONLY);
            write(desc4, messageRetour, sizeof(messageRetour));
            read(desc3, messageRetour, sizeof(messageRetour));
            long int tep[10];
            for (int i = 0; i < strlen(messageRetour); i++)
                tep[i] = atoi(readLine("mytemp", i));
            decrypt(messageRetour, priv[numeroProc - 1][0], n[numeroProc - 1], tep);
            printf("Processus[%d] : valeur lue %s.\n\n\n", getpid(), messageRetour);
            close(desc4);
            close(desc3);
            fflush(stdout);
        } else {
            desc3 = open(backwardPipe[nombreDeProcessus - numeroProc], O_RDONLY);
            desc4 = open(backwardPipe[nombreDeProcessus - numeroProc + 1], O_WRONLY);
            read(desc3, messageRetour, sizeof(messageRetour));
            long int tep[10];
            for (int i = 0; i < strlen(messageRetour); i++)
                tep[i] = atoi(readLine("mytemp", i));
            decrypt(messageRetour, priv[numeroProc - 1][0], n[numeroProc - 1], tep);
            if (numeroProc != nombreDeProcessus)
                printf("Processus[%d] : valeur decryptee lue %s.\n", getpid(), messageRetour);

            /**
             * Chaque processus Pi enleve son
             * information et envoie le reste
             * a Pi-1
             */
            char buf[100];
            for (int i = 0; i < strlen(messageRetour) - 10; i++)
                buf[i] = messageRetour[i];
            strcpy(messageRetour, buf);
            temp[numeroProc - 1] = encrypt(messageRetour,
                                           publicKey[numeroProc - 2], n[numeroProc - 1], tmp);
            int fd1 = open("mytemp", O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0644);
            for (int i = 0; i < strlen(messageAller); i++) {
                char bf[10];
                sprintf(bf, "%ld", temp[numeroProc - 1][i]);
                write(fd1, bf, strlen(bf));
                write(fd1, "\n", 1);
            }
            write(desc4, messageRetour, sizeof(messageRetour));
            printf("Processus[%d] : valeur cryptée envoyée %s.\n", getpid(), messageRetour);
            close(desc3);
            close(desc4);
        }
    }

    /**
     * Terminaison des processus
     */
    if (getpid() != pidMainProc) {
        if (signal(SIGUSR2, handlerTerminaison) == SIG_ERR) {
            perror("Erreur");
            exit(EXIT_FAILURE);
        }

        sleep((unsigned) numeroProc + 5);
        if (numeroProc != 1 && numeroProc != nombreDeProcessus) {
            /**
             * On force Pn-1 a entrer en premier
             * et ainsi de suite
             */
            if (numeroProc != nombreDeProcessus - cpt) {
                cpt++;
                sleep(10);
            }

            pid = adressePidFils;
            if (kill(pid[numeroProc], SIGKILL) == -1) {
                perror("Erreur 1");
                exit(EXIT_FAILURE);
            }
            if (kill(pid[numeroProc - 2], SIGUSR2) == -1) {
                perror("Erreur 2");
                exit(EXIT_FAILURE);
            }
        }
        if (numeroProc == 1) {
            pid = adressePidFils;
            if (numeroProc != nombreDeProcessus - cpt)
                sleep((unsigned) numeroProc + 10);
            if (kill(pid[numeroProc], SIGKILL) == -1) {
                perror("Erreur 3");
                exit(EXIT_FAILURE);
            }
            printf("Processus[%d]: mort de [%d]\n", pid[numeroProc - 1], pid[numeroProc]);
            printf("Processus[%d]: je me suicide\n", pid[numeroProc - 1]);
            if (kill(pid[numeroProc - 1], SIGKILL) == -1) {
                perror("Erreur");
                exit(EXIT_FAILURE);
            }
        }
    }

    sleep(10);

    /**
     * Destruction des tubes par P0
     */
    for (int i = 0; i < nombreDeProcessus; i++) {
        unlink(forwardPipe[i]);
    }

    for (int i = 0; i < nombreDeProcessus; i++) {
        unlink(backwardPipe[i]);
    }
    /**
     * Suppression des segments de memoire
     */
    /*adrtemp = adresseTemp;
    int j = 0;
    while (j < 10) {
        printf("tmp : %ld", adrtemp[0][j]);
        j++;
    }*/

    rmKEYS();


    return EXIT_SUCCESS;
}