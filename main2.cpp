/*
    Projet : Lecture de capteurs sur carte nucléo pour les récupérer sur une base de données
    Auteur : Carrascosa Arnaud
    Date : 14/03/2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> //Gestion de string
#include <unistd.h>//Pour interagir avec le port série
#include <fcntl.h>//Descripteurs de fichiers
#include <termios.h>//Contrôle des ports de communication asynchrone
#include <errno.h>//Gestion des erreurs
#include <mariadb/mysql.h>//Pour utiliser mysql
#include <iostream>//Permet l'utilisation du cout
using namespace std;//Pour ne pas avoir à rajouter des std::

#define PORTSERIE "/dev/ttyS0"//Définis la constante sur le chemin sur lequel la carte est branchée
#define BAUDRATE 115200//Définis le taux de BAUD


int main(){

    //Déclaration des variables
    int sfd, c, res;
    char buf[255];
    string tempOutput;
    string humOutput;
    string pressOutput;
    string requeteMySQL = "INSERT INTO mesure (pression,temperature,humidite) VALUES ('";//Début de requete SQL
    char requete[1024]; //Sera utilisée pour la requete finale

    //Ouvre le port série
    struct termios newtio;
    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = B115200 | CRTSCTS | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR | ICRNL;
    newtio.c_lflag = ICANON;
    sfd = open(PORTSERIE, O_RDONLY | O_NOCTTY);
    tcflush(sfd, TCIFLUSH);
    tcsetattr(sfd,TCSANOW,&newtio);

    if (sfd == -1){//Si le port série ne s'ouvre pas
   	 cout << "Numero d'erreur : " << errno << endl;//Donne le numéro de l'erreur
   	 cout << "Description d'erreur : " << strerror(errno) << endl;//Donne la description de l'erreur
   	 return(-1);//Renvoie une erreur
    }
    else{//S'il n'y a pas d'erreur

   	 cout << "Port serie ouvert en lecture.\n";//Message s'affichant pour dire que le port est ouvert

   	 //Boucle permettant de lire les données
   	 for (int i = 0; i < 90; i++){
   		 res = read(sfd,buf,125);
   		 string chaineDeCarac(buf);
   		 buf[res]=0;
   		 
   		 //Permet de d'extraire les données de températures
   		 if (chaineDeCarac.find("Temp[0]") != string::npos){//Si Temp[0] est trouvé alors :
   			 tempOutput = chaineDeCarac.substr(10, chaineDeCarac.find(" d"));//Extrait le début de la chaine
   			 tempOutput = tempOutput.substr(0,tempOutput.find(" "));//Enlève la fin de la chaine
   			 //cout << tempOutput << endl;//Affiche les données (utile pour le debug)
   		 }
   		 //Permet de d'extraire les données d'humidité
   		 if(chaineDeCarac.find("Hum[0]") != string::npos){//Si Hum[0] est trouvé alors :
   			 humOutput = chaineDeCarac.substr(8, chaineDeCarac.find(" %"));//Extrait le début de la chaine
   			 humOutput = humOutput.substr(0,humOutput.find(" "));//Enlève la fin de la chaine
   			 //cout << humOutput << endl;//Affiche les données (utile pour le debug)
   		 }
   		 //Permet de d'extraire les données de pression
   		 if(chaineDeCarac.find("Press[1]") != string::npos){//Si Press[1] est trouvé alors :
   			 pressOutput = chaineDeCarac.substr(10, chaineDeCarac.find(" h"));//Extrait le début de la chaine
   			 pressOutput = pressOutput.substr(0,pressOutput.find(" "));//Enlève la fin de la chaine
   			 //cout << pressOutput << endl;//Affiche les données (utile pour le debug)
   		 }
   	 }


   	 //Concaténation pour ajouter les données à la requête finale
   	 requeteMySQL += pressOutput +="','";
   	 requeteMySQL += tempOutput += "','";
   	 requeteMySQL += humOutput += "');";

   	 //cout << requeteMySQL << endl;//Affiche la requête SQL finale, utile pour le debug
   	 strcpy(requete, requeteMySQL.c_str());//Copie la requete du format string en tableau de char
    }

    close(sfd);//Ferme la lecture du port série

    MYSQL * conn;//Connexion à la DataBase

    //Gestion d'erreur de connexion
    if ((conn = mysql_init (NULL)) == NULL){
   	 cout << stderr << "N'a pas pu initiliser la base de donnees\n";//Message affiché si on n'arrive pas à se connecter à la DB
   	 return EXIT_FAILURE;//Fermeture du programme avec erreur
    }

    //Gestion d'erreur de connexion
    //localhost peut être remplacé par l'ip de la DB via adminer.php
    //bts et snir sont réciproquement le nom d'utilisateur et mdp pour se connecter à la DB
    //TPNucleoMesures est le nom de la DB
    if (mysql_real_connect (conn, "localhost", "bts", "snir", "TPNucleoMesures", 0, NULL, 0) == NULL){
   	 cout << stderr << "Erreur de connexion a la base de donnees\n";//Message affiché si on n'arrive pas à se connecter à la DB
   	 return EXIT_FAILURE;//Fermeture du programme avec erreur
    }

    //Permet d'envoyer une requête SQL
    if (mysql_query(conn, requete) != 0){
   	 cout << stderr << "Echec de requete\n";//Message affiché si la requête envoyée n'est pas acceptée
   	 return EXIT_FAILURE;//Fermeture du programme avec erreur
    }


    mysql_close(conn);//Déconnexion à la DB
    return EXIT_SUCCESS;//Fermeture du programme sans erreur
}
