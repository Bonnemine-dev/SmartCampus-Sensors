#include "IHMActions.h"
#include "../variablesGlobales.h"
#include "../Usuals/Services.h"
#include "../Usuals/icones.h"
#include "../Tasks/taskServerWeb.h"
#include "../Usuals/icones.h"

bool affichagePublic = true;

unsigned short int numeroAffichage = 1;
unsigned short int nombreSelectionne = 1;
unsigned short int numeroAnimation = 0;
String pinCode = "0000";

void actionBoutonBlanc()
{
    if (ecranAllume && !lumineusiteReduite)
    {
        if (affichagePublic)
        {
            numeroAffichage++;
            if(numeroAffichage == 4) numeroAffichage = 1;

            rafraichirAffichage();
        }
        else if(numeroAffichage == 4)
        {
            for (int i = 0; i < 4; i++)
            {
                if(pinCode[i] == '0')
                {
                    pinCode[i] = '0' + nombreSelectionne;
                    break;    
                }
            }
            if(pinCode[3] != '0')
            {
                if (pinCode == "1111")
                {
                    numeroAffichage = 5;
                    nombreSelectionne = 1;
                    pinCode = "0000";
                    Serial.println("server WEB lancement de la task");
                    if(taskServerWEBHandle == NULL)Serial.println(xTaskCreate(SERVER_WEB, "SERVER_WEB", 4096, NULL, 2, &taskServerWEBHandle));
                }
                else
                {
                    nombreSelectionne = 1;
                    pinCode = "0000";
                }
            }
            rafraichirAffichage();
        }
        else if(numeroAffichage == 6)
        {
            DataCapture *captureForce = new DataCapture();
            if(captureForce->POST())
            {
                numeroAffichage = 7;
                rafraichirAffichage();
            }
            else
            {
                numeroAffichage = 8;
                numeroAnimation = 0;
                if(taskanimationEchec == NULL)xTaskCreatePinnedToCore(animationEchec, "animationEchec", 1024, NULL, 1, &taskanimationEchec, 1);
            }
            delete captureForce;
        }
        else if(numeroAffichage == 7 || numeroAffichage == 8)
        {
            numeroAffichage = 5;
            rafraichirAffichage();
        }
    }
    else
    {
        reinitialiserCompteur();
    }
}

void actionBoutonBleu()
{
    if (ecranAllume && !lumineusiteReduite)
    {
        if (affichagePublic)
        {
            numeroAffichage--;
            if(numeroAffichage == 0) numeroAffichage = 3;

            rafraichirAffichage();
        }
        else
        {
            if (numeroAffichage == 4)
            {
                nombreSelectionne++;
                if (nombreSelectionne == 7)nombreSelectionne = 1;
                rafraichirAffichage();
            }
            else if(numeroAffichage == 5)
            {
                numeroAffichage = 6;
                rafraichirAffichage();
            }
            else if(numeroAffichage == 6 || numeroAffichage == 7 || numeroAffichage == 8)
            {
                numeroAffichage = 5;
                rafraichirAffichage();
            }
        }
    }
    else
    {
        reinitialiserCompteur();
    }
}

void actionBoutonBlancEtBleu()
{
    if (ecranAllume && !lumineusiteReduite)
    {
        affichagePublic = !affichagePublic;
        if(affichagePublic)
        {
            numeroAffichage = 1;
            nombreSelectionne = 1;
            pinCode = "0000";
            SERVER_WEB_OFF();
        }
        else numeroAffichage = 4;
        rafraichirAffichage();
    }
    else
    {
        reinitialiserCompteur();
    }
}

