#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

//=========================== Configuración de parámetro de conectividad =========================================//
byte mac[]    = {  0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD };  //dirección MAC del dispositivo: Ethernet Shield
IPAddress ip(192, 168, 0, 14);                           //ip del dispositivo.
IPAddress server(192, 168, 0, 2);                       //ip del broker MQTT: mosquitto.
EthernetClient ethClient;                                 //cliente ethernet
PubSubClient client(ethClient);                           //cliente MQTT, partiendo del anterior. 
//===============================================================================================================//



void setup()
{
  Serial.begin(57600);

  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}




//=========================== Función para conectarse, publicar y subscribirse ==================================//
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient","UserMQTTBraker","passwordMQTTBraker")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//===============================================================================================================//



//=========================== Función para recibir mensajes =====================================================// 
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
//===============================================================================================================//

