#include "extractValues.h"
#include "../variablesGlobales.h"
#include "sensirion_common.h"
#include "sgp30.h"

String Humidite()
{
    
    String hum = String(dht.getHumidity(),1);
    if(hum == "nan")
    {
        if(etatESP != 4)etatESP = 1;
        else ancienEtatESP = 1;
        return "-1";
    }
    else return String(dht.getHumidity(),1);
}

String Temperature()
{
    String temp = String(dht.getTemperature(),1);
    if(temp == "nan")
    {
        if(etatESP != 4)etatESP = 1;
        else ancienEtatESP = 1;
        return "-1";
    }
    else return String(dht.getTemperature(),1);
}

String CO2()
{
    u16 co2_eq_ppm, tvoc_ppb;
    sgp_measure_iaq_blocking_read(&co2_eq_ppm, &tvoc_ppb);
    return String(400 + co2_eq_ppm);
}