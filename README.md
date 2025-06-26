<img src="/assets/Trackbot.png" align="right" width="250" alt="header pic" />

# TRACKBOT

## Índex
1. [Què és el Trackbot?](#1-què-és-el-trackbot)
2. [Componenets](#2-componenets)
3. [Diagrames](#3-diagrames)
   1. [Diagrama Fritzing](#31-diagrama-fritzing)
   2. [Diagrama de mòduls SW](#32-diagrama-de-mòduls-sw)
4. [Algorismes](#4-algorismes)
5. [Models 3D](#5-models-3d)
   1. [Tinkercad](#51-tinkercad)
   2. [Inkscape](#52-inkscape)
6. [Imatges Trackbot](#6-imatges-trackbot)
7. [Conclusions](#7-conclusions)

---

## 1. Què és el Trackbot?

Robot mòbil amb tres mòduls ESP32 disposats en tríada per mesurar el RSSI des de diferents angles; mitjançant càlcul de gradients de senyal, ajusta el seu desplaçament en temps real per acostar-se al dispositiu emissor Wi-Fi.

## 2. Componenets

El componenets que hem utilitzat han estat:

- x3 ESP32 --> Captar intensitats RSSI
- x2 HC-SR04 --> Sensor de proximitat (ultrasons)
- L9110 --> Controladora
- Portapiles --> 3 piles AAA
- x2 Motor DC N20
- Mini-Protoboard

## 3. Diagrames

Durant el desenvolupament de la arquitectura, desenvoluparem els següents diagrames:

### 3.1 Diagrama Fritzing

<img src="/assets/DiagramaFritzing.jpg" alt="Diagrama Fritzing" width="400" />

### 3.2 Diagrama de mòduls SW

<img src="/assets/DiagramaSW.png" alt="Diagrama SW" width="400" />

## 4. Algorismes

**master.ino**

L’ESP32 actua com a cervell central: primer es connecta en mode estació a la xarxa Wi-Fi del dispositiu objectiu, després dins del bucle principal llegeix periòdicament el RSSI amb `WiFi.RSSI()` o `WiFi.scanNetworks()`, acumula diverses mostres per calcular-ne una mitjana més estable i finalment envia aquest valor mitjà pels ports UART als dos “esclaus” (moviment i ultrasons). A més, implementa un protocol molt senzill de comandes de text per sol·licitar a cada mòdul la dada corresponent i llegir la resposta per sèrie.

**slave\_movement.ino**

Aquest mòdul rep via UART el RSSI mitjà enviat pel master i aplica un control de llindar,  per decidir si avançar o girar i així maximitzar la senyal. Tot i que existeix `getRSSI()` per mesurar localment i fer la mitjana de milers de lectures de `WiFi.RSSI()`, normalment s’usa el valor centralitzat. Amb aquest RSSI el compara amb un llindar desitjat i ajusta la velocitat de les rodes (via PWM amb LEDC) per corregir el rumb: més senyal → recte; menys senyal → girs de cerca.

**slave\_us.ino**

Aquí es gestiona el sensor HC-SR04 per mesurar obstacles: el mòdul dispara un trig (`digitalWrite(TRIG_PIN, HIGH)` molt breu), mesura la durada de l’eco amb `pulseIn(ECHO_PIN, HIGH)` i converteix aquest temps en distància (cm) amb la fórmula `distance = duration * 0.0343 / 2`. Després envia la distància pel UART al mòdul de moviment, que l’integra a la seva lògica per aturar-se o esquivar obstacles. Així el robot segueix la font Wi-Fi i, alhora, evita col·lisions.

## 5. Models 3D

En aquest apartat mostrem algunes imatges dels models implementats, molt importants alhora d'utilitzar l'eina de tall làser.
### 5.1 Tinkercad

<img src="/assets/TINKER1.png" alt="T1" width="300" />
<img src="/assets/TINKER2.png" alt="T2" width="300" />
<img src="/assets/TINKER3.png" alt="T3" width="300" />


### 5.2 Inkscape

<img src="/assets/INK.png" alt="INK" width="400" />

## 6. Imatges Trackbot

<img src="/assets/IMG_4149.png" alt="A1" width="300" />
<img src="/assets/IMG_4146.png" alt="A2" width="300" />
<img src="/assets/IMG_4144.png" alt="A3" width="300" />

## 7. Conclusions

Aquest projecte ens ha tret de la nostra zona de confort i ens ha servit per treballar amb noves eines com la impressora 3D per fabricar els suports dels motors i la màquina de tall per làser per construir l’estructura. Realment, tot i no ser de la mencio de computació, hem vist una cara de la especialització força interessant, i que ens ha aportat coneixements d'algorismes i componenets essencial pel futr. Finalment, és una llàstima que els motors no hagin tingut prou potència per moure amb eficàcia el robot, perquè de ser així hauria obtés els resultats esperats.

VIDEO ROBOT: https://youtu.be/orLypsw5CK8
