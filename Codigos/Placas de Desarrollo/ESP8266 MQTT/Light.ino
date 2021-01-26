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
const int ledPin =  2;   //D4 led incorporado.
bool Anterior = false;   //variable utilizada para guardar estado del boton y que solo se presione una vez.
bool lightState = false;//estado del PIN.
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
    ONOFF("M");
    reconnect();
  }
  client.loop();
  ONOFF("M");
}



//======================= Funcion para Conectarse al Wi-Fi=======================================================//
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);                    //WIFI en modo estación es decir un cliente como la PC.
  WiFi.begin(ssid, password);             //Inicia conexión con la red WIFI.
  
  unsigned long TiempoAhora = 0;          //Variable necesaria para almacenar valor de millis(). 
  int estadoWifi = WiFi.status();         //Variable que almacena estado de la conexión WIFI.
  while (estadoWifi != WL_CONNECTED) {    //Mientras el estado de la conexión sea distinto de "conectado", se ejecuta.
     if(millis() > TiempoAhora + 500){    //millis():Devuelve el número de milisegundos transcurridos desde que la placa comenzó a ejecutar el Sketch. Si ese número es mayor a TiempoAhora(inicialmente vale cero y luego de entrar en el if toma el valor de millis() de ese momento) más el tiempo de retardo deseado... se ejecuta el if. 
        TiempoAhora = millis();           //TiempoAhora adquiere el valor de millis() de ese momento.
        estadoWifi = WiFi.status();       //actualiza "estadoWifi" con el estado de la conexicón WIFI de ese momento. 
        Serial.print(".");
     } 
     else ONOFF("M");                     //en caso de que no haya transcurrido el tiempo necesario para entrar al if, se ejecuta la función para actualizar estado de las luces.
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//===============================================================================================================//


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
    char* estado[]={"OFF", "ON"};
    if((modo=="ON") || (modo=="OFF")){
        if(modo=="ON"){
            lightState = HIGH;
            //client.publish("Eluz","ON");
            Serial.print("estado del PIN: ");
            Serial.println(lightState);
        }
        else{
            lightState = LOW;
            //client.publish("Eluz","OFF");
            Serial.print("estado del PIN: ");
            Serial.println(lightState);
        }
    client.publish("Eluz",estado[lightState]);
    }
    else{
        if (digitalRead(buttonPin) == HIGH) {
            if(Anterior == false){
            //Serial.print("presionado ");
            lightState = !lightState;
            //Serial.print("Estado: ");
            //Serial.println(lightState);
            Serial.print("estado del PIN: ");
            Serial.println(lightState);
            if(client.connected()){
              client.publish("Eluz",estado[lightState]);
            }
            Anterior = true;
            }
            delay(10);
        }
        else { Anterior = false;}
        }

    digitalWrite(ledPin,lightState);
    
}
//===============================================================================================================//
