#include <M5Stack.h>
#include "icons.h"  
#include "ArduinoJson.h"




#include <WiFi.h>
#include <WiFiUdp.h>
const char * networkName = "Home WiFi";
const char * networkPswd = "pixel2013";
const char * udpAddress = "192.168.88.220";
const int udpPort = 1234;
boolean connected = false;
WiFiUDP udp;



String readString, cmd, cmd_val;
int focus_val = 0;
int focus_val_prev = 0;
float focus_val_metric = 0;
float focus_val_metric_prev = 0; 

int cal_data_points_coint = 6;


struct LensData {
  char lens_name[32];
  float cal_data[128][2];
};

void setup() {
  M5.begin();
  M5.Lcd.fillScreen(TFT_BLACK);
  
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);


  // udp

  connectToWiFi(networkName, networkPswd);

  // udp 



  

  M5.Lcd.drawXBitmap(138, 88, logo, logoWidth, logoHeight, TFT_WHITE);
  delay(500);
  M5.Lcd.clear(BLACK);

  LensData LensData;
  load_lens_data(&LensData);
  print_header(&LensData, 0, 0, 0);
  print_buttons("set", "menu", "...");
  print_focus_ring(&LensData);
  print_rec_button(false);

}

void loop() {
  while (Serial2.available()) {
    if (Serial2.available() >0) {
      char c = Serial2.read();
      readString += c;
    }
  }

  if (readString.length() >0) {
      cmd = readString.substring(0, 6); 
      cmd_val = readString.substring(9, 13); 
      
      if(cmd == ":01060") { // focus cmd
        focus_val = strtol(cmd_val.c_str(), 0, 16)/10;
      }           
    readString="";
  }


  //udp

  


  // udp
  
    //here
    LensData LensData;
    update_focus_ring(&LensData);
    
    // make separate functions for header, rec, focus wheel, bottom button
}




void print_header(struct LensData* LensData, int wifi, int camera, int link) {
    M5.Lcd.drawString(LensData->lens_name,  2, 4, 2);
    M5.Lcd.drawXBitmap(241, 2, wi_fi_bits, wi_fi_width, wi_fi_height, TFT_WHITE);
    M5.Lcd.drawXBitmap(271, 2, camera_bits, camera_width, camera_height, TFT_WHITE);
    M5.Lcd.drawXBitmap(301, 4, link_bits, link_width, link_height, TFT_WHITE);
}

void print_buttons(String left, String center, String right){
    M5.Lcd.drawString(left,  55, 220, 2);
    M5.Lcd.drawString(center, 145, 220, 2);
    M5.Lcd.drawString(right,  250, 220, 2);
}

void print_rec_button(bool state){
  if(state) {
    M5.Lcd.drawXBitmap(128, 38, rec_bits, rec_width, rec_height, TFT_RED);
  } else {
    M5.Lcd.drawXBitmap(128, 38, rec_bits, rec_width, rec_height, TFT_WHITE);
  }
}

void print_focus_ring(struct LensData* LensData) {
  M5.Lcd.setTextDatum(MC_DATUM);
  
  M5.Lcd.drawString("          ", 150, 130, 6);
  M5.Lcd.drawString(String(focus_val), 160, 130, 6);
  
  M5.Lcd.setTextDatum(TL_DATUM);
  M5.Lcd.drawFastHLine(70, 180, 180, TFT_WHITE); // focus line
  //M5.Lcd.drawFastVLine(map(focus_val, 0, 999, 70, 250), 175, 10, TFT_WHITE); // focus current

  for (int i=0; i <= cal_data_points_coint; i++){
    Serial.println(LensData->cal_data[i][0]);
    Serial.println(LensData->cal_data[i][1]);
    
    String point_name = String(LensData->cal_data[i][0]);
    
    if(int(LensData->cal_data[i][0]) == 999) {
      point_name = "inf";
    } 
    
    //M5.Lcd.drawFastHLine(70, 180, 180, TFT_WHITE); // focus line
    if ( (i % 2) != 0) {
      M5.Lcd.drawString(point_name, map(int(LensData->cal_data[i][1]), 0, 999, 70, 250) - 10, 195, 2); 
      M5.Lcd.drawFastVLine(         map(int(LensData->cal_data[i][1]), 0, 999, 70, 250),      185, 5, TFT_WHITE); 
    } else {
      M5.Lcd.drawString(point_name, map(int(LensData->cal_data[i][1]), 0, 999, 70, 250) - 10, 155, 2); 
      M5.Lcd.drawFastVLine(         map(int(LensData->cal_data[i][1]), 0, 999, 70, 250),      170, 5, TFT_WHITE); 
    }
    
  }

}

