#include "taskServerWeb.h"
#include "../variablesGlobales.h"
#include <DNSServer.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include "AsyncTCP.h"
#include "../Usuals/Services.h"
#include "../Objet/DataCapture.h"
#include "../Usuals/IHMActions.h"

DNSServer dnsServer;
AsyncWebServer server(80);
String entete = "";
bool doitRedemarrer;

String formConstructeur() {
return 
    "<!DOCTYPE html>"
    "<html lang=\"en\">"
    "<head>"
    "    <meta charset=\"UTF-8\">"
    "    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">"
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "    <title>Configuration</title>"
    "</head>"
    "<body>"
    "<style>"
    "    @import url('https://fonts.googleapis.com/css?family=Montserrat:400,700|Roboto:400,700');"
    "    * {margin: 0;padding: 0;box-sizing: border-box;}"
    "    body {font-family: 'Montserrat', 'Roboto', Tahoma, Verdana, sans-serif;background: linear-gradient(90deg, #00B292, #7ED957);display: flex;flex-direction: column;justify-content: center;align-items: center;color: #FFF;padding: 30px 20px;}"
    "    h1 {font-size: 30px;margin-top: 20px;}"
    "    .form {background-color: #5D5D5D;padding: 20px;margin-top: 30px;border-radius: 10px;}"
    "    .form form { display: flex;flex-direction: column;align-items: flex-start;}"
    "    .form form label {font-size: 15px;margin-top: 15px;}"
    "    .form form label:first-child {margin-top: 0;}"
    "    .form form input[type=submit] {background: linear-gradient(90deg, #00B292, #7ED957);border: none;padding: 10px 20px;border-radius: 1000px;color: #FFF;font-size: 15px;margin-top: 20px;cursor: pointer;}"
    "    .form form input[type=submit]:hover {transform: scale(1.1);border: 1px solid #FFF;}"
    "    .form form input[type=text],"
    "    .form form input[type=url],"
    "    .form form input[type=password],"
    "    .form form input[type=time],"
    "    .form form select {background-color: #FFF;border: 1px solid #BABABA;padding: 5px 10px;border-radius: 1000px;font-size: 15px;margin-top: 5px;}"
    "    .form form input[type=url] {width: 100%;}"
    "</style>"
    "<h1>Configuration du boitier d'acquisition</h1>"
    "<p class=\"Error_message\" style=\"color : red;\">"+entete+"</p>"
    "<div class=\"form\">"
    "    <form action=\"/submit\" method=\"post\">"
    "        <label for=\"ssid\">SSID :</label>"
    "        <input type=\"text\" id=\"ssid\" name=\"ssid\" value=\""+WIFI_SSID+"\">"
    "        <label for=\"security\">Securité :</label>"
    "         <select id=\"security\" name=\"security\" onchange=\"toggleUsernameField()\">"
    "            <option value=0>WPA-Personal</option>"
    "            <option value=0>WPA2-Personal</option>"
    "            <option value=1>WPA2-Enterprise</option>"
    "            <option value=0>WPA3-Personal</option>"
    "            <option value=1>WPA3-Enterprise</option>"
    "        </select>"
    "        <div id=\"usernameField\" style=\"display:none;\">"
    "            <label for=\"username\">Nom d'utilisateur:</label>"
    "            <input type=\"text\" id=\"username\" name=\"username\" value=\""+WIFI_IDENTIFIANT+"\">"
    "        </div>"
    "        <label for=\"password\">Mot de passe du WiFi :</label>"
    "        <input type=\"password\" id=\"password\" name=\"password\" value=\""+WIFI_MOTDEPASSE+"\">"
    "        <label for=\"saName\">Nom du SA (entre 7 et 15 caractères) :</label>"
    "        <input type=\"text\" id=\"saName\" name=\"saName\" minlength=\"7\" maxlength=\"15\" value=\""+NOM_SA+"\">"
    "        <label for=\"roomName\">Nom de la salle :</label>"
    "        <input type=\"text\" minlength=\"2\" minlength=\"15\" id=\"roomName\" name=\"roomName\" value=\""+NOM_SALLE+"\">"
    "        <label for=\"heure_demarrage\">Heure de démarrage du boitier :</label>"
    "        <input type=\"time\" id=\"heure_demarrage\" name=\"heure_demarrage\" value=\"" + HEURE_DE_DEMARRAGE + "\">"
    "        <label for=\"heure_arret\">Heure d'extinction du boitier :</label>"
    "        <input type=\"time\" id=\"heure_arret\" name=\"heure_arret\" value=\"" + HEURE_ARRET + "\">"
    "        <label for=\"interval\">Temps entre chaque envoi (minutes) :</label>"
    "        <input type=\"range\" id=\"interval\" name=\"interval\" min=\"5\" step=\"5\" max=\"60\" value=\"" + String(DUREE_CYCLE_DE_CAPTURES / 60) + "\" oninput=\"updateTimeCycle(this.value)\">"
    "        <p><span id=\"timeCycleDisplay\">" + String(DUREE_CYCLE_DE_CAPTURES / 60) + "</span> minutes.</p>"
    "        <label for=\"numCaptures\">Nombre de captures (min 2 - max 15) :</label>"
    "        <input type=\"range\" id=\"numCaptures\" name=\"numCaptures\" min=\"2\" max=\"15\" value=\"" + String(NB_CAPTURES_PAR_CYCLE) + "\" oninput=\"updateNumCapture(this.value)\">"
    "        <p><span id=\"numCaptureDisplay\">" + String(NB_CAPTURES_PAR_CYCLE) + "</span> capture(s) par cycle.</p>"
    "        <label for=\"reconnectTime\">Temps entre chaque tentative de reconnexion (secondes) :</label>"
    "        <input type=\"range\" id=\"reconnectTime\" name=\"reconnectTime\" step=\"10\" min=\"10\" max=\"300\" value=\"" + String(TEMPS_ENTRE_CHAQUE_TANTATIVE_DE_RECONNEXION) + "\" oninput=\"updateReconnectTime(this.value)\">"
    "        <p><span id=\"reconnectTimeDisplay\">" + String(TEMPS_ENTRE_CHAQUE_TANTATIVE_DE_RECONNEXION) + "</span> secondes</p>"
    "        <label for=\"apiUrl\">URL de l'API :</label>"
    "        <input type=\"url\" id=\"apiUrl\" name=\"apiUrl\" value=\"" + String(URL_API) + "\">"
    "        <input type=\"submit\" value=\"Enregistrer\" class=\"button\">"
    "    </form>"
    "</div>"
    "<script>"
    "     function updateNumCapture(value) {"
    "        document.getElementById('numCaptureDisplay').innerText = value;"
    "    }"
    "     function updateTimeCycle(value) {"
    "        document.getElementById('timeCycleDisplay').innerText = value;"
    "    }"
    "     function updateReconnectTime(value) {"
    "        document.getElementById('reconnectTimeDisplay').innerText = value;"
    "    }"
    "     function toggleUsernameField() {"
    "            var securityType = document.getElementById(\"security\").value;"
    "            var usernameField = document.getElementById(\"usernameField\");"
    "            if (securityType === \"1\") {"
    "                usernameField.style.display = \"block\";"
    "            } else {"
    "                usernameField.style.display = \"none\";"
    "            }"
    "        }"
    "</script>"
    "</body>"
    "</html>";
}

