#include "taskGestionBoutons.h"
#include <Arduino.h>
#include "../variablesGlobales.h"
#include "../Usuals/IHMActions.h"
#include <WS2812FX.h>

#define DELAI_APPUI 100

// initialisation de l'objet WS2812FX
WS2812FX led = WS2812FX(1, 18, NEO_RGB + NEO_KHZ800);

void IHM(void *parameter)
{
    // Initialisation des variables locales
    unsigned long tempsAppuiBlanc = 0;
    unsigned long tempsAppuiBleu = 0;
    bool detectionEnCours = false;
    unsigned long timerLED = 0;

    // initialisation de la LED
    led.init();
    led.start();

    // Message de bienvenue
    display.clear();
    display.drawString(((display.width() - display.getStringWidth("Bienvenue sur"))/2), 16, "Bienvenue sur");
    display.drawString(((display.width() - display.getStringWidth("Votre boitier"))/2), 32, "Votre boitier");
    display.drawString(((display.width() - display.getStringWidth("d'acquisition"))/2), 48, "d'acquisition");
    display.display();
    // --------------------

    // Boucle infinie de la tâche
    while (true)
    {   
        // Réduit la lumineusité de l'écran
        if(timerEcran != 0 && !lumineusiteReduite && timerEcran + (MISE_EN_VEILLE_ECRAN * 750) <= millis())
        {
            Serial.println("lumineusite reduite");
            display.setBrightness(1);
            lumineusiteReduite = true;
        }
        // Rétablie la lumineusité de l'écran
        else if(timerEcran != 0 && lumineusiteReduite && timerEcran + (MISE_EN_VEILLE_ECRAN * 750) > millis())
        {
            Serial.println("lumineusite normale");
            display.setBrightness(255);
            lumineusiteReduite = false;
        }
        // Entre en mode veille

        if(timerEcran != 0 && ecranAllume && timerEcran + (MISE_EN_VEILLE_ECRAN * 1000) <= millis())
        {
            Serial.println("display off");
            display.displayOff();
            ecranAllume = false;
            timerEcran = 0;
        }
        // Sort du mode veille
        else if(timerEcran != 0 && !ecranAllume)
        {
            Serial.println("display on");
            ecranAllume = true;
            display.displayOn();
        }

        // Si une alerte est en attente
        if(alert != 0)
        {   
            // alert 1 : triple clignotement bleu
            if(alert == 1)
            {
                if(timerLED == 0){
                    timerLED = millis() + 1000;
                    led.setBrightness(50);
                    led.setColor(0x0000FF);
                    led.setSpeed(200);
                    led.setMode(FX_MODE_BLINK);
                }
                else if(timerLED <= millis())
                {
                    timerLED = 0;
                    alert = 0;
                }
            }
        }
        // Si aucune alerte en attente affiche l'état de l'ESP
        else
        {
        if(etatESP == 0) { led.setBrightness(5); led.setColor(0x00FF00);display.displayOff();led.setMode(FX_MODE_STATIC);led.service();esp_deep_sleep_start();}  // rouge
        else if(etatESP == 1){ led.setBrightness(5); led.setColor(0x7FFF00);}  // orange
        else if(etatESP == 4){ led.setBrightness(5); led.setColor(0x00FFEA);}  // violet
        else { led.setBrightness(5); led.setColor(0xFF0000); }  // vert
        led.setMode(FX_MODE_STATIC);
        }
        // Affiche la couleur de la LED
        led.service();

        // Gestion des boutons
        bool etatActuelBlanc = digitalRead(BOUTON_BLANC_NBPIN);
        bool etatActuelBleu = digitalRead(BOUTON_BLEU_NBPIN);

        // Si la detection est autorisé
        if (detectionEnCours)
        {
            // Lance le timer en cas d'appui sur un bouton
            if (etatActuelBlanc == LOW && tempsAppuiBlanc == 0)
            {
                tempsAppuiBlanc = millis();
            }
            if (etatActuelBleu == LOW && tempsAppuiBleu == 0)
            {
                tempsAppuiBleu = millis();
            }

            // Si un timer atteint le délai d'appui et que l'autre bouton est appuyé
            if ((tempsAppuiBlanc != 0 && (millis() - tempsAppuiBlanc <= DELAI_APPUI) && etatActuelBleu == LOW) ||
            (tempsAppuiBleu != 0 && (millis() - tempsAppuiBleu <= DELAI_APPUI) && etatActuelBlanc == LOW)) {
                detectionEnCours = false;
                tempsAppuiBlanc = 0;
                tempsAppuiBleu = 0;
                //Action a réaliser quand les deux boutons sont appuyé
                actionBoutonBlancEtBleu();
            }

            // Si le timer du bouton blanc atteint le délai d'appui
            if (tempsAppuiBlanc != 0 && (millis() - tempsAppuiBlanc > DELAI_APPUI))
            {
                tempsAppuiBlanc = 0;
                detectionEnCours = false;
                //Action a réaliser quand le bouton BLANC est appuyé
                actionBoutonBlanc();
            }

            // Si le timer du bouton bleu atteint le délai d'appui
            if (tempsAppuiBleu != 0 && (millis() - tempsAppuiBleu > DELAI_APPUI))
            {
                tempsAppuiBleu = 0;
                detectionEnCours = false;
                //Action a réaliser quand le bouton BLEU est appuyé
                actionBoutonBleu();
            }
        }
        // Si aucun bouton n'est appuyé
        else if (etatActuelBlanc == HIGH && etatActuelBleu == HIGH)
        {
            detectionEnCours = true;
            tempsAppuiBlanc = 0;
            tempsAppuiBleu = 0;
        }

        // Attente de 10ms entre chaque detection
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
