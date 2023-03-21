// library

#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include<string.h>
#include <Arduino_JSON.h>


// sensor pinout
#define DHTPIN 14
#define DHTTYPE DHT11 
#define CO  32
#define soil 34
#define ledRelay 4
#define WaterRelay 2
#define Co2Relay 5





// variables to store sensor data
float temp;
float humi;
int most;
int partpermillion;

// define wifi ssid and password
const char* ssid = "gourav";
const char* password = "12345Gg@";

//api server
const char* serverName = "https://sensordata.cyclic.app/api/v1/64149cd2d662644901ed4f7f";
const char* serverLed = "https://sensordata.cyclic.app/api/v1/bulb/6414a83c7697094aed555f58";

// defined dht object
DHT dht(DHTPIN, DHTTYPE);

// defined time delays
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;



void setup() {

  wifiinit();

  dht.begin();
  pinMode(ledRelay, OUTPUT);
  pinMode(WaterRelay, OUTPUT);
  pinMode(Co2Relay, OUTPUT);


}

void loop() {

   humtep();


   gas_sensor();

   soil_moisture();


  

  Serial.println();
  Serial.println();
  Serial.println();
  dataSending();

  delay(1500);


  Relay();

}

void humtep(){
   humi = dht.readHumidity();
   temp = dht.readTemperature();

  Serial.print(F("Humidity: "));
  Serial.print(humi);
  Serial.print(F("%  Temperature: "));
  Serial.print(temp);
  Serial.println(F("°C "));
}

void gas_sensor(){
  partpermillion = analogRead(CO);
  Serial.print("Gas Sensor: ");  
  Serial.print(partpermillion);   
  Serial.println("");
}

void soil_moisture(){
  
  most= analogRead(soil);
  most = map(most,4095,0,0,100);
  
  Serial.print("Moisture : ");
  Serial.print(most);
  Serial.println("%");
}

void wifiinit(){
    Serial.begin(115200);2

  WiFi.begin(ssid, password);
  Serial.println("Connecting");

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void dataSending(){
    if ((millis() - lastTime) > timerDelay) {
    
    if(WiFi.status()== WL_CONNECTED){

      HTTPClient http;
    

      http.begin(serverName);
      
   
      http.addHeader("Content-Type", "application/json");
      Serial.println(temp);
      Serial.println(humi);
      Serial.println(most);
      Serial.println(partpermillion);

      String t = String(temp);
      String h = String(humi);
      String m = String(most);
      String p = String(partpermillion);  


      int httpResponseCode = http.PUT("{\"temperature\":" + t + ",\"humidity\":" + h + ",\"moisture\":" + m+  ",\"ppm\":" + p + "}");

      Serial.println(temp);
      Serial.println(humi);
      Serial.println(most);
      Serial.println(partpermillion);
      Serial.println(t);
      Serial.println(h);
      Serial.println(m);
      Serial.println(p);


   
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }
}

void Relay(){
  String payload = "{}";
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http1;

      String serverPath = "https://sensordata.cyclic.app/api/v1/bulb/6414de2256ee464763b2b0a4";
      
      // Your Domain name with URL path or IP address with path
      http1.begin(serverPath);
      
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      
      // Send HTTP GET request
      int httpResponseCode = http1.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
         payload = http1.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http1.end();
    }
    
  JSONVar myObject = JSON.parse(payload);

    // JSON.typeof(jsonVar) can be used to get the type of the var
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }

    Serial.print("JSON object = ");
    Serial.println(myObject);
    JSONVar keys = myObject.keys();
    Serial.println("---------------------------------->");
    String demo  = myObject[keys[0]];
    Serial.println(myObject[keys[0]]);
    Serial.println(demo);
    if(demo == "true"){
      Serial.println("true---->>");
        digitalWrite(ledRelay,HIGH);
    }
    else if (demo == "false"){
      Serial.println("---->>False");
        digitalWrite(ledRelay,LOW);

    }




    
  if(partpermillion <=0){
      Serial.println("true---->>");
        digitalWrite(Co2Relay,HIGH);
    }
    else if (partpermillion>=900){
      Serial.println("---->>False");
        digitalWrite(Co2Relay,LOW);

    }


    
    if(most <=0){
      Serial.println("true---->>");
        digitalWrite(WaterRelay,HIGH);
        delay(5000);
            digitalWrite(WaterRelay,LOW);
    }
   


    } 
          





  


    