const String pageDeCheck = 
"<!DOCTYPE html>"
    "<html lang=\"en\">"
    "<head>"
    "    <meta charset=\"UTF-8\">"
    "    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">"
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "    <title>Terminé !</title>"
    "</head>"
    "<body>"
    "<style>"
    "    @import url('https://fonts.googleapis.com/css?family=Montserrat:400,700|Roboto:400,700');"
    "    * {margin: 0;padding: 0;box-sizing: border-box;}"
    "    body {font-family: 'Montserrat', 'Roboto', Tahoma, Verdana, sans-serif;background: linear-gradient(90deg, #00B292, #7ED957);display: flex;flex-direction: column;justify-content: center;align-items: center;color: #FFF;padding: 30px 20px;}"
    "    .window { display: flex;flex-direction: column;align-items: center;background-color: #5D5D5D;padding: 20px;margin-top: 30px;border-radius: 10%;}"
    "    .window h1 {font-size: 30px;margin-top: 20px;}"
    "    .window h2 {font-size: 20px;margin-top: 5px;}"
    "    .window form { display: flex;flex-direction: column;align-items: flex-start;}"
    "    .window form input[type=submit] {background: linear-gradient(90deg, #00B292, #7ED957);border: none;padding: 10px 20px;border-radius: 1000px;color: #FFF;font-size: 15px;margin-top: 20px;cursor: pointer;}"
    "    .window form input[type=submit]:hover {transform: scale(1.1);border: 1px solid #FFF;}"
    "</style>"
    "<body>"
    "<div class=\"window\">"
    "<h1>Configuration terminé</h1>"
    "<h2>Redémarrer votre boitier</h2>"
    "<form action=\"/shutdown\"><input type=\"submit\" value=\"Redémarrer le boitier\" /></form>"
    "</div>"
    "</body>"
    "</html>";

