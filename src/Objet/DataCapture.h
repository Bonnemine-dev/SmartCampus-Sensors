#ifndef DATACAPTURE_H
#define DATACAPTURE_H

#include <Arduino.h>
#include <stack>
#include "../Usuals/extractValues.h"
#include "../Usuals/Services.h"

struct DataCapture {
    static std::vector<float> tempVals, humVals, co2Vals;
    static std::stack<DataCapture*> fileAttente;
    String temperature;
    String humidite;
    String tauxCarbone;
    String date = ""; 
    int8_t error_code = 1;

    DataCapture();

    DataCapture(std::vector<DataCapture*>* dernieresCaptures);

    bool POST();

    void mise_en_attente();
};


#endif