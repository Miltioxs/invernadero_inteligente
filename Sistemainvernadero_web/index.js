let BrokerMQTT = 'broker.shiftr.io';
let PuertoMQTT = 80;
let ClienteIDMQTT = "Sitio_invernadero_inteligente";
let UsuarioMQTT = "leyo2123";
let ContrasenaMQTT = "pass1234";

client = new Paho.MQTT.Client(BrokerMQTT, PuertoMQTT, ClienteIDMQTT);

let Temperatura, Humedad, HumedadTierra;
client.onConnectionLost = MQTTPerder;
client.onMessageArrived = MQTTMensaje;

client.connect({
  onSuccess: CuandoConectadoMQTT,
  userName: UsuarioMQTT,
  password: ContrasenaMQTT
});

function MQTTPerder(responseObject) {
  if (responseObject.errorCode !== 0) {
    console.log("MQTT Perdio coneccion Error:" + responseObject.errorMessage);
  }
}
let MensajeUltomo;

function MQTTMensaje(message) {
  MensajeUltomo = message;
  if (message.destinationName == "/Sensor/Humedad") {
    console.log("Mensaje Humedad:" + message.destinationName + " - " + message.payloadString);
    Humedad = StringToInt(message.payloadString);
  } else {
    console.log("Mensaje recibido:" + message.destinationName + " - " + message.payloadString);
  }
}

function CuandoConectadoMQTT() {
  console.log("MQTT Conectado");
  client.subscribe("/Sensor/#");

}


function setup() {
  var BotonEncender = createButton("Encender Bonba");
  BotonEncender.mousePressed(EncenderBonba);
  var BotonApagar = createButton("Apagar Bonba");
  BotonApagar.mousePressed(ApagarBonba);
}

function ApagarBonba() {
  message = new Paho.MQTT.Message("Apagar");
  message.destinationName = "/Senal/BonbaAgua";
  client.send(message);
}

function EncenderBonba() {
  message = new Paho.MQTT.Message("Encender");
  message.destinationName = "/Senal/BonbaAgua";
  client.send(message);
}

function draw() {
  if (Humedad < 50) {
    EncenderBonba();
  }
}