void update_focus_ring(struct LensData* LensData) {
      int target = focus_val;
      byte nearindex = 0;
      int neardiff = abs(int(LensData->cal_data[0][1]) - target);
      
      for (byte i = 1; i < cal_data_points_coint; i++) {
        if (abs(int(LensData->cal_data[i][1]) - target) < neardiff) {
          nearindex = i;
          neardiff = abs(int(LensData->cal_data[i][1]) - target);
        }
      }

      if(target > LensData->cal_data[nearindex][1]) {
       focus_val_metric = mapf(target, LensData->cal_data[nearindex][1], LensData->cal_data[nearindex+1][1], LensData->cal_data[nearindex][0], LensData->cal_data[nearindex+1][0]);
      } else {
       focus_val_metric = mapf(target, LensData->cal_data[nearindex-1][1], LensData->cal_data[nearindex][1], LensData->cal_data[nearindex-1][0], LensData->cal_data[nearindex][0]);
      }

      if(abs(focus_val_metric - focus_val_metric_prev) > 0.01) {
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.drawString("              ", 150, 130, 6);
        if(LensData->cal_data[cal_data_points_coint-1][0] < focus_val_metric) {
          M5.Lcd.drawString(String("inf"), 160, 130, 4);
        } else {
          M5.Lcd.drawString(String(focus_val_metric), 160, 130, 6);
        }
        focus_val_metric_prev = focus_val_metric;


        // udp
        char senddata[20];
        String focus_val_metric_to_send = String(focus_val_metric);
        focus_val_metric_to_send += "\n";
        focus_val_metric_to_send.toCharArray(senddata, sizeof(senddata));
        if(connected){
          //Send a packet
          udp.beginPacket(udpAddress,udpPort);
          udp.printf(senddata);
          udp.endPacket();
        }
        // udp
        
        
      }

      if(focus_val_prev != focus_val) {
        M5.Lcd.setTextDatum(TL_DATUM);
        M5.Lcd.drawFastVLine(70, 175, 10, TFT_WHITE); // focus mark min
        M5.Lcd.drawFastVLine(250, 175, 10, TFT_WHITE); // focus mark max
        
        M5.Lcd.drawFastVLine(map(focus_val_prev, 0, 999, 70, 250), 175, 10, TFT_BLACK); // clear prev focus mark
        M5.Lcd.drawFastHLine(70, 180, 180, TFT_WHITE); // focus line
        M5.Lcd.drawFastVLine(map(focus_val, 0, 999, 70, 250), 175, 10, TFT_WHITE); // focus current
        focus_val_prev = focus_val;
      }


      
}

bool load_lens_data(struct LensData* LensData) {
    StaticJsonDocument<900> doc;
    char json[] =   "{ \"cal_data\": [ [0.45, 0], [0.5, 145], [0.7, 429], [1, 629], [1.5, 761], [3, 874], [999, 999] ], \"lens\": \"Nikon AF Nikkor 50mm f\/1.8D\" }";
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
    }
    strcpy(LensData->lens_name, doc["lens"]);
    copyArray(doc["cal_data"], LensData->cal_data);
    return true;
}


double mapf(double val, double in_min, double in_max, double out_min, double out_max) {
    return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}



void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  
  //Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());  
          //initializes the UDP state
          //This initializes the transfer buffer
          udp.begin(WiFi.localIP(),udpPort);
          connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
      default: break;
    }
}
