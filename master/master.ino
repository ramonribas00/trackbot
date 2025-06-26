#include <HardwareSerial.h>
#include <WiFi.h>
const char* ssid     = "POCO X6 Pro 5G";
const char* password = "xkda3agwxrtfzgw";
HardwareSerial SerialA(1);  // UART1
HardwareSerial SerialB(2);  // UART2

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
  int* A = rssi();

  move('A', 10);
  delay(2500);
  move('D', 2000);
  delay(2500);
  move('S', 0);
  delay(2500);

  

  delay(10000);
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
