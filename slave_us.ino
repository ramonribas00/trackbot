#include <WiFi.h>
HardwareSerial slave(1);
/*––– MODIFICA aquestes dues línies amb el teu Wi-Fi –––*/
const char* ssid     = "X";
const char* password = "Y";
/*––––––––––––––––––––––––––––––––––––––––––––––––––––––*/
#define TRIG_PIN 12      // HC-SR04 trig
#define ECHO_PIN 14      // HC-SR04 echo

long rssi_mitja = 0;

void setup() {

  Serial.begin(115200);
  slave.begin(115200, SERIAL_8N1, 19, 18);  // RX, TX

  delay(500);                     // dóna temps a obrir el monitor sèrie
  WiFi.mode(WIFI_STA);            // mode “estació” (client)
  Serial.printf("Connectant a %s", ssid);

  WiFi.begin(ssid, password);
  uint8_t intents = 0;
  while (WiFi.status() != WL_CONNECTED && intents < 60) {
    delay(500);
    Serial.print('.');
    intents++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nConnectat! IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\nNo s'ha pogut connectar – revisa SSID/password.");
  }
}

void loop() {
  if (slave.available()) {
    String rebut = slave.readStringUntil('\n');
    Serial.print("Rebut: ");
    Serial.println(rebut);
    if(rebut == "rssi"){
      if (WiFi.status() == WL_CONNECTED) {
        long rssiRead = getRSSI();
        slave.println(rssiRead);
      } else {
        Serial.println("Sense Wi-Fi");
      }
    } else if(rebut == "dist"){
      float us = dist();
      slave.println(us);
    }
  
    delay(1000);
  }
}

long getRSSI(){
  if (WiFi.status() == WL_CONNECTED) {
    rssi_mitja = 0;
    int i;
    for(i= 0; i!=10000;i++){
      long rssi = WiFi.RSSI();       // dBm (−30 molt fort, −90 molt fluix)
      rssi_mitja += rssi;
      
    }
    
    long mitjana = rssi_mitja / (i+1);
    Serial.printf("RSSI mitjana: %ld dBm\n\n", mitjana);
      
    return mitjana;
  } else {
    Serial.println("Sense Wi-Fi");

  }
}

float dist(){
  long duration;
  float distance;

  // Send trig pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo pulse
  duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout: 30ms (max ~5m)

  // Convert to distance in cm
  distance = duration * 0.0343 / 2;

  // Print result
  if (duration == 0) {
    Serial.println("Out of range");
  } else {
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }
  return distance;
  delay(500);
}