#include "DataCapture.h"
#include "../variablesGlobales.h"
#include "HTTPClient.h"
#include <WiFiClientSecure.h>

// Définition des variables statiques
std::vector<float> DataCapture::tempVals, DataCapture::humVals, DataCapture::co2Vals;
std::stack<DataCapture*> DataCapture::fileAttente;

// Définition du constructeur par défaut
DataCapture::DataCapture()
{
    if(etatESP != 4)etatESP = -1;
    temperature = Temperature();
    humidite = Humidite();
    tauxCarbone = CO2();
    date = dateActuelle();
    // Initialiser les autres membres si nécessaire
}

// Définition du constructeur avec paramètres
// paramètres : vecteur de pointeurs vers les dernières captures
DataCapture::DataCapture(std::vector<DataCapture*>* dernieresCaptures)
{
    // Remplir les vecteurs avec les valeurs initiales
    for (DataCapture* capture : *dernieresCaptures) {
        tempVals.push_back(capture->temperature.toFloat());
        humVals.push_back(capture->humidite.toFloat());
        co2Vals.push_back(capture->tauxCarbone.toFloat());
    }

    // Filtrer les valeurs aberrantes
    if(tempVals.size() > NB_VALEURS_POUR_VERIFICATION)tempVals = filtreAberrantes(tempVals, TEMPERATURE_GAP_MAX); // 5°C d'écart pour la température
    if(humVals.size() > NB_VALEURS_POUR_VERIFICATION)humVals = filtreAberrantes(humVals, HUMIDITE_GAP_MAX);  // 15% d'écart pour l'humidité
    if(co2Vals.size() > NB_VALEURS_POUR_VERIFICATION)co2Vals = filtreAberrantes(co2Vals, TAUXCARBONE_GAP_MAX); // 600 ppm d'écart pour le CO2

    // Calculer les moyennes finales
    temperature = String(calculerMoyenne(tempVals),1);
    humidite = String(calculerMoyenne(humVals),1);
    tauxCarbone = String(calculerMoyenne(co2Vals),0);

    //on ajoute un repère dans le temps aux données
    date = dateActuelle();

    // Supprimer tous les objets dans le vecteur et vider le vecteur
    for (DataCapture* capture : *dernieresCaptures) {
        delete capture; // Supprime l'objet pointé par capture
    }
    dernieresCaptures->clear(); // Vide le vecteur

    // Garder uniquement les 4 dernière valeurs, pour vérifier les valeurs aberrantes
    if (tempVals.size() > NB_VALEURS_POUR_VERIFICATION)tempVals.erase(tempVals.begin(), tempVals.end() - NB_VALEURS_POUR_VERIFICATION);
    if (humVals.size() > NB_VALEURS_POUR_VERIFICATION)humVals.erase(humVals.begin(), humVals.end() - NB_VALEURS_POUR_VERIFICATION);
    if (co2Vals.size() > NB_VALEURS_POUR_VERIFICATION)co2Vals.erase(co2Vals.begin(), co2Vals.end() - NB_VALEURS_POUR_VERIFICATION);
}

bool DataCapture::POST()
{
    // Serial.println(heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    // initialisation de la requête HTTP POST vers l'API

    // Creation des corps de requête
    const String postData[3] = {
        "{\"nom\":\"temp\",\"valeur\":\"" + temperature + "\",\"dateCapture\":\"" + date + "\",\"localisation\":\"" + NOM_SALLE + "\",\"description\":\"\",\"nomsa\":\"" + NOM_SA + "\"}",
        "{\"nom\":\"hum\",\"valeur\":\""  + humidite    + "\",\"dateCapture\":\"" + date + "\",\"localisation\":\"" + NOM_SALLE + "\",\"description\":\"\",\"nomsa\":\"" + NOM_SA + "\"}",
        "{\"nom\":\"co2\",\"valeur\":\""  + tauxCarbone + "\",\"dateCapture\":\"" + date + "\",\"localisation\":\"" + NOM_SALLE + "\",\"description\":\"\",\"nomsa\":\"" + NOM_SA + "\"}"
        };
    
    // Envoi des requêtes
    for (size_t i = 0; i < 3; i++)
    {
        int httpCode = http.POST(postData[i]);
        String payload = http.getString();
        Serial.print(httpCode);
        Serial.println(" " + payload);
        // Vérification de la réponse
        if (httpCode != HTTP_CODE_CREATED)
        {
            Serial.print("Erreur lors de l'envoi de la requête, code : ");
            Serial.println(httpCode);
            return false;
        }
    }
    // Lancement de  l'alerte
    alert = 1;
    return true;
}
