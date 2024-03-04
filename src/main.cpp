#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "variablesGlobales.h"
#include "Usuals/extractValues.h"
#include "sensirion_common.h"
#include "sgp30.h"
#include "Usuals/Services.h"
#include "Objet/DataCapture.h"
#include "Tasks/taskUPLOAD.h"
#include "Tasks/taskGestionBoutons.h"
#include "Tasks/taskServerWEB.h"

// Définition des variables globales
DHTesp dht;
HTTPClient http;
TaskHandle_t taskReconnexionWIFIHandle = NULL;
TaskHandle_t taskServerWEBHandle = NULL;
TaskHandle_t taskanimationEchec = NULL;
TaskHandle_t taskGestionBoutonsHandle = NULL;
SSD1306Wire display(0x3C, SDA, SCL, GEOMETRY_128_64);
Preferences configuration;
String NOM_SA;
String NOM_SALLE;
bool WIFI_SECURITY_TYPE;
String WIFI_SSID;
String WIFI_MOTDEPASSE;
String WIFI_IDENTIFIANT;
unsigned short int DUREE_CYCLE_DE_CAPTURES;
unsigned short int NB_CAPTURES_PAR_CYCLE;
unsigned short int TEMPS_ENTRE_CHAQUE_TANTATIVE_DE_RECONNEXION;
String URL_API;
unsigned short int etatESP = -1;
unsigned short int ancienEtatESP = etatESP;
unsigned short int alert = 0;
bool ecranAllume = true, lumineusiteReduite = false;
unsigned long timerEcran = 1;
String HEURE_ARRET;
String HEURE_DE_DEMARRAGE;
// --------------------------------
hw_timer_t *timer_to_stop = NULL;

