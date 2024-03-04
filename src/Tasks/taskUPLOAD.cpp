#include "taskUPLOAD.h"
#include <Arduino.h>
#include <vector>
#include "Objet/DataCapture.h"
#include "../variablesGlobales.h"
#include "../Tasks/taskReconnexionWIFI.h"

void BOUCLE_ENVOIE(void *parameter)
{
    std::vector<DataCapture *> dernieresCaptures;
    const unsigned short int DELTA_ENTRE_CAPTURES = DUREE_CYCLE_DE_CAPTURES / NB_CAPTURES_PAR_CYCLE;
    // récupère le nombre de tick ecoullé depuis l'initialisation de freertos
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (true)
    {
        // Attend le temps nécessaire pour réaliser une nouvelle capture
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(DELTA_ENTRE_CAPTURES * 1000));
        // Capture des données via la création de l'objet DataCapture
        DataCapture *capture = new DataCapture;
        // Ajout de la capture dans le vecteur en attente de traitement
        dernieresCaptures.push_back(capture);
        if (dernieresCaptures.size() >= NB_CAPTURES_PAR_CYCLE)
        {
            DataCapture *CapturesVerifie = new DataCapture(&dernieresCaptures);
            // on fait appel a un fonction qui prend un dataCapture et fait les POST , retourne les POST qui n'on pas marché
            if (taskReconnexionWIFIHandle == NULL && CapturesVerifie->POST())
            {
                delete (CapturesVerifie);
                Serial.println("POST : OK");
            }
            else
            {
                // on demarre la task qui s'occupe de se reconnecter
                if (taskReconnexionWIFIHandle == NULL)
                {
                    xTaskCreate(
                        reconnexionWIFI,           // Fonction de la tâche
                        "reconnexionWIFI",         // Nom de la tâche
                        4096,                      // Taille de la pile de la tâche
                        NULL,                      // Paramètre passé à la tâche
                        1,                         // Priorité de la tâche
                        &taskReconnexionWIFIHandle // Handle de la tâche
                    );
                }

                // on introduit notre capture dans la file d'attente
                DataCapture::fileAttente.push(CapturesVerifie);
                Serial.print("Taille de la file d'attente : ");
                Serial.println(DataCapture::fileAttente.size());
            }
        }

        // Nettoyage (si nécessaire, en fonction de votre logique de traitement)
        // ...
    }
}
