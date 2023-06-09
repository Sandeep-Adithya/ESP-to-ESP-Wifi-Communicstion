#include <esp_now.h>
#include <WiFi.h>

uint8_t broadcastAddress[] = {0xC8, 0xF0, 0x9E, 0xF3, 0x2A, 0xF8}; // mac of ESP_1

String msg_recv; //message to be recieved from 1
String msg_sent; //message to be sent to 2

String success;
esp_now_peer_info_t peerInfo;

// callback func when current temp is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

// callback func when required temp is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&msg_recv, incomingData, sizeof(msg_recv));
  String message = msg_recv;
  Serial.println(message);
}


void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // put your main code here, to run repeatedly:
  msg_sent = "Message to be sent to 1";

  // send the message
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &msg_sent, sizeof(msg_sent));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  
  delay(2500);

}
