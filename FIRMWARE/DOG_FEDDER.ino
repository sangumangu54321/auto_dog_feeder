/************************* SHOURYA SETH "HACKCLUB MEMBER" ****************/
/**************************DOG FEEDER FOR #WOOF EVENT @ADV ***************/ 
/***************** THANK YOU SO MUCH TO GIVE ME THIS OPPTUNITY*************/

/************* BLYNK CONFIG *************/
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Dog Feeder"
#define BLYNK_AUTH_TOKEN "YOUR_AUTH_TOKEN"

/************* LIBRARIES *************/
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Servo.h>
#include "HX711.h"

/************* WIFI *************/
char ssid[] = "SSID";
char pass[] = "PS";

/************* SERVO *************/
#define SERVO_PIN 18
Servo feeder;

/************* HX711 *************/
#define HX_DT  19
#define HX_SCK 23
HX711 scale;

/************* LOAD CELL SETTINGS *************/
float calibration_factor = -7050;   //  ACCORING TO LOAD CELL
float emptyWeight = 275;            // grams (empty bowl)
float fullWeight  = 570;           // grams (full bowl)

/************* FOOD CONTROL *************/
int feedCount = 1;                  // V1 (1–4 feeds)
int foodLevel = 0;                  

/************* READ FOOD LEVEL *************/
void updateFoodLevel() {
   float weight = scale.get_units(5);
  if (weight < emptyWeight) weight = emptyWeight;
 
  foodLevel = map(weight, emptyWeight, fullWeight, 0, 100);
   foodLevel = constrain(foodLevel, 0, 100);

  Blynk.virtualWrite(V4, foodLevel);

  Serial.print("Weight: ");
    Serial.print(weight);
   Serial.print(" g | Food Level: ");
  Serial.print(foodLevel);
   Serial.println("%");
}

/************* DISPENSE FUNCTION *************/
void dispenseFood() {
  Blynk.virtualWrite(V2, 1); // ON

  // Run servo feedCount times
  for (int i = 0; i < feedCount; i++) {
    feeder.write(90);
    delay(700);
     feeder.write(0);
    delay(400);
  }

  delay(1500);          // allow food to settle
  updateFoodLevel();    // read actual weight

  Blynk.virtualWrite(V2, 0); // Status OFF
}

/************* BLYNK INPUTS *************/
BLYNK_WRITE(V0) {   // Feed Now
  if (param.asInt() == 1) {
    dispenseFood();
  }
}

BLYNK_WRITE(V1) {   // Quantity slider
  feedCount = param.asInt();
  Serial.print("Feed count set to: ");
  Serial.println(feedCount);
}

BLYNK_WRITE(V3) {   // Schedule
  if (param.asInt() == 1) {
    dispenseFood();
  }
}

/************* SETUP *************/
void setup() {
  Serial.begin(9600);

  feeder.attach(SERVO_PIN);
  feeder.write(0);

  scale.begin(HX_DT, HX_SCK);
  scale.set_scale(calibration_factor);
  scale.tare();  // bowl empty during startup IMPORTANT

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Blynk.virtualWrite(V1, feedCount);
  updateFoodLevel();
}

/************* LOOP *************/
void loop() {
  Blynk.run();

  static unsigned long lastRead = 0;
  if (millis() - lastRead > 3200) {   // update every 3 sec
    lastRead = millis();
    updateFoodLevel();
  }
}
