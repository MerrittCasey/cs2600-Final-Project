#define PIN_LED 2

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define WIFI_NETWORK "Merritt Wang manor"
#define WIFI_PASSWORD "wangmerritt12345"
#define WIFI_TIMEOUT_MS 20000

//lcd
#define SDA 13
#define SCL 14
LiquidCrystal_I2C lcd(0x27, 16, 2);

// MQTT Broker
const IPAddress mqttServerIP(192, 168, 0, 249);
const char *topic = "cs2600/final";
const char *mqtt_username = "mqtt";
const char *mqtt_password = "cs2600";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

//vars
int newMessage = -1;

bool connectToWifi(){
    Serial.print("Connecting to Wifi");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD);

    unsigned long startAttemptTime = millis();

    while(WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS){
        Serial.print(".");
        delay(100);
    }

    if(WiFi.status() != WL_CONNECTED){
        Serial.println(" Failed!");
        return false;
    }else{
        Serial.println("");
        Serial.println("Connected");
        Serial.println(WiFi.localIP());
        return true;
    }

    return false;
}

void callback(char *topic, byte *payload, unsigned int length) {
    //payload[length] = '\0';
    Serial.println();
    Serial.println("----------------");
    Serial.println("Message received");

    if(payload[0] == '1'){
        Serial.println("Led On");
        digitalWrite(PIN_LED, HIGH);
        newMessage = 1;
    }else if(payload[0] == '2'){
        Serial.println("Led Off"); 
        digitalWrite(PIN_LED, LOW);
        newMessage = 2;
    }else if(payload[0] == '3'){
        String t;
        for(int i = 2; i < length; i++){
            t += (char)payload[i];
        }
        lcdDisplay(t);
        newMessage = 3;
    }else if(payload[0] == '4'){
        String t;
        for(int i = 2; i < length; i++){
            t += (char)payload[i];
        }
        lcdDisplay(t);
        newMessage = 4;
    }else{
        newMessage = -1;  
    }

    Serial.println("----------------");
}

void setup() {
    Serial.begin(115200);
    pinMode(PIN_LED, OUTPUT);
    Wire.begin(SDA,SCL);
    lcd.begin(0x27, 16, 2);
    lcd.backlight();
  
    if(connectToWifi()){
        client.setServer(mqttServerIP, mqtt_port);
        client.setCallback(callback);
  
        while (!client.connected()) {
            String client_id = "esp32-client-";
            client_id += String(WiFi.macAddress());
            Serial.printf("The client %s connects to mqtt server\n", client_id.c_str());

            if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
                Serial.println("local mqtt server connected");
            } else {
                Serial.print("failed with state ");
                Serial.print(client.state());
                Serial.println("");
                delay(2000);
            }
        }
    
        client.publish(topic, "Arduino Connected");
        client.subscribe(topic);
    } 
}

void lcdDisplay(String dis){
    lcd.setCursor(0,0);
    lcd.print(dis);
    delay(5000);
    lcd.clear();
}

void loop() {
    if(newMessage != -1){
        if(newMessage == 1){
            client.publish(topic, "Led On");
        }else if(newMessage == 2){
            client.publish(topic, "Led Off");
        }else if(newMessage == 3){
            client.publish(topic, "Displayed Time");
        }else if(newMessage == 4){
            client.publish(topic, "Displayed Date");
        }else{
            newMessage = -1;
        }
        newMessage = -1;
    }
    client.loop();
}
