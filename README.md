# Système d'Acquisition de Données (DAQ) avec ESP32

Ce projet consiste en un système d'acquisition de données utilisant un microcontrôleur ESP32-2-Saola. L'objectif est de recueillir des données à partir de capteurs environnementaux, notamment un capteur d'humidité et de température DHT22, un capteur de qualité de l'air SGP30, et un capteur de présence BS312. Les données sont ensuite affichées localement sur un écran SSD1306.

## Fonctionnalités

- **Capteurs Intégrés :**
  - Capteur d'humidité et de température DHT22.
  - Capteur de qualité de l'air SGP30.
  - Capteur de présence BS312.

- **Affichage :**
  - Affichage des données sur un écran SSD1306.
  - Possibilité d'afficher les données sur un terminal si connecté à un IDE compatible.

- **Connectivité :**
  - Nécessite une connexion Wi-Fi pour récupérer la date et l'heure via le serveur NTP.
  - Configuration par défaut pour le réseau Eduroam de l'IUT Informatique. Les variables peuvent être modifiées au début du fichier `main.cpp` pour s'adapter à d'autres réseaux.

## Configuration

Avant de compiler et téléverser le code sur l'ESP32, assurez-vous de configurer les variables suivantes dans le fichier `main.cpp` :

- `ssid` : Nom du réseau Wi-Fi
- `EAP_IDENTITY` : Identifiant EAP
- `EAP_USERNAME` : Nom d'utilisateur EAP
- `EAP_PASSWORD` : Mot de passe EAP

## [Librairies](https://www.arduinolibraries.info/libraries) Utilisées

- **Librairie incluse depuis le code :**
    - DHTesp
    - Sensirion Common
    - SGP30
    - FreeRTOS
    - WS2812FX
    - WiFi
    - ESP-WPA2
    - Time
    - SSD1306Wire

- **Librairie incluse depuis le fichier *platformio.ini* :**
    - [DHT sensor library for ESPx](https://www.arduinolibraries.info/libraries/dht-sensor-library-for-es-px)
    - [Grove SPG30_Gas_Sensor](https://wiki.seeedstudio.com/Grove-VOC_and_eCO2_Gas_Sensor-SGP30/)
    - [WS2812FX](https://www.arduinolibraries.info/libraries/ws2812-fx)
    - [ESP8266 and ESP32 OLED driver for SSD1306 displays](https://www.arduinolibraries.info/libraries/esp8266-and-esp32-oled-driver-for-ssd1306-displays)

## Utilisation

1. Téléchargez et installez les librairies mentionnées ci-dessus dans votre IDE Arduino.
2. Configurez les variables du réseau Wi-Fi et les identifiants EAP au début du fichier `main.cpp`.
3. Compilez et téléversez le code sur l'ESP32-2-Saola.
4. Observation des données sur l'écran SSD1306 ou dans le terminal IDE.

**Remarque :** Pour une utilisation sans écran, commentez les sections liées à l'écran dans le code.

---

*Note : Assurez-vous de respecter les licences des librairies tierces utilisées dans ce projet.*

---

**Attention :** Si vous rencontré une erreur au moment de build le programme il vient surement d'un conflit de nommage ayant lieux entre deux librairie librairie, en effet les librairie WS2812FX et OLED driver for SSD1306 displays, possèdent des variables de même noms, notament les variable BLACK et WHITE, pour arranger cela il faut modifier les fichiers suivant : 
  - /sae3-l22-archi/SA-Projet-PIO/.pio/libdeps/esp32-s2-saola-1/WS2812FX/src/WS2812FX.h
  - /sae3-l22-archi/SA-Projet-PIO/.pio/libdeps/esp32-s2-saola-1/WS2812FX/src/WS2812FX.cpp
  - /sae3-l22-archi/SA-Projet-PIO/.pio/libdeps/esp32-s2-saola-1/WS2812FX/src/modes.cpp
