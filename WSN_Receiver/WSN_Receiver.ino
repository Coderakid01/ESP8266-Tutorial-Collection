#include <ESP8266WiFi.h>;
#include <espnow.h>
#include <WiFiClient.h>;
#include <ThingSpeak.h>;

const char* ssid = "YOURWIFINAME"; //Your WiFi Name
const char* password = "YOURWIFIPASSWORD"; //Your WiFi Password

WiFiClient client;

unsigned long myChannelNumber = 12345678; //Your Channel Number (Without Brackets)
const char * myWriteAPIKey = "YOURWRITEAPIKEY"; //Your Write API Key

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  float Temp;
  float Humid;
} struct_message;

// Create a struct_message called myData
struct_message myData;

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);

  Serial.print("Example Data a: ");
  Serial.println(myData.Temp);
  Serial.print("Example Data b: ");
  Serial.println(myData.Humid); 
  Serial.println();
}

void setup(){
Serial.begin(115200);
//delay(10);
//dht.begin();

// Set device as a Wi-Fi Station
WiFi.mode(WIFI_STA);

// Init ESP-NOW
if (esp_now_init() != 0) {
   Serial.println("Error initializing ESP-NOW");
   return;
}

// Once ESPNow is successfully Init, we will register for recv CB to
// get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

// Connect to WiFi network
WiFi.begin(ssid, password);
ThingSpeak.begin(client);
}

void loop(){

// set the fields with the values
  ThingSpeak.setField(1, myData.Temp);
  ThingSpeak.setField(2, myData.Humid);
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  delay(20000); // Wait 20 seconds to update the channel again
}