void rafraichirAffichage()
{
    if (numeroAffichage == 1)
    {
        display.clear();
        display.drawString(((display.width() - display.getStringWidth("Température"))/2), 0, "Température");
        display.setColor(BLACK);
        display.fillRect(44, 16, 1, 2);
        display.setColor(WHITE);
        display.drawXbm(0, 16, temp_width, temp_height, reinterpret_cast<const uint8_t*>(temp_bits)); // On affiche le logo température
        display.setFont(ArialMT_Plain_24);
        display.drawString(40, 28, Temperature() + "°C");
        display.setFont(ArialMT_Plain_16);
        display.display();
    }

    else if (numeroAffichage == 2)
    {
        display.clear();
        display.drawString(((display.width() - display.getStringWidth("Humidité"))/2), 0, "Humidité");
        display.drawXbm(0, 24, hum_width, hum_height, reinterpret_cast<const uint8_t*>(hum_bits)); // On affiche le logo température
        display.setFont(ArialMT_Plain_24);
        display.drawString(40, 28, Humidite() + "%");
        display.setFont(ArialMT_Plain_16);
        display.display();
    }

    else if (numeroAffichage == 3)
    {
        display.clear();
        display.drawString(((display.width() - display.getStringWidth("Taux CO2"))/2), 0, "Taux CO2");
        display.drawXbm(0, 16, co2_width, co2_height, reinterpret_cast<const uint8_t*>(co2_bits)); // On affiche le logo température
        display.setFont(ArialMT_Plain_24);
        display.drawString(50, 20, CO2());
        display.drawString(50, 40, "ppm");
        display.setFont(ArialMT_Plain_16);
        display.display();
    }

    else if (numeroAffichage == 4)
    {
        display.clear();
        display.drawRect(20,0,88,16);
        for (int i = 0; i < 4; i++)
        {
            if(pinCode[i] == '0')
            {
                display.drawString(40 + i * 12,0,"-");
            }
            else
            {
                display.drawString(40 + i * 12,2,"*");
            }
        }

        for (int i = 0; i <= 5; i++)
        {
            display.drawString(3 + i * 20,35,String(i + 1));
        }
        display.drawRect(3 + (nombreSelectionne - 1) * 19,34,18,21);

        display.display();
    }

    else if (numeroAffichage == 5)
    {
        display.clear();
        String qrDataString = String("WIFI:S:") + String(NOM_SA) + String(";T:WPA;P:") + String(AP_MOTDEPASSE) + String(";;");
        char* qrData = new char[qrDataString.length() + 1];     
        strcpy(qrData, qrDataString.c_str()); 

        displayQRCode(0 , 17 , qrData);
        delete[] qrData; 
        display.setFont(ArialMT_Plain_10);
        display.drawString(0,0,"Connectez-vous au réseau :");
        display.drawString(37 , 17 , String("SSID : ") + String(NOM_SA));
        display.drawString(37 , 36 , String("MDP : ") + String(AP_MOTDEPASSE));
        display.setFont(ArialMT_Plain_16);
        display.display();
    }

    else if (numeroAffichage == 6)
    {
        display.clear();
        display.drawString(((display.width() - display.getStringWidth("Bouton blanc"))/2), 16, "Bouton blanc");
        display.drawString(((display.width() - display.getStringWidth("pour envoyer"))/2), 32, "pour envoyer");
        display.drawString(((display.width() - display.getStringWidth("des données"))/2), 48, "des données");
        display.display();
    }

    else if (numeroAffichage == 7)
    {
        display.clear();
        display.drawString(((display.width() - display.getStringWidth("Données"))/2), 16, "");
        display.drawString(((display.width() - display.getStringWidth("envoyé"))/2), 32, "envoyé");
        display.drawString(((display.width() - display.getStringWidth("avec succès !"))/2), 48, "avec succès");
        display.display();
    }

    else if (numeroAffichage == 8  && numeroAnimation == 0)
    {
        display.clear();
        display.drawString(((display.width() - display.getStringWidth("Échec lors de"))/2), 16, "Échec lors de");
        display.drawString(((display.width() - display.getStringWidth("l'envoie des"))/2), 32, "l'envoie des");
        display.drawString(((display.width() - display.getStringWidth("données !"))/2), 48, "données");
        display.display();
    }

    else if (numeroAffichage == 8 && numeroAnimation == 1)
    {
        display.clear();
        display.drawString(((display.width() - display.getStringWidth("Vérifiez la"))/2), 16, "Vérifiez la");
        display.drawString(((display.width() - display.getStringWidth("configuration"))/2), 32, "configuration");
        display.drawString(((display.width() - display.getStringWidth("du boitier"))/2), 48, "du boitier");
        display.display();
    }
}

// Échec lors de l'envoie des données

void activeAffichagePointAcces()
{
    if(taskanimationEchec != NULL) {
        vTaskDelete(taskanimationEchec);
    }
    affichagePublic = false;
    numeroAffichage = 5;
    rafraichirAffichage();
}

void desactiveAffichagePointAcces()
{
    affichagePublic = true;
    numeroAffichage = 1;
    rafraichirAffichage();
}

void animationEchec(void *parameter)
{
    while (true)
    {
        if (numeroAffichage != 8)
            vTaskDelete(taskanimationEchec);
        else
        {
            numeroAnimation = numeroAnimation == 0 ? 1 : 0;
            rafraichirAffichage();
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
}
// Width: 16
// Height: 19