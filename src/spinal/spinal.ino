/*
 * @file
 * @author Vsevolod (Seva) Ivanov
*/

#include <stdlib.h> // dtostrf - float to char*

#include <Wire.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

#include <utility/imumaths.h>

extern "C" {
  // bus scanning - Wire lib
  #include "utility/twi.h"
}

#define TCAADDR 0x70

int BNO_SWITCH_RATE_MS = 100;
int BNO_SAMPLE_RATE_MS = 100 - BNO_SWITCH_RATE_MS;

// unique id = bno id on i2c multiplex
Adafruit_BNO055 bno7 = Adafruit_BNO055(7); // highest
Adafruit_BNO055 bno2 = Adafruit_BNO055(2);
Adafruit_BNO055 bno3 = Adafruit_BNO055(3);
Adafruit_BNO055 bno4 = Adafruit_BNO055(4);
Adafruit_BNO055 bno6 = Adafruit_BNO055(6); // lowest

// lowest to highest transmission ids order
Adafruit_BNO055 *bno_ids[5] = {&bno6, &bno4, &bno3, &bno2, &bno7};

void tcaselect(uint8_t i)
{
    if (i > 7)
        return;

    Wire.beginTransmission(TCAADDR);
    Wire.write(1 << i);
    Wire.endTransmission();
}

void find_bnos()
{
    Wire.begin();

    for (uint8_t t = 0; t < 8; t++)
    {
        tcaselect(t);
        Serial.print("TCA port #"); Serial.println(t);

        for (int addr = 0; addr<=127; addr++)
        {
            if (addr == TCAADDR)
                continue;

            uint8_t data;

            if (!twi_writeTo(addr, &data, 0, 1, 1))
            {
                Serial.print("Found I2C 0x");
                Serial.println(addr,HEX);
            }
        }
    }
}

void sensor_details(sensor_t *sensor)
{
    Serial.println("------------------------------------");
    Serial.println("Sensor:       " + String(sensor->name));
    Serial.println("Driver ver:   " + String(sensor->version));
    Serial.println("Unique id:    " + String(sensor->sensor_id));
    Serial.println("Max value:    " + String(sensor->max_value) + " deg");
    Serial.println("Min value:    " + String(sensor->min_value) + " deg");
    Serial.println("Resolution:   " + String(sensor->resolution) + " deg");
    Serial.println("------------------------------------\n");
    delay(500);
}

void setup(void)
{
    Serial.begin(9600);

    //find_bnos();

    for (uint8_t i = 0; i < 5; i++)
    {
        sensor_t bno;
        bno_ids[i]->getSensor(&bno);
        
        tcaselect(bno.sensor_id);

        if (!bno_ids[i]->begin())
        {
            while (1)
                Serial.println("Cannot detect bno" + char(i + 1));
        }
        //sensor_details(&bno);
    }
}

void loop(void)
{
    sensor_t bno;
    sensors_event_t event;
    
    for (int i = 0; i < 5; i++)
    {
        bno_ids[i]->getSensor(&bno);
        bno_ids[i]->getEvent(&event);
        
        tcaselect(bno.sensor_id);

        char byte_buff[512];
        String x = "x",
               y = "y",
               z = "z";
        
        /* double -> char conversion
         *      with width of 2 and
         *      a precision of 4 (number of digits after the dicimal sign)
        */
        x.concat(dtostrf(event.orientation.x, 2, 4, byte_buff));
        y.concat(dtostrf(event.orientation.y, 2, 4, byte_buff));
        z.concat(dtostrf(event.orientation.z, 2, 4, byte_buff));

        String segment = "bno" + String(i, DEC) + x + y + z + "$\n";

        Serial.write(&segment[0]);

        delay(BNO_SWITCH_RATE_MS);
    }
 
    delay(BNO_SAMPLE_RATE_MS);
}
