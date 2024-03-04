#ifndef SERVICES_H
#define SERVICES_H

#include <Arduino.h>
#include <vector>
#include "../Objet/DataCapture.h"

struct DataCapture;

bool initWiFi(String ssid,bool security, String motdepasse, String identifiant);

void dateInit();

String dateActuelle();

DataCapture dataVerifie(std::vector<DataCapture*>* dernieresCaptures);

float calculerMoyenne(const std::vector<float>& valeurs);

std::vector<float> filtreAberrantes(const std::vector<float>& valeurs, float ecartMax);

void displayQRCode(int startX, int startY,const char* data);

void reinitialiserCompteur();

void IRAM_ATTR extinctionDuSysteme();

uint64_t nbSecondesJsqSommeil();

uint64_t nbSecondesSommeil();

String formConstructeur();

#endif