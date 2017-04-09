/*
 * @file
 * @author Vsevolod (Seva) Ivanov
 * @copyright Copyright 2017 Vsevolod (Seva) Ivanov. All rights reserved.
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
#define TCA_OFFSET 3
#define NUMBER_OF_BNOS 5

int BNO_SWITCH_RATE_MS = 50;
int BNO_SAMPLE_RATE_MS = 0;

// unique id = bno id on i2c multiplex
Adafruit_BNO055 bno7 = Adafruit_BNO055(7); // highest
Adafruit_BNO055 bno6 = Adafruit_BNO055(6);
Adafruit_BNO055 bno5 = Adafruit_BNO055(5);
Adafruit_BNO055 bno4 = Adafruit_BNO055(4);
Adafruit_BNO055 bno3 = Adafruit_BNO055(3); // lowest

// lowest to highest transmission ids order
Adafruit_BNO055 *bno_sensors[NUMBER_OF_BNOS] = {
  &bno3,&bno4, &bno5, &bno6, &bno7};

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

    Serial.println();
    for (uint8_t t = 0; t < 8; t++)
    {
        tcaselect(t);
        Serial.print("TCA port #"); Serial.print(t);

        for (int addr = 0; addr <= 127; addr++)
        {
            if (addr == TCAADDR)
                continue;

            uint8_t data;

            if (!twi_writeTo(addr, &data, 0, 1, 1))
            {
                Serial.print(" -> Found I2C 0x");
                Serial.print(addr, HEX);
            }
        }
        Serial.println();
    }
}

void find_failed_bnos()
{
    for (uint8_t i = 0; i < NUMBER_OF_BNOS; i++)
    {
        sensor_t bno;
        bno_sensors[i]->getSensor(&bno);
        
        tcaselect(bno.sensor_id);

        if (!bno_sensors[i]->begin())
            Serial.println(
              "ERROR: Cannot detect bno" + String(i + 3));
        //else
          //sensor_details(&bno);
    }
}

void bnos_details()
{
    for (uint8_t i = 0; i < NUMBER_OF_BNOS; i++)
    {
        sensor_t bno;
        bno_sensors[i]->getSensor(&bno);
        tcaselect(bno.sensor_id);

        if (bno_sensors[i]->begin())
          sensor_details(&bno);
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
    find_bnos();
    Serial.println();
    find_failed_bnos();
}

void loop(void)
{
    for (int i = 0; i < NUMBER_OF_BNOS; i++)
    {
        // select bno on the tca multiplexer
        tcaselect(i + TCA_OFFSET);

        // lowest id == 0
        String id = String(i, DEC),
                x = "x", y = "y", z = "z";

        imu::Vector<3> euler = bno_sensors[i]->getVector(
            Adafruit_BNO055::VECTOR_EULER
        );
        /* dtostrf:
         *    double -> char conversion
         *      with width of 2 and
         *      a precision of 4 (number of digits after the dicimal sign)
        */
        char byte_buff[512];

        x.concat(dtostrf(euler.x(), 2, 4, byte_buff));
        y.concat(dtostrf(euler.y(), 2, 4, byte_buff));
        z.concat(dtostrf(euler.z(), 2, 4, byte_buff));

        String segment = "bno" + id + x + y + z + "end" + id + "$\n";
        Serial.write(segment.c_str());

        delay(BNO_SWITCH_RATE_MS);
    }
    delay(BNO_SAMPLE_RATE_MS);
}
