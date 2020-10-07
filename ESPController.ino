#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
const char* WIFI_SSID = "HahYouWish"; // Enter your WiFi name
const char* WIFI_PASSWORD =  "NoWays"; // Enter WiFi PASSWORD
const char* MQTT_SERVER = "192.168.1.112";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "";
const char* MQTT_PASSWORD = "";
const char* MQTT_TOPIC_LOG = "CarController/Log";
 
WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  
  Serial.begin(9600);  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);    
  }  
 
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
 
  while (!client.connected()) {   
    if (client.connect("ESP8266Client")) {
      client.publish(MQTT_TOPIC_LOG, "Mqtt connected");
      //Serial.println("connected");  
    } else {
      //Serial.print("failed with state ");
      //Serial.print(client.state());
      delay(2000);
 
    }
  }
 
  client.subscribe("CarController/Instructions");
}
 
void callback(char* topic, byte* payload, unsigned int length) {
  for (int i = 0; i < length; i++) {
    Serial.write((char)payload[i]);
  } 
}
 
void loop() 
{

  if(WiFi.status() == WL_CONNECTED)
  {
    //Set the blue LED on 
    digitalWrite(LED_BUILTIN, LOW);
  }
  
  client.loop();
}

void sendVehicleTestCommands(){
  Serial.write("{\"instruction\":\"drive\",\"value\":1}");
  delay(5000);
  Serial.write("{\"instruction\":\"drive\",\"value\":-1}");
  delay(5000);
}
