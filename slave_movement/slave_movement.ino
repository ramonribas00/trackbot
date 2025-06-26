/*
  merged_robot_with_rssi.ino
  Sketch combinado: control de movimientos de robot diferencial con ESP32,
  medición de RSSI por UART.
*/

#include <Arduino.h>
#include <WiFi.h>
#include "driver/ledc.h"  // PWM de bajo nivel

// Configuración Wi-Fi
const char* ssid     = "X";
const char* password = "Y";

// UART para comandos RSSI
typedef HardwareSerial slave(1);

// Pines motor A (izquierdo)
const int IN2_A = 25;
const int IN1_A = 26;
const int ENA    = 12;  // PWM
// Pines motor B (derecho)
const int IN2_B = 27;
const int IN1_B = 32;
const int ENB    = 14;  // PWM

// Canales PWM
const int CH_A = 0;
const int CH_B = 1;
const int PWM_FREQ = 5000;   // 5 kHz
const int PWM_RES  = 10;     // 10 bits (0-1023)


long long rssi_mitja = 0;

void setup() {
  // Serial
  Serial.begin(115200);
  slave.begin(115200, SERIAL_8N1, 18, 19);  // RX=19, TX=18
  Serial.println("Inicio robot combinado con RSSI");

  // Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.printf("Conectando a %s", ssid);
  uint8_t intents = 0;
  while (WiFi.status() != WL_CONNECTED && intents < 60) {
    delay(500);
    Serial.print('.');
    intents++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nConectado! IP: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\nNo se pudo conectar a Wi-Fi");
  }

  // Pines motores
  pinMode(IN1_A, OUTPUT);
  pinMode(IN2_A, OUTPUT);
  pinMode(IN1_B, OUTPUT);
  pinMode(IN2_B, OUTPUT);

  // Configurar PWM
  ledc_timer_config_t timer_config = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .duty_resolution = (ledc_timer_bit_t)PWM_RES,
    .timer_num = LEDC_TIMER_0,
    .freq_hz = PWM_FREQ,
    .clk_cfg = LEDC_AUTO_CLK
  };
  ledc_timer_config(&timer_config);
  ledc_channel_config_t chanA = {
    .gpio_num   = ENA,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel    = (ledc_channel_t)CH_A,
    .intr_type  = LEDC_INTR_DISABLE,
    .timer_sel  = LEDC_TIMER_0,
    .duty       = 0,
    .hpoint     = 0
  };
  ledc_channel_config(&chanA);
  ledc_channel_config_t chanB = chanA;
  chanB.gpio_num = ENB;
  chanB.channel  = (ledc_channel_t)CH_B;
  ledc_channel_config(&chanB);
}

void loop() {
  // Chequear comandos UART para RSSI
  if (slave.available()) {
    String cmd = slave.readStringUntil('\n');
    Serial.print("Comando recibido: "); Serial.println(cmd);
    if (cmd == "rssi") {
      if (WiFi.status() == WL_CONNECTED) {
        float val = getRSSI();
        slave.println(val);
      } else {
        Serial.println("Wi-Fi no conectado");
      }
    } else {
      int espai = cmd.indexOf(' ');
      if (espai != -1 ) {
        char moviment = cmd.charAt(0);
        int vel = cmd.substring(espai + 1).toInt();

        Serial.print("Moviment rebut -> ");
        Serial.print(moviment);
        Serial.print(", ");
        Serial.println(vel);
        switch(moviment){
          case 'A':
            avanzar(vel); delay(1000); break;
          case 'D':
            rotarDerecha(vel); delay(1000); break;
          case 'I':
            rotarIzquierda(vel); delay(1000); break;
          case 'S':
            parar(); break;
        }
      }
    }

  }
  
}

// Funciones de movimiento
void avanzar(int velocidad) {
  digitalWrite(IN1_A, HIGH); digitalWrite(IN2_A, LOW);
  digitalWrite(IN1_B, HIGH); digitalWrite(IN2_B, LOW);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)CH_A, velocidad);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)CH_A);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)CH_B, velocidad);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)CH_B);
  Serial.println("Avanzando");
}
void rotarDerecha(int velocidad) {
  digitalWrite(IN1_A, HIGH); digitalWrite(IN2_A, LOW);
  digitalWrite(IN1_B, LOW);  digitalWrite(IN2_B, HIGH);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)CH_A, velocidad);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)CH_A);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)CH_B, velocidad);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)CH_B);
  Serial.println("Rotando derecha");
}
void rotarIzquierda(int velocidad) {
  digitalWrite(IN1_A, LOW);  digitalWrite(IN2_A, HIGH);
  digitalWrite(IN1_B, HIGH); digitalWrite(IN2_B, LOW);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)CH_A, velocidad);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)CH_A);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)CH_B, velocidad);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)CH_B);
  Serial.println("Rotando izquierda");
}
void parar() {
  ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)CH_A, 0);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)CH_A);
  ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)CH_B, 0);
  ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)CH_B);
  digitalWrite(IN1_A, LOW); digitalWrite(IN2_A, LOW);
  digitalWrite(IN1_B, LOW); digitalWrite(IN2_B, LOW);
  Serial.println("Parado");
}

// Medición RSSI promedio
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
