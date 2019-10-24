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
    console.log("MQTT Perdio conexion Error:" + responseObject.errorMessage);
  }
}
let MensajeUltimo;

function MQTTMensaje(message) {
  MensajeUltimo = message;
  if (message.destinationName == "/Sensor/Humedad") {
    console.log("Mensaje Humedad:" + message.destinationName + " - " + message.payloadString);
    Humedad = Number(message.payloadString);
  } else if(message.destinationName == "/Sensor/temperatura") {
    console.log("Mensaje recibido:" + message.destinationName + " - " + message.payloadString);
    Temperatura = Number(message.payloadString);
  } else if(message.destinationName == "/Sensor/HumedadTierra") {
    console.log("Mensaje recibido:" + message.destinationName + " - " + message.payloadString);
    HumedadTierra = Number(message.payloadString);
  } else {
    console.log("Mensaje recibido:" + message.destinationName + " - " + message.payloadString);
  }
}

function CuandoConectadoMQTT() {
  console.log("MQTT Conectado");
  client.subscribe("/Sensor/#");

}


function setup() {
  var BotonEncender = createButton("Encender Bomba");
  BotonEncender.mousePressed(EncenderBomba);
  var BotonApagar = createButton("Apagar Bomba");
  BotonApagar.mousePressed(ApagarBomba);
}

function ApagarBomba() {
  message = new Paho.MQTT.Message("Apagar");
  message.destinationName = "/Senal/BombaAgua";
  client.send(message);
}

function EncenderBomba() {
  message = new Paho.MQTT.Message("Encender");
  message.destinationName = "/Senal/BombaAgua";
  client.send(message);
}

//EXISTE UN BUCLE EN EL IF QUE PRODUCE UN FALLO DE CONEXIÓN
//AL ENVIARSE MUCHOS MENSAJES EN POCO TIEMPO HACIA EL ARDUINO.
function draw() {
  if ((Humedad < 40) || (HumedadTierra < 100) || (Temperatura > 30)) {
    EncenderBomba();
  } else if ((Humedad > 40) || (HumedadTierra>100) || (Temperatura < 30)) {
    ApagarBomba();
  }
}
