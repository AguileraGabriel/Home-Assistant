#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//=========================== Configuración de parámetro de conectividad =========================================//
const char* ssid = "Your SSID";
const char* password = "Your Password";
IPAddress mqtt_server(---, ---, -, ---);                       //ip del broker MQTT: mosquitto.
WiFiClient espClient;
PubSubClient client(espClient);
//===============================================================================================================//


void setup()
{
  Serial.begin(57600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(1500);
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}



//======================= Funcion para Conectarse al Wi-Fi========================
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA); //wifi en modo estación es decir un cliente como la PC.
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


//=========================== Función para conectarse, publicar y subscribirse ==================================//
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient","user","password")) {
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
