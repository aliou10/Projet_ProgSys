workflow



En se basant sur la description du projet nous avons proposé un programme dont le déroulement s'effectue comme suit :
----------------------------------------------------------

        Création de l'anneau 


1 L'utilisateur de saisit le nombre de processus qu'il souhaiterait créer grace la fonction "ControleArguments"
2 Création l'anneau bidirectionnel constitué des différents tubes : 

 -ForwardPipe pour les tubes nommés  qui sont chargés de la communication dans le premier sens 
 
-BackwardPipe pour ceux chargés de la communication dans le sens inverse 



------------------------------------------------------------

       Création du segment de mémoire partagée pour stocker les clés publiques de chaque processus pour le RSA

3 le processus principal (main) crée le segment de mémoire 
             (Nous attachons le segment de mémoire partagé  AdresseSegmentsPublicKey) 
-----------------------------------------------------------


      Création du segment de mémoire partagée pour stocker les pid des processus créés


4 le processus principal (main) crée le segment de mémoire partagé pour faciliter l'envoi des signaux au différents processus  


------------------------------------------------------------


         Création des différents processus 


5 le processus principal P0 (main) crée tous les autres processus fils 

6 Chaque processus Pi créé générera ses clés privées et publiques 

7 
Pi s'attache au segment de mémoire partagée des clés publiques et y dépose sa clé publique et fait avancer le pointeur


8 Pi affiche ses clés privées et publiques 


9 Pi s'attache au segment de mémoire partagée contenant les PID pour y déposer son PID et avancer le pointeur


10 Pi se met alors en attente s'il n'est pas le dernier processus à être créé (pour la synchronisation de tous les processus ils attendent tous la notification du dernier processus leur signalant la disponibilité de toutes les clés publiques) 
11 

Le P (n-ieme) ,après avoir rempli les étapes précédentes,envoie un signal à tous les autres processus  leur faisant part de la disponibilité de toutes les clés 



12 Pour recevoir les signaux envoyés par le n-ième processus après les étapes ci-dessus chaque processus se positionnent et font appel à leur handler 

13 
faire dormir les processus pour 10s pour plus de synchronisation 


              ------------------------------------------------------------


              
   Communication bi-directionnelle


   ------------------------------

  Parcours 1er sens 


Pour le parcours de l'anneau dans le premier sens : 

  -S'il ne s'agit pas du premier processus 
	
14 le processus Pi crypte le message qu'il doit envoyer et le met dans le tube pour le processus suivant 

-S'il ne s'agit pas du processus de n 1 
	
15 le processus lit ce qu'il y'a dans son tube de lecture décrypte le message y ajoute son propre message le crypte et envoie ça au processus suivant

-S'il ne s'agit pas du dernier processus 
16 il affiche le message qu'il a envoyé

17 
Le dernier processus ecrit le message qui contient tous les messages des autres processus dans le fichier SharedInfos

-----------------------------


Parcours sens inverse 


Le parcours en sens inverse se déroule dans le même schéma que celui en sens normal sauf que chaque processus enlève le message qu'il avait envoyé en renvoie le message ainsi modifié





--------------------------------


Terminaison des processus 

18 le processus Pi-1 tue le processus Pi et envoie SIGUSR a Pi-2 et ainsi de suite jusqu'au dernier processus qui s'auto kill







A la fin du programme nous détruisons tous les tubes et fermons les descripteurs de fichiers