void setup()
{
  // put your setup code here, to run once:
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setBrightness(255);
  Serial.begin(VITESSECOMSERIAL);
  display.fillRect(0, 0, 14, 16);
  display.drawString(((display.width() - display.getStringWidth("Initialisation"))/2), 24, "Initialisation");
  display.drawString(((display.width() - display.getStringWidth("En cours"))/2), 40, "En cours...");
  display.display();

  configuration.begin("config", false); // Ouvre les préférences avec le namespace "config"

  // Lire les valeurs ou utiliser les valeurs par défaut
  NOM_SA = configuration.getString("NOM_SA", "ESP-");
  NOM_SALLE = configuration.getString("NOM_SALLE", "");
  WIFI_SECURITY_TYPE = configuration.getBool("WIFI_S_T", true);
  WIFI_SSID = configuration.getString("WIFI_SSID", "eduroam");
  WIFI_MOTDEPASSE = configuration.getString("WIFI_MDP", "w?qfbgpht4W-mx66");
  WIFI_IDENTIFIANT = configuration.getString("WIFI_ID", "rfreitas");
  DUREE_CYCLE_DE_CAPTURES = configuration.getUShort("D_C_D_C", 300);
  NB_CAPTURES_PAR_CYCLE = configuration.getUShort("NB_CAP_CYCLE", 4);
  TEMPS_ENTRE_CHAQUE_TANTATIVE_DE_RECONNEXION = configuration.getUShort("T_E_C_T_D_R", 60);
  URL_API = configuration.getString("URL_API", "https://sae34.k8s.iut-larochelle.fr/api/captures");
  HEURE_ARRET = configuration.getString("H_A", "20:00"); // HEURE_D_ARRET
  HEURE_DE_DEMARRAGE = configuration.getString("H_D_D", "07:30"); // HEURE_DE_DEMARRAGE

  configuration.end(); // Ferme les préférences

  Serial.println("NOM_SA : " + NOM_SA + "\nNOM_SALLE : " + NOM_SALLE + "\nWIFI_SECURITY_TYPE : " + String(WIFI_SECURITY_TYPE) + "\nWIFI_SSID : " + WIFI_SSID + "\nWIFI_MOTDEPASSE : " + WIFI_MOTDEPASSE + "\nWIFI_IDENTIFIANT : " + WIFI_IDENTIFIANT + "\nDUREE_CYCLE_DE_CAPTURES : " + String(DUREE_CYCLE_DE_CAPTURES) + "\nNB_CAPTURES_PAR_CYCLE : " + String(NB_CAPTURES_PAR_CYCLE) + "\nTEMPS_ENTRE_CHAQUE_TANTATIVE_DE_RECONNEXION : " + String(TEMPS_ENTRE_CHAQUE_TANTATIVE_DE_RECONNEXION) + "\nURL_API : " + URL_API + "\nHEURE_ARRET : " + HEURE_ARRET + "\nHEURE_DE_DEMARRAGE : " + HEURE_DE_DEMARRAGE);

  display.fillRect(16, 0, 14, 16);
  display.display();
  dht.setup(DHTPIN, DHTesp::AM2302);
  Serial.println("setup");
  if (sgp_probe() != STATUS_OK)
  {
    Serial.println("SGP30 non détecté, régler le problème avant de continuer");
    while (true);    
  }  
  if(dht.getStatusString() != "OK")
  {
    Serial.println("DHT non détecté, régler le problème avant de continuer");
    while (true);    
  }

  pinMode(DETECTEUR_DE_MOUVEMENT_PIN, INPUT_PULLUP); // Utilisation de INPUT_PULLUP si un bouton est connecté
  attachInterrupt(digitalPinToInterrupt(DETECTEUR_DE_MOUVEMENT_PIN), reinitialiserCompteur, RISING);

  display.fillRect(32, 0, 14, 16);
  display.display();

  if(!initWiFi(WIFI_SSID,WIFI_SECURITY_TYPE,WIFI_MOTDEPASSE,WIFI_IDENTIFIANT))
  {
      Serial.println("En attente d'une connexion WiFi pour continuer");
       while(WiFi.status() != WL_CONNECTED)
       {
           vTaskDelay( 1000 / portTICK_PERIOD_MS);
       }
  }

  display.fillRect(48, 0, 14, 16);
  display.fillRect(64, 0, 14, 16);
  display.display();

  dateInit();

    http.begin(URL_API);
    http.addHeader("connection", "keep-alive");
    http.addHeader("accept", VALEUR_ACCEPTER);
    http.addHeader("dbname", NOM_DB);
    http.addHeader("username", NOM_UTILISATEUR);
    http.addHeader("userpass", MOT_DE_PASSE);
    http.addHeader("Content-Type", VALEUR_ACCEPTER);

  display.fillRect(80, 0, 14, 16);
  display.display();

  timer_to_stop = timerBegin(0, 80, true);
  timerAttachInterrupt(timer_to_stop, &extinctionDuSysteme, true);
  timerAlarmWrite(timer_to_stop, nbSecondesJsqSommeil() * 1000000, true);
  timerAlarmEnable(timer_to_stop);
  esp_sleep_enable_timer_wakeup(nbSecondesSommeil() * 1000000);
  
  display.fillRect(96, 0, 14, 16);
  display.display();

  xTaskCreate(
      BOUCLE_ENVOIE,   // Fonction de la tâche
      "BOUCLE_ENVOIE", // Nom de la tâche
      4096,           // Taille de la pile (stack) de la tâche
      NULL,            // Paramètre passé à la tâche
      1,               // Priorité de la tâche
      NULL             // Handle de la tâche
  );

  display.fillRect(112, 0, 14, 16);
  display.display();

  pinMode(BOUTON_BLANC_NBPIN, INPUT_PULLUP);
  pinMode(BOUTON_BLEU_NBPIN, INPUT_PULLUP);
  
  xTaskCreate(
      IHM,   // Fonction de la tâche
      "IHM", // Nom de la tâche
      4096,           // Taille de la pile (stack) de la tâche
      NULL,            // Paramètre passé à la tâche
      1,               // Priorité de la tâche
      NULL             // Handle de la tâche
  );
  // Serial.println(heap_caps_get_free_size(MALLOC_CAP_INTERNAL)); 

  DataCapture test;
  test.POST();
}

void loop()
{
// Ne pas utiliser
}
