#include <HardwareSerial.h>
#include <WiFi.h>
const char* ssid     = "X";
const char* password = "Y";
HardwareSerial SerialA(1);  // UART1
HardwareSerial SerialB(2);  // UART2
long long rssi_mitja;
void setup() {
  Serial.begin(115200);
  SerialA.begin(115200, SERIAL_8N1, 18, 19);  // RX, TX
  SerialB.begin(115200, SERIAL_8N1, 16, 17); //RX1,TX1 amb rodes

  delay(1000);
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
  float rssi_mestre = getRSSI();           // RSSI del propi ESP32 (client WiFi)
  int* rssi_vals = rssi();                 // RSSI dels esclaus per UART
  int rssiA = rssi_vals[0];
  int rssiB = rssi_vals[1];

  // Si cap valor no és vàlid, no fem res
  if (rssiA == -999 && rssiB == -999) {
    Serial.println("Cap RSSI vàlid dels esclaus.");
    delay(3000);
    return;
  }

  int millorRSSI = max(rssiA, rssiB);
  Serial.printf("RSSI mestre: %.2f | Millor RSSI esclau: %d\n", rssi_mestre, millorRSSI);

  if (rssi_mestre > millorRSSI) {
    Serial.println("RSSI mestre és millor → continuar endavant.");
    move('A', 200);
    delay(1000);
    move('S', 0);
  } else {
    if (rssiA > rssiB) {
      Serial.println("Millor RSSI esclau: A → gira esquerra i avança.");
      move('I', 100);
      delay(1000);
    } else {
      Serial.println("Millor RSSI esclau: B → gira dreta i avança.");
      move('D', 100);
      delay(1000);
    }
    move('A', 200);
    delay(1000);
    move('S', 0);
  }

  delay(3000);  // Espera abans del següent cicle
}

void move(char mov, int vel){ //'A' endevant ; 'D' rotar dreta ; 'I' rotat esquerra ; 'S' Stop
  String respostaB;
  String comanda = String(mov) + " " + String(vel)+'\n';
  Serial.println(comanda);
  SerialB.println(comanda);
  delay(500);
  if (SerialB.available()) {
    respostaB = SerialB.readStringUntil('\n');
    Serial.print("Resposta B: "); Serial.println(respostaB);

  }
  
}
int* rssi(){
  static int result[2];
  int respostaA=-999, respostaB=-999;
  String comanda = "rssi";
  SerialA.println(comanda);
  SerialB.println(comanda);
  delay(500);
  if (SerialA.available()) {
    respostaA = SerialA.readStringUntil('\n').toFloat();
    Serial.print("Resposta A: "); Serial.println(respostaA);

  }
  if (SerialB.available()) {
    respostaB = SerialB.readStringUntil('\n').toFloat();
    Serial.print("Resposta B: "); Serial.println(respostaB);
  }
  result[0] = respostaA;
  result[1] = respostaB;
  Serial.print("A: ");
  Serial.println(result[0]);

  Serial.print("B: ");
  Serial.println(result[1]);
  return result;

}
float dist(){
  String respostaA;
  String comanda = "dist";
  SerialA.println(comanda);
  delay(500);
  if (SerialA.available()) {
    respostaA = SerialA.readStringUntil('\n');
    Serial.print("Resposta A: "); Serial.println(respostaA);
  }
  return respostaA.toFloat();
}
float getRSSI() {
  rssi_mitja = 0;
  int i;
  for (i = 0; i < 10000; i++) {
    rssi_mitja += WiFi.RSSI();
  }
  float avg = rssi_mitja / (float)i;
  Serial.printf("RSSI promedio: %ld dBm\n", avg);
  return avg;
}

