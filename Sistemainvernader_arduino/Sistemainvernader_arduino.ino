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

#define DHTPIN 14
#define DHTTYPE DHT11
#define SensorPin A0
float Valorsensor = 0;
int relayPin = 4;

DHT dht(DHTPIN, DHTTYPE);

const char ssid1[] = "ALSW";
const char pass1[] = "25264897";
const char ssid2[] = "ALSW2";
const char pass2[] = "7210-3607";
const char ssid3[] = "ssid";
const char pass3[] = "pass";

WiFiClient net;
MQTTClient client;

unsigned long tiempoPasado = 0;
int Led = 5;


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
  pinMode(relayPin, OUTPUT);

  digitalWrite(Led, 1);
  Serial.println("Iniciando Wifi");
  WiFi.mode(WIFI_STA);//Cambiar modo del Wi-Fi
  delay(100);
  wifiMulti.addAP(ssid1, pass1);
  wifiMulti.addAP(ssid2, pass2);
  wifiMulti.addAP(ssid3, pass3);
  dht.begin();

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

  if ( millis() > tiempoPasado + 1000) {
    tiempoPasado = millis();
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    for (int i = 0; i <= 100; i++) {
      Valorsensor = Valorsensor + analogRead(SensorPin);
      delay(1);
    }
    Valorsensor = Valorsensor / 100.0;
    Serial.print("T:");
    Serial.print(t);
    Serial.print("c H:");
    Serial.print(h);
    Serial.print(" % HS:");
    Serial.print(Valorsensor);
    Serial.println("%");

    String Mensaje = String(h);
    client.publish("/Sensor/Humedad", Mensaje);
    Mensaje = String(t);
    client.publish("/Sensor/temperatura", Mensaje);
    Mensaje = String(Valorsensor);
    client.publish("/Sensor/HumedadTierra", Mensaje);
  }
}

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
