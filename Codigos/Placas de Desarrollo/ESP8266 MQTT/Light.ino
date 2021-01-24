#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//=========================== Configuración de parámetro de conectividad =========================================//
const char* ssid = "Your SSID";
const char* password = "Your Password";
IPAddress mqtt_server(---, ---, -, ---);                       //ip del broker MQTT: mosquitto.
WiFiClient espClient;
PubSubClient client(espClient);
//===============================================================================================================//


//============================ PINES ============================================================================//
const int buttonPin = 0; //D3 tiene resistencia Pull-Down.
const int ledPin =  2; //D4 led incorporado.
bool Anterior = false;
bool buttonState = false;
//===============================================================================================================//


void setup()
{
  Serial.begin(57600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);
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
      client.subscribe("luz");
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
  /*for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }*/
  String msjRecibido = "";
  //int intMsj = 0;
  for (int i = 0; i < length; i++) {
    msjRecibido += (char)payload[i];
  }
  Serial.println();
   if (strcmp(topic, "luz") == 0) {
       if(msjRecibido == "ON"){
            //client.publish("Eluz","ON");
            ONOFF("ON");
       }
       else {
            //client.publish("Eluz","OFF");
            ONOFF("OFF");
       }
   } 
}
//===============================================================================================================//


//========================= Función para Interactuar con el pulsador y activar PIN D4 ===========================//
void ONOFF(String modo){
    if((modo=="ON") || (modo=="OFF")){
        if(modo=="ON"){
            buttonState = HIGH;
            client.publish("Eluz","ON");
        }
        else{
            buttonState = LOW;
            client.publish("Eluz","OFF");
        }
    }
    else{
        if (digitalRead(buttonPin) == HIGH) {
            if(Anterior == false){
            //Serial.print("presionado ");
            buttonState = !buttonState;
            //Serial.print("Estado: ");
            //Serial.println(buttonState);
            Anterior = true;
            }
            delay(10);
        }
        else { Anterior = false;}
        }
    Serial.print("estado del PIN: ");
    Serial.println(buttonState);
    digitalWrite(ledPin,buttonState);
}

//===============================================================================================================//