#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>//Pour interagir avec le port série
#include <fcntl.h> //Descripteurs de fichiers
#include <termios.h> //Contrôle des ports de communication asynchrone
#include <errno.h>//Gestion des erreurs
#include <mariadb/mysql.h>
#include <iostream>
using namespace std;

#define PORTSERIE "/dev/ttyS0"//Défini la constante sur le chemin sur lequel la carte est branchée
#define BAUDRATE 115200


int main(){

	int sfd, c, res;
	char buf[255];

	struct termios newtio;//Crée la structure
	bzero(&newtio, sizeof(newtio));//Initialisaiton de la structure
	newtio.c_cflag = B115200 | CRTSCTS | CS8 | CLOCAL | CREAD;//On lui affecte des paramètres
	newtio.c_iflag = IGNPAR | ICRNL;//On lui affecte des paramètres
	newtio.c_lflag = ICANON;//On lui affecte des paramètres
	sfd = open(PORTSERIE, O_RDONLY | O_NOCTTY);//Ouvre le port série en lecture seule, sans que le terminal contrôle le processus 
	tcflush(sfd, TCIFLUSH);//Vide la ligne du modem
	tcsetattr(sfd,TCSANOW,&newtio);//Active la configuration pour le port
	if (sfd == -1){//Si le port série ne s'ouvre pas
		cout << "Error no is : " << errno << endl;//Donne le numéro de l'erreur
		cout << "Error description is : " << strerror(errno) << endl;//Donne la description de l'erreur
		return(-1);//Renvoie une erreur
	}
	else{//S'il n'y a pas d'erreur

		cout << "Port serie ouvert en lecture.\n";//Message s'affichant pour dire que le port est ouvert

		for (int i = 0; i < 80; i++){//Tourne 50 fois
			res = read(sfd,buf,125);//retourne après la lecture de 255 char
			string chaineDeCarac(buf);
			buf[res]=0;
			
			//cout << s << endl;
			if (chaineDeCarac.find("Temp[0]") != string::npos){
				cout << "Temp !";
			}
			if(chaineDeCarac.find("Hum[0]") != string::npos){
				cout << "Hum !";
			}
			if(chaineDeCarac.find("Press[1]") != string::npos){
				cout << "Press !";
			}
		}
	}

	close(sfd);

	MYSQL * conn;

	if ((conn = mysql_init (NULL)) == NULL){
		cout << stderr << "Could not init DB\n";
		return EXIT_FAILURE;
	}

	if (mysql_real_connect (conn, "localhost", "bts", "snir", "TPNucleoMesures", 0, NULL, 0) == NULL){
		cout << stderr << "DB Connection Error\n";
		return EXIT_FAILURE;
	}

	if (mysql_query(conn, "INSERT INTO mesure (pression,temperature,humidite) VALUES ('25.0','22.0','35.5')") != 0){
		cout << stderr << "Query Failure\n";
		return EXIT_FAILURE;
	}


	mysql_close(conn);
	return EXIT_SUCCESS;
}

