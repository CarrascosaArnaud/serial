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
//#include "mbed.h"

//AnalogIn capteur_temperature(ADC_TEMP); //le capteur interne est une entrée analogique

#define FALSE 0
#define TRUE 1
#define _POSIX_SOURCE 1
#define PORTSERIE "/dev/ttyS0"//Défini la constante sur le chemin sur lequel la carte est branchée
#define BAUDRATE 115200

//volatile int STOP = FALSE;

/*
void print (std::string::size_type n, std::string const &buf)
{
	if (n == std::string::npos){
		std::cout << "not found\n";
	}else {
		std:: cout << " " << buf.substr(n) << endl;
	}
}
*/

int main(){

	int sfd, c, res;
	std::string::size_type n;
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

		for (int i = 0; i < 100; i++){//Tourne 50 fois
			res = read(sfd,buf,125);//retourne après la lecture de 255 char
			std::string s(buf);
			buf[res]=0;
			
			//cout << s << endl;
			if (s.find("Temp[0]") != string::npos){
				cout << "Temp !";
			}
			else if(s.find("Hum[0]") != string::npos){
				cout << "Hum !";
			}
			else if(s.find("Press[0]") != string::npos){
				cout << "Press !";
			}

		
				//n = s.find("Temp[0]");
				//print(n,s);
			
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

//voir sprintf

//http://electroniqueamateur.blogspot.com/2017/09/mesurer-une-temperature-avec-une-carte.html
}

