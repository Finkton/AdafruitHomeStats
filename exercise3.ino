// Adafruit IO Temperature & Humidity Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-temperature-and-humidity
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016-2017 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

/************************ Example Starts Here *******************************/
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// pin connected to DH22 data line
#define DATA_PIN 2

// create DHT22 instance
DHT_Unified dht(DATA_PIN, DHT11);

// set up the 'temperature', 'humidity', 'light' and 'string' feeds
AdafruitIO_Feed *temperature = io.feed("temperature");
AdafruitIO_Feed *humidity = io.feed("humidity");
AdafruitIO_Feed *light = io.feed("light");
AdafruitIO_Feed *string = io.feed("string");

// time tracker
unsigned long timeMillis = 0;

// averaging weight and value for measurements
double celsiusAvg = 0;
double humidityAvg = 0;
double lightAvg = 0;
double a = 0.000001;

void setup() {

  // start the serial connection
  Serial.begin(9600);

  // wait for serial monitor to open
  while(! Serial);

  // initialize dht22
  dht.begin();
  
  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  
  // start tracking time
  timeMillis = millis();
  
}

void loop() {
  
  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  sensors_event_t event;
  dht.temperature().getEvent(&event);

  double celsius = event.temperature; // read Temperature
  celsiusAvg = celsius *a + celsiusAvg *(1.0-a); // multiples the current value and adds it to the previous value.

  double humidityValue = dht.humidity().getEvent(&event); // read Humidity
  humidityAvg = humidityValue *a + humidityAvg *(1.0-a); // multiples the current value and adds it to the previous value.

  double lightValue = (double)analogRead(A0) / 1024.0; // read Light value
  lightAvg = lightValue *a + lightAvg *(1.0-a); // multiples the current value and adds it to the previous value.
  
  if( millis() - timeMillis > 3600000){ // 3600 seconds is one hour. 1000 milli seconds is one second.
    timeMillis = millis();

    // print celsius
    Serial.print("celsius: ");
    Serial.print(celsiusAvg);
    Serial.println("C");
    
    // save celsius to Adafruit IO
    temperature->save(celsiusAvg);

    // print humidity
    Serial.print("humidity: ");
    Serial.print(humidityAvg);
    Serial.println("%");
    // save humidity to Adafruit IO
    humidity->save(humidityAvg);

    // print light
    Serial.print("light: ");
    Serial.print(lightAvg);
    Serial.println("");
  
    // save light to Adafruit IO
    light->save(lightAvg);
    
    // save overall string
    string->save("\ncelsius: "+String(celsius)+",\n"+"humidity: "+String(event.relative_humidity)+",\n"+"light: "+String(lightValue)+"\n");
    
  }else if((double)millis() - (double)timeMillis < -1){ // this is if the millis loops back to 0, which normally happens after 50 days
    timeMillis = millis();
  }
}