const String pageDeFin = 
"<!DOCTYPE html>"
    "<html lang=\"en\">"
    "<head>"
    "    <meta charset=\"UTF-8\">"
    "    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">"
    "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
    "    <title>Terminé !</title>"
    "</head>"
    "<body>"
    "<style>"
    "    @import url('https://fonts.googleapis.com/css?family=Montserrat:400,700|Roboto:400,700');"
    "    * {margin: 0;padding: 0;box-sizing: border-box;}"
    "    body {font-family: 'Montserrat', 'Roboto', Tahoma, Verdana, sans-serif;background: linear-gradient(90deg, #00B292, #7ED957);display: flex;flex-direction: column;justify-content: center;align-items: center;color: #FFF;padding: 30px 20px;}"
    "    h1 {font-size: 30px;margin-top: 20px;}"
    "</style>"
    "<body>"
    "<h1>Redémarrage en cours, vous pouvez fermer la fenêtre</h1>"
    "</body>"
    "</html>";

class CaptiveRequestHandler : public AsyncWebHandler
{
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request)
  {
    // request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request)
  {
    Serial.println("Envoie du formulaire");
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    entete = "";
    response->print(formConstructeur());
    request->send(response);
  }
};

void SERVER_WEB(void *parameter)
{
  Serial.println("server WEB setup");
  doitRedemarrer = false;
  // Création du point d'accès
  WiFi.softAP(NOM_SA, AP_MOTDEPASSE);

  // Configuration des adresses IP
  IPAddress local_IP(4, 3, 2, 1);
  IPAddress gateway(4, 3, 2, 1);
  IPAddress subnet(255, 255, 255, 0);
  // -----------------------------

  // Configuration du point d'accès avec l'IP désirée
  WiFi.softAPConfig(local_IP, gateway, subnet);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    Serial.println("Envoie du formulaire");
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print(formConstructeur());
    request->send(response); });

  server.on("/check", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print(pageDeCheck);
    request->send(response); });

  server.on("/shutdown", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print(pageDeFin);
    request->send(response);
    SERVER_WEB_OFF();
    });

  server.onNotFound([](AsyncWebServerRequest *request)
                    {
    Serial.println("Envoie du formulaire");
    AsyncResponseStream *response = request->beginResponseStream("text/html");
    response->print(formConstructeur());
    request->send(response); });

  // Gestion de la soumission du formulaire
  server.on("/submit", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              // Initialisation de tous les paramètres retournés par le formulaire
              doitRedemarrer = true;
              String ssid, username, password, saName, roomName, interval, numCaptures, reconnectTime, apiUrl, heure_demarrage, heure_arret;
              bool security = false;
              // Définition de tous les paramètres
              if (request->hasParam("ssid", true))
                ssid = request->getParam("ssid", true)->value();
              if (request->hasParam("username", true))
                username = request->getParam("username", true)->value();
              if (request->hasParam("password", true))
                password = request->getParam("password", true)->value();
              if (request->hasParam("saName", true))
                saName = request->getParam("saName", true)->value();
              if (request->hasParam("roomName", true))
                roomName = request->getParam("roomName", true)->value();
              if (request->hasParam("interval", true))
                interval = request->getParam("interval", true)->value();
              if (request->hasParam("numCaptures", true))
                numCaptures = request->getParam("numCaptures", true)->value();
              if (request->hasParam("reconnectTime", true))
                reconnectTime = request->getParam("reconnectTime", true)->value();
              if (request->hasParam("apiUrl", true))
                apiUrl = request->getParam("apiUrl", true)->value();
              if (request->hasParam("security", true))
                security = (request->getParam("security", true)->value() == "1") ? true : false;
              if (request->hasParam("heure_demarrage", true))
                heure_demarrage = (request->getParam("heure_demarrage", true)->value());
              if (request->hasParam("heure_arret", true))
                heure_arret = (request->getParam("heure_arret", true)->value());


              Serial.println("SSID: " + ssid + " Security: " + security + " Username: " + username + " Password: " + password + " SA Name: " + saName + " Room Name: " + roomName + " Interval: " + interval + " Number of Captures: " + numCaptures + " Reconnect Time: " + reconnectTime + " API URL: " + apiUrl + " Heure de démarrage: " + heure_demarrage + " Heure d'arrêt: " + heure_arret);

              bool validForm = true;

              if (saName.length() < 7 || saName.length() > 15)
              {
                entete += "Le nom du SA doit être compris entre 7 et 15 caractères<br>";
                validForm = false;
              }
              if (roomName.length() < 2 || roomName.length() > 15)
              {
                entete += "Le nom de la salle doit être compris entre 2 et 15 caractères<br>";
                validForm = false;
              }
              if (interval.toInt() < 5 || interval.toInt() > 60)
              {
                entete += "Le temps entre chaque envoie doit être compris entre 5 et 60 minutes<br>";
                validForm = false;
              }
              if (numCaptures.toInt() < 2 || numCaptures.toInt() > 15)
              {
                entete += "Le nombre de captures doit être compris entre 2 et 15<br>";
                validForm = false;
              }
              if (reconnectTime.toInt() < 10 || reconnectTime.toInt() > 300)
              {
                entete += "Le temps entre chaque tentative de reconnexion doit être compris entre 10 et 300 secondes<br>";
                validForm = false;
              }
              if (ssid != WIFI_SSID || password != WIFI_MOTDEPASSE)
              {
                if (!initWiFi(ssid, security, password, username))
                {
                  entete = "Impossible de se connecter à ce réseau<br>";
                  validForm = false;
                }
              }
              if (apiUrl != URL_API)
              {
                String ancienneURL = URL_API;
                URL_API = apiUrl;
                DataCapture *captureTest = new DataCapture;
                if (!captureTest->POST())
                {
                  URL_API = ancienneURL;
                  entete += "Impossible de joindre l'API vérifier les informations<br>";
                  validForm = false;
                }
              }
              if (validForm)
              {
                // Ouvrir les préférences pour enregistrer les configurations l'écriture
                configuration.begin("config", false);
                NOM_SA = saName;
                configuration.putString("NOM_SA", NOM_SA);
                NOM_SALLE = roomName;
                configuration.putString("NOM_SALLE", NOM_SALLE);
                DUREE_CYCLE_DE_CAPTURES = interval.toInt() * 60;
                configuration.putUShort("D_C_D_C", DUREE_CYCLE_DE_CAPTURES);
                HEURE_DE_DEMARRAGE = heure_demarrage;
                configuration.putString("H_D_D", HEURE_DE_DEMARRAGE);
                HEURE_ARRET = heure_arret;
                configuration.putString("H_A", HEURE_ARRET);
                NB_CAPTURES_PAR_CYCLE = numCaptures.toInt();
                configuration.putUShort("NB_CAP_CYCLE", NB_CAPTURES_PAR_CYCLE);
                TEMPS_ENTRE_CHAQUE_TANTATIVE_DE_RECONNEXION = reconnectTime.toInt();
                configuration.putUShort("T_E_C_T_D_R", TEMPS_ENTRE_CHAQUE_TANTATIVE_DE_RECONNEXION);
                configuration.putString("URL_API", URL_API);
                configuration.putString("WIFI_SSID", WIFI_SSID);
                configuration.putString("WIFI_MDP", WIFI_MOTDEPASSE);
                configuration.putString("WIFI_ID", WIFI_IDENTIFIANT);
                configuration.putBool("WIFI_S_T", WIFI_SECURITY_TYPE);
                configuration.end(); // Ferme les préférences
                request->redirect("/check");
              }
              else
              {
                request->redirect("/");
              }
              // Page réponses
            });
  // Démarrage du server DNS, Le serveur DNS est configuré pour resoudre tous les noms de dommaine à l'adresse du point d'accès (Captive Portal)
  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); // only when requested from AP

  // Démarrage du server WEB
  server.begin();
  Serial.println("server WEB on");
  ancienEtatESP = etatESP;
  etatESP = 4;

  // Préparation de l'écran LCD
  activeAffichagePointAcces();
  // Boucle qui permet d'intercepter toutes les requêtes HTTP

  while (true)
  {
    dnsServer.processNextRequest();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void SERVER_WEB_OFF()
{
  if (taskServerWEBHandle != NULL)
  {
    WiFi.softAPdisconnect(true);
    desactiveAffichagePointAcces();
    etatESP = ancienEtatESP;
    dnsServer.stop();
    server.end();
    Serial.println("server WEB OFF");
    vTaskDelete(taskServerWEBHandle);
    if(doitRedemarrer)ESP.restart();
  }
}