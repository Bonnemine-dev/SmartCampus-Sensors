#ifndef VARIABLESGLOBALES_H
#define VARIABLESGLOBALES_H

#include "DHTesp.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include <SSD1306Wire.h>
#include <WebServer.h>
#include <Preferences.h>
#include <WiFiClientSecure.h>

// Port GPIO du capteur de température et humidité
#define DHTPIN 2

// Port GPIO du détecteur de mouvement
#define DETECTEUR_DE_MOUVEMENT_PIN 3

// Port GPIO du bouton blanc
#define BOUTON_BLANC_NBPIN 4

// Port GPIO du bouton bleu
#define BOUTON_BLEU_NBPIN 5

// Vitesse de communication avec le monitor
#define VITESSECOMSERIAL 9600 //bauds

// Temps d'attente avant la mise en veille de l'écran
#define MISE_EN_VEILLE_ECRAN 60 // En secondes

// #define WIFI_SSID "iPhone 14 pro"
// #define WIFI_MOTDEPASSE "jeancastex"

// Données de connexion au WiFi eduroam
extern String WIFI_SSID;          // nom du réseau
extern bool WIFI_SECURITY_TYPE; // securité du réseau
extern String WIFI_IDENTIFIANT;         // Login eduroam
extern String WIFI_MOTDEPASSE;    // Mot de passe eduroam
extern unsigned short int TEMPS_ENTRE_CHAQUE_TANTATIVE_DE_RECONNEXION; //En ms
#define TEMPS_ATTENTE_AVANT_DEMARRAGE_HOTSPOT 10 // En secondes

// Données de création du hotspot                  
#define AP_MOTDEPASSE "12345678"            // Mot de passe d'accès au hotspot

// Données nécessaire pour régler l'heure du système
#define NTP_ADRESSE "fr.pool.ntp.org"       // Adresse du serveur NTP 
#define GMT 3600                            // Définition du décalage horraire par rapport à GMT
#define HEURE_ETE 3600                      // Définition du décalage horraire en heure d'été


extern unsigned short int DUREE_CYCLE_DE_CAPTURES;
extern unsigned short int  NB_CAPTURES_PAR_CYCLE;
#define TEMPERATURE_GAP_MAX 5
#define HUMIDITE_GAP_MAX 15
#define TAUXCARBONE_GAP_MAX 600
//Nombre de valeurs nécéssaire pour vérifier si une valeur est abérante
#define NB_VALEURS_POUR_VERIFICATION 4

// Données nécessaire pour utiliser l'API
extern String URL_API;  // URL de l'API
#define VALEUR_ACCEPTER "application/json"                          // Valeurs accepter en réponses
#define NOM_DB "sae34bdl2eq2"                                       // Nom de la base de données du groupe
#define NOM_UTILISATEUR "l2eq2"                                     // Nom d'utilisateur, pour accéder à la base de données
#define MOT_DE_PASSE "wiqnyt-fuqgyc-7vUhby"                         // Mot de passe, pour accèder à la base de données

extern String NOM_SA;
extern String NOM_SALLE;

extern DHTesp dht;

extern HTTPClient http;

extern TaskHandle_t taskReconnexionWIFIHandle;

extern TaskHandle_t taskServerWEBHandle;

extern TaskHandle_t taskanimationEchec;

extern SSD1306Wire display;

extern unsigned short int etatESP;

extern unsigned short int alert;

extern unsigned short int ancienEtatESP;

extern Preferences configuration;

extern bool ecranAllume;

extern bool lumineusiteReduite;

extern unsigned long timerEcran;

extern String HEURE_ARRET;

extern String HEURE_DE_DEMARRAGE;

#endif