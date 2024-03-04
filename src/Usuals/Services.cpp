#include "Services.h"
#include "../variablesGlobales.h"
#include <WiFiClientSecure.h>
#include <time.h>
#include <qrcode.h> 
#include <Tasks/taskServerWeb.h>
#include <esp_task_wdt.h>

bool initWiFi(String ssid,bool security ,String motdepasse, String identifiant = ""){
    // Se connecter au réseau WiFi
    Serial.print("Connexion au réseau WiFi en cours...");
    WiFi.disconnect(true);
    if(security)WiFi.begin(ssid, WPA2_AUTH_PEAP, identifiant, identifiant, motdepasse);
    else WiFi.begin(ssid,motdepasse);
    int i = 0;
    for (int i = 0; i < TEMPS_ATTENTE_AVANT_DEMARRAGE_HOTSPOT; ++i) {
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Attente de 1 seconde
        esp_task_wdt_reset();
        Serial.print(".");
        if (WiFi.status() == WL_CONNECTED) {
            WIFI_SSID = ssid;
            WIFI_MOTDEPASSE = motdepasse;
            WIFI_IDENTIFIANT = identifiant;
            WIFI_SECURITY_TYPE = security;
            Serial.println(String("\nConnexion au réseau ") + WIFI_SSID + " établie avec succès");
            return true;
        }
    }

    Serial.println("\nImpossible de se connecter à ce réseau");
    if(taskServerWEBHandle == NULL) {
        xTaskCreate(SERVER_WEB, "SERVER_WEB", 4096, NULL, 2, &taskServerWEBHandle);
    }
    // WiFi.disconnect(true);
    return false;
}

void dateInit() {
    // Configuration de la connexion NTP
    configTime(GMT, HEURE_ETE, NTP_ADRESSE);

    // Attente de la synchronisation de l'heure
    struct tm timeinfo;
    Serial.println("\nSynchronisation du temps");
    unsigned short int i = 0;
    while (!getLocalTime(&timeinfo)) {
        if (i++ > 10) {
            Serial.println("\nImpossible de synchroniser l'heure");
            ESP.restart();
            return;
        }
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nHeure synchronisée avec succès !");
    Serial.println(dateActuelle());
}

String dateActuelle() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "Erreur lors de l'obtention de la date";
    }
    char dateFormate[20]; // dateFormate pour stocker la date formatée
    strftime(dateFormate, sizeof(dateFormate), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(dateFormate);
}

float calculerMoyenne(const std::vector<float>& valeurs) {
    if (valeurs.empty()) return 0.0;

    float somme = 0.0;
    for (float val : valeurs) {
        somme += val;
    }
    return somme / valeurs.size();
}

std::vector<float> filtreAberrantes(const std::vector<float>& valeurs, float ecartMax) {
    std::vector<float> valeursFiltrees;

    // Copier les 4 premières valeurs car elles sont considérées comme non aberrantes
    for (size_t i = 0; i < valeurs.size() && i < NB_VALEURS_POUR_VERIFICATION; ++i) {
        valeursFiltrees.push_back(valeurs[i]);
    }

    // Traitement des valeurs à partir de la 5ème
    for (size_t i = NB_VALEURS_POUR_VERIFICATION; i < valeurs.size(); ++i) {
        float somme = 0.0;
        // Calculer la moyenne des 4 valeurs précédentes
        for (size_t j = i - NB_VALEURS_POUR_VERIFICATION; j < i; ++j) {
            somme += valeursFiltrees[j];
        }
        float moyennePrecedente = somme / NB_VALEURS_POUR_VERIFICATION;

        // Vérifier si la valeur actuelle est aberrante
        if (abs(valeurs[i] - moyennePrecedente) <= ecartMax) {
            valeursFiltrees.push_back(valeurs[i]);
        }
    }
    return valeursFiltrees;
}

void displayQRCode(int x0, int y0,const char* data) {
  // Générer le QR code
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, 0, data);

  // Afficher le QR code
  display.clear();
  for (int y = 0; y < qrcode.size; y++) {
    for (int x = 0; x < qrcode.size; x++) {
      if (qrcode_getModule(&qrcode, x, y)) {
        display.setPixel(x + x0, y + y0); // Ajouter x0 et y0 aux coordonnées
      }
    }
  }
}

void reinitialiserCompteur()
{
    timerEcran = millis();
}

void IRAM_ATTR extinctionDuSysteme()
{
    etatESP = 0;
}

uint64_t nbSecondesJsqSommeil()
{
    int heure = HEURE_ARRET.substring(0,2).toInt();
    int minute = HEURE_ARRET.substring(3,5).toInt();

    // Obtenir l'heure actuelle
    time_t now = time(nullptr);
    tm *now_tm = localtime(&now);

    // Construire l'heure cible pour aujourd'hui
    tm target_tm = *now_tm;
    target_tm.tm_hour = heure;
    target_tm.tm_min = minute;
    target_tm.tm_sec = 0;

    // Convertir les deux temps en time_t pour le calcul
    time_t now_t = mktime(now_tm);
    time_t target_t = mktime(&target_tm);

    // Calculer la différence
    uint64_t diffSeconds = target_t - now_t;

    // Si l'heure cible est déjà passée aujourd'hui, ajouter 24 heures
    if (diffSeconds < 0) {
        diffSeconds += 24 * 3600;
    }

    return diffSeconds;
}

uint64_t nbSecondesSommeil() {
    int heure1 = HEURE_ARRET.substring(0,2).toInt();
    int minute1 = HEURE_ARRET.substring(3,5).toInt();

    int heure2 = HEURE_DE_DEMARRAGE.substring(0,2).toInt();
    int minute2 = HEURE_DE_DEMARRAGE.substring(3,5).toInt();
    // Convertir les heures et minutes en secondes depuis minuit
    int totalSeconds1 = heure1 * 3600 + minute1 * 60;
    int totalSeconds2 = heure2 * 3600 + minute2 * 60;

    // Calculer la différence
    uint64_t difference = totalSeconds2 - totalSeconds1;

    // Si la différence est négative, cela signifie que heure2:minute2 est le jour suivant
    if (difference < 0) {
        difference += 24 * 3600; // Ajouter 24 heures en secondes
    }

    return difference;
}