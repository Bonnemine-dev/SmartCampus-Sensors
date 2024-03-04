#include "taskReconnexionWIFI.h"
#include "../variablesGlobales.h"
#include "../Usuals/Services.h"
#include "../Objet/DataCapture.h"

void reconnexionWIFI(void *parameter) {
    // Boucle infinie de la tâche
    while (true) {
        // Deconnexion forcé du réseau
        WiFi.disconnect();

        // Active et désactive le mode station
        WiFi.enableSTA(false);
        WiFi.enableSTA(true);

        // Reconnexion au réseau
        Serial.println("Reconnexion au réseau en cours");

        // Boucle de reconnexion
        while (WiFi.status() != WL_CONNECTED) {

            // Tante une reconnexion
            WiFi.reconnect();

            // Attend un certain temps avant de retenter une reconnexion
            vTaskDelay(TEMPS_ENTRE_CHAQUE_TANTATIVE_DE_RECONNEXION * 1000 / portTICK_PERIOD_MS);
        }
        // Connexion au réseau réussie
        Serial.println(String("\nConnexion au réseau ") + WIFI_SSID + " rétablie avec succès");

        // Boucle qui réalise tous les POST en attente
        bool tousPostReussis = true;
        while(!DataCapture::fileAttente.empty()){
            if (!DataCapture::fileAttente.top()->POST()) {
                tousPostReussis = false;
                break; // Sortir de la boucle si POST() échoue
            }
            // Vide la file d'attente au fur et à mesure
            DataCapture::fileAttente.pop();
        }

        // Si tous les POST en attente ont été faits, sortir de la boucle infinie
        if (tousPostReussis) {
            Serial.println("Tous les POST en attente ont été faits !");
            break; // Sortir de la boucle principale si tous les POST réussissent
        } 
        // Sinon, recommencer la boucle principale
        else {
            Serial.println("Un POST a échoué, nouvelle tentative...");
            // Pas de break ici, la boucle principale va recommencer
        }
    }

    // Suppression de la tâche, et redémarrage du programme
    Serial.println("Fin de la tâche de reconnexion au réseau");
    taskReconnexionWIFIHandle = NULL;
    ESP.restart();
    vTaskDelete(NULL); // Autosuppression de la tâche
}

