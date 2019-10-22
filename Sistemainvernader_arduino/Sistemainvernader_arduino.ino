#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#else
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#endif

#include <MQTT.h>
#include <DHT.h>


const char ssid1[] = "ALSW";
const char pass1[] = "25264897";
const char ssid2[] = "ALSW2";
const char pass2[] = "7210-3607";
const char ssid3[] = "ssid";
const char pass3[] = "pass";

WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;
int Led = 5;

void Conectar() {
  Serial.print("Conectando a Wifi...");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(Led, 1);
    delay(2000);
    digitalWrite(Led, 0);
    delay(2000);
  }
  Serial.print("\nConectado a MQTT...");

  while (!client.connect("esp8266", "leyo2123", "pass1234")) {
    Serial.print("*");
    digitalWrite(Led, 1);
    delay(500);
    digitalWrite(Led, 0);
    delay(500);
  }

  Serial.println("\nConectado MQTT!");

  client.subscribe("/ALSW/Clasificar");
}

void RecibirMQTT(String &topic, String &payload) {
  Serial.println("Recivio: " + topic + " - " + payload);
  if (payload == "Encender") {
    Serial.println("Encender Foco");
   
  } else if (payload == "Apagar") {
    Serial.println("Apagar Foco");
   
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(Led, OUTPUT);
  digitalWrite(Led, 1);
  Serial.println("Iniciando Wifi");
  WiFi.mode(WIFI_STA);//Cambiar modo del Wi-Fi
  delay(100);
  wifiMulti.addAP(ssid1, pass1);
  wifiMulti.addAP(ssid2, pass2);
  wifiMulti.addAP(ssid3, pass3);

  client.begin("broker.shiftr.io", net);
  client.onMessage(RecibirMQTT);

  Conectar();
}

void loop() {
  client.loop();
  delay(10);

  if (!client.connected()) {
    Conectar();
  }
}
