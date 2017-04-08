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


int prime(long int pr) {
    int i;
    int j = (int) sqrt(pr);
    for (i = 2; i <= j; i++) {
        if (pr % i == 0)
            return 0;
    }
    return 1;
}

long int cd(long int x, long int t) {
    long int k = 1;
    while (1) {
        k = k + t;
        if (k % x == 0)
            return (k / x);
    }
}

long int generate(long int pub[], long int priv[]) {
    long int p, q;
    srand((unsigned) time(NULL) + getpid());   // should only be called once
    p = 5, q = 11;
    /*while (prime(q) != 1) {
        q = rand() % (100 - 5) + 5;
    }*/
    /*while (prime(p) != 1) {
        p = rand() % (q - 3) + 3;
    }*/
    long int n = p * q, t;
    t = (p - 1) * (q - 1);
    ce(p, q, t, pub, priv);
    return n;
}

void ce(long int p, long int q, long int t, long int e[], long int d[]) {
    int k;
    k = 0;
    for (int i = 2; i < t; i++) {
        if (t % i == 0)
            continue;
        long int flag = prime(i);
        if (flag == 1 && i != p && i != q) {
            e[k] = i;
            flag = cd(e[k], t);
            if (flag > 0) {
                d[k] = flag;
                k++;
            }
            if (k == 99)
                break;
        }
    }
}


long int *encrypt(char *msg, long int e, long int n, long int temp[]) {
    long int pt, ct, key = e, k, len;
    int i = 0, j;
    len = strlen(msg);
    while (i != len) {
        pt = msg[i];
        pt = pt - 96;
        k = 1;
        for (j = 0; j < key; j++) {
            k = k * pt;
            k = k % n;
        }
        temp[i] = k;
        ct = k + 96;
        msg[i] = ct;
        i++;
    }
    return temp;
}

void decrypt(char *msg, long int d, long int n, long int *temp) {
    long int pt, ct, key = d, k;
    int i = 0, j;
    while (i < strlen(msg)) {
        ct = temp[i];
        k = 1;
        for (j = 0; j < key; j++) {
            k = k * ct;
            k = k % n;
        }
        pt = k + 96;
        msg[i] = pt;
        i++;
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

char *readLine(char *filename, int n) {
    FILE *file = fopen(filename, "r");
    int count = 0;
    if (file != NULL) {
        char *line = malloc(10 * sizeof(long int));
        while (fgets(line, sizeof line, file) != NULL) {
            if (count == n) {
                fclose(file);
                return line;
            } else
                count++;
        }
    }
}


void rmKEYS() {
    int shmid;
    int shmidpid;
    int shmidadr;

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
