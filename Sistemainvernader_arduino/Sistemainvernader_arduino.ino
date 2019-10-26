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

String apiKey = "T6VR8MY45YFA26S3";
const char* server = "api.thingspeak.com";
const char *ssid = "Dirty Bomb";
const char *pass = "2097ESTS3";
float h;
float t;
int BombaPin = 4;

DHT dht(DHTPIN, DHTTYPE);

const char ssid1[] = "ALSW";
const char pass1[] = "25264897";
const char ssid2[] = "Dirty Bomb";
const char pass2[] = "2097ESTS3";
const char ssid3[] = "ssid";
const char pass3[] = "pass";

WiFiClient client;
MQTTClient clientt;

unsigned long interval = 10000;
unsigned long previousMillis = 0;
float moisturePercentage;

unsigned long tiempoPasado = 0;
int Led = 5;


void RecibirMQTT(String &topic, String &payload) {
  Serial.println("Recivio: " + topic + " - " + payload);
  if (payload == "Encender") {
    Serial.println("Encender Bomba");
    digitalWrite(BombaPin, 1);

  } else if (payload == "Apagar") {
    Serial.println("Apagar Bomba");
    digitalWrite(BombaPin, 0);

  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(Led, OUTPUT);
  pinMode(BombaPin, OUTPUT);

  digitalWrite(BombaPin, 0);

  digitalWrite(Led, 1);
  Serial.println("Iniciando Wifi");
  WiFi.mode(WIFI_STA);//Cambiar modo del Wi-Fi
  delay(100);

  WiFi.begin(ssid2, pass2);
  wifiMulti.addAP(ssid1, pass1);
  wifiMulti.addAP(ssid2, pass2);
  wifiMulti.addAP(ssid3, pass3);
  dht.begin();

  clientt.begin("broker.shiftr.io", client);
  clientt.onMessage(RecibirMQTT);

  Conectar();

}

void loop() {
  clientt.loop();
  delay(10);

  if (!clientt.connected()) {
    Conectar();
  }

  if ( millis() > tiempoPasado + 1000) {
    tiempoPasado = millis();
    h = dht.readHumidity();
    t = dht.readTemperature();

    Serial.print(" Temperatura: ");
    Serial.print(t);
    Serial.print("°C   HumedadAmbiente: ");
    Serial.print(h);
    Serial.print(" %   HumedadTierra: ");
    Serial.print(moisturePercentage);
    Serial.println(" % ");
    String Mensaje = String(h);
    clientt.publish("/Sensor/Humedad", Mensaje);
    Mensaje = String(t);
    clientt.publish("/Sensor/temperatura", Mensaje);
    Mensaje = String(moisturePercentage);
    clientt.publish("/Sensor/HumedadTierra", Mensaje);
  }

  unsigned long currentMillis = millis();


  moisturePercentage = (100.00 - ((analogRead(SensorPin) / 1024.00) * 100.00));

  if ((t > 31) && (digitalRead(BombaPin) == 0)) {
    Serial.println("Encender Bomba");
    digitalWrite(BombaPin, 1);
  }

  if ((t < 28) && (digitalRead(BombaPin) == 1)) {
    Serial.println("Apagar Bomba");
    digitalWrite(BombaPin, 0);
  }

  if ((unsigned long)(currentMillis - previousMillis) >= interval) {
    sendThingspeak();
    previousMillis = millis();
    client.stop();
  }


}

void sendThingspeak() {
  if (client.connect(server, 80))
  {
    String postStr = apiKey;              // add api key in the postStr string
    postStr += "&field1=";
    postStr += String(t);    // add mositure readin
    postStr += "&field2=";
    postStr += String(h);                 // add tempr readin
    postStr += "&field3=";
    postStr += String(moisturePercentage);                  // add humidity readin
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());           //send lenght of the string
    client.print("\n\n");
    client.print(postStr);                      // send complete string

    Serial.println(" Datos enviados a ThingSpeak.");
  }
}



void Conectar() {

  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(Led, 1);
    delay(2000);
    digitalWrite(Led, 0);
    delay(2000);
  }


  while (!clientt.connect("Sistema_invernadero", "leyo2123", "pass1234")) {
    Serial.print("*");
    digitalWrite(Led, 1);
    delay(500);
    digitalWrite(Led, 0);
    delay(500);
  }



  clientt.subscribe("Senal/BombaAgua");
}
