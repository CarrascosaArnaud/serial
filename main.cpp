#include <string.h>
#include <stdio.h>
#include <unistd.h>//Pour interagir avec le port série
#include <fcntl.h> //Descripteurs de fichiers
#include <termios.h> //Contrôle des ports de communication asynchrone
#include <errno.h>//Gestion des erreurs

#define PORTSERIE "/dev/ttyS0"//Défini la constante sur le chemin sur lequel la carte est branchée
#define BAUDRATE 115200
int main(){
	int res;
	char buf[255];
	struct termios newtio;//Crée la structure
	bzero(&newtio, sizeof(newtio));//Initialisaiton de la structure
	newtio.c_cflag = B115200 | CRTSCTS | CS8 | CLOCAL | CREAD;//On lui affecte des paramètres
	newtio.c_iflag = IGNPAR | ICRNL;//On lui affecte des paramètres
	newtio.c_lflag = ICANON;//On lui affecte des paramètres
	int sfd = open(PORTSERIE, O_RDONLY | O_NOCTTY);//Ouvre le port série en lecture seule, sans que le terminal contrôle le processus 
	tcflush(sfd, TCIFLUSH);//Vide la ligne du modem
	tcsetattr(sfd,TCSANOW,&newtio);//Active la configuration pour le port
	if (sfd == -1){//Si le port série ne s'ouvre pas
		printf ("Error no is : %d\n", errno);//Donne le numéro de l'erreur
		printf ("Error description is : %s\n", strerror(errno));//Donne la description de l'erreur
		return(-1);//Renvoie une erreur
	}
	else{//S'il n'y a pas d'erreur

		printf ("Port serie ouvert en lecture.\n");//Message s'affichant pour dire que le port est ouvert

		for (int i = 0; i < 50; i++){//Tourne 50 fois
			res = read(sfd,buf,127);//retourne après la lecture de 255 char
			buf[res]=0;
			printf(":%s:%d\n", buf, res);//affiche
		}
	}
	close(sfd);
	return 0;
}


	
/*
	
	struct termios configeTerm;
	bzero(&configTerm, sizeof(configTerm));
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	tcflush(sfd, TCIFLUSH);
	tcsetattr(sfd,TCSANOW,&newtio);
	
	flags	:
	BAUDRATE: Set bps rate.
	CRTSCTS : output hardware flow control (only used if the cable has all necessary lines.)
	CS8		: 8n1 (8bit, no parity, 1stopbit)
	CLOCAL	: local connection, no modem control
	CREAD	: enable receiving characters
	
*/

