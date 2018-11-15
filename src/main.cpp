#include <Arduino.h>

#include <stdlib.h>

#include <WiFi.h>
#include <Wire.h>
#include <SparkFunHTU21D.h>
#include <PubSubClient.h>

#define LED 25
#define SWITCH 0

const char* ssid     = "Theutheu";
const char* password = "vT__12126";

/*
CloudMQTT
const char* mqttServer = "m23.cloudmqtt.com";
const int mqttPort = 10226;
const char* mqttUser = "qnopdrhy";
const char* mqttPassword = "fq7FS6HhlY6V";
*/

// Raspberry pi
const char* mqttServer = "192.168.1.118";
const int mqttPort = 1883;
const char* mqttUser = "";
const char* mqttPassword = "";

HTU21D myHumidity;

WiFiClient espClient;
PubSubClient client(espClient);

// the OLED used
//U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}


void setup() {
    
    Serial.begin(115200);
    myHumidity.begin();

    pinMode(LED,OUTPUT);
    pinMode(SWITCH,INPUT);

    Serial.println("*** ESP32 Wifi MQTT Tester ***");

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);

    while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
        if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
        
          Serial.println("connected");  
    
        } else {
        
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
    
        }
    }
 
    //client.subscribe("esp/test");
    //client.publish("esp32/temp", "Hello from ESP32");
}

void loop() {

    int i;

    float humd = myHumidity.readHumidity();
    float temp = myHumidity.readTemperature();

    char tempstring[5];
    char humdstring[5];
    char temp_message[30];
    char humd_message[30];
    dtostrf(temp,3,1,tempstring);
    dtostrf(humd,3,1,humdstring);

    sprintf(temp_message, "%s", tempstring);
    sprintf(humd_message, "%s", humdstring);

    client.publish("esp/temp",temp_message);
    client.publish("esp/humd",humd_message);

    Serial.print("Time:");
    Serial.print(millis());
    Serial.print(" Temperature:");
    Serial.print(tempstring);
    Serial.print("C");
    Serial.print(" Humidity:");
    Serial.print(humdstring);
    Serial.print("%");

    Serial.println();

    for(i=0;i<=5;i++){
        client.loop();
        delay(2000);
    }
}