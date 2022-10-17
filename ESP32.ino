#include <Arduino.h>
#include <DHT.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

const char TWPlatformBaseURL[] ="https://PP-21123116135Z.portal.ptc.io";
const char appKey[] = "288e1379-7971-4a0e-9fcc-ad77307167c8";
#define ACCEPT_TYPE   "text/csv"  
#define CONTENT_TYPE  "application/json"  
#define ACCEPT_TYPEJS "application/json"  
#define SCREEN_WIDTH  128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
#define WL_MAC_ADDR_LENGTH 6

const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIEvjCCA6agAwIBAgIQBtjZBNVYQ0b2ii+nVCJ+xDANBgkqhkiG9w0BAQsFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
"QTAeFw0yMTA0MTQwMDAwMDBaFw0zMTA0MTMyMzU5NTlaME8xCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxKTAnBgNVBAMTIERpZ2lDZXJ0IFRMUyBS\n" \
"U0EgU0hBMjU2IDIwMjAgQ0ExMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKC\n" \
"AQEAwUuzZUdwvN1PWNvsnO3DZuUfMRNUrUpmRh8sCuxkB+Uu3Ny5CiDt3+PE0J6a\n" \
"qXodgojlEVbbHp9YwlHnLDQNLtKS4VbL8Xlfs7uHyiUDe5pSQWYQYE9XE0nw6Ddn\n" \
"g9/n00tnTCJRpt8OmRDtV1F0JuJ9x8piLhMbfyOIJVNvwTRYAIuE//i+p1hJInuW\n" \
"raKImxW8oHzf6VGo1bDtN+I2tIJLYrVJmuzHZ9bjPvXj1hJeRPG/cUJ9WIQDgLGB\n" \
"Afr5yjK7tI4nhyfFK3TUqNaX3sNk+crOU6JWvHgXjkkDKa77SU+kFbnO8lwZV21r\n" \
"eacroicgE7XQPUDTITAHk+qZ9QIDAQABo4IBgjCCAX4wEgYDVR0TAQH/BAgwBgEB\n" \
"/wIBADAdBgNVHQ4EFgQUt2ui6qiqhIx56rTaD5iyxZV2ufQwHwYDVR0jBBgwFoAU\n" \
"A95QNVbRTLtm8KPiGxvDl7I90VUwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQWMBQG\n" \
"CCsGAQUFBwMBBggrBgEFBQcDAjB2BggrBgEFBQcBAQRqMGgwJAYIKwYBBQUHMAGG\n" \
"GGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBABggrBgEFBQcwAoY0aHR0cDovL2Nh\n" \
"Y2VydHMuZGlnaWNlcnQuY29tL0RpZ2lDZXJ0R2xvYmFsUm9vdENBLmNydDBCBgNV\n" \
"HR8EOzA5MDegNaAzhjFodHRwOi8vY3JsMy5kaWdpY2VydC5jb20vRGlnaUNlcnRH\n" \
"bG9iYWxSb290Q0EuY3JsMD0GA1UdIAQ2MDQwCwYJYIZIAYb9bAIBMAcGBWeBDAEB\n" \
"MAgGBmeBDAECATAIBgZngQwBAgIwCAYGZ4EMAQIDMA0GCSqGSIb3DQEBCwUAA4IB\n" \
"AQCAMs5eC91uWg0Kr+HWhMvAjvqFcO3aXbMM9yt1QP6FCvrzMXi3cEsaiVi6gL3z\n" \
"ax3pfs8LulicWdSQ0/1s/dCYbbdxglvPbQtaCdB73sRD2Cqk3p5BJl+7j5nL3a7h\n" \
"qG+fh/50tx8bIKuxT8b1Z11dmzzp/2n3YWzW2fP9NsarA4h20ksudYbj/NhVfSbC\n" \
"EXffPgK2fPOre3qGNm+499iTcc+G33Mw+nur7SpZyEKEOxEXGlLzyQ4UfaJbcme6\n" \
"ce1XR2bFuAJKZTRei9AqPCCcUZlM51Ke92sRKw2Sfh3oius2FkOH6ipjv3U/697E\n" \
"A7sKPPcw7+uvTPyLNhBzPvOk\n" \
"-----END CERTIFICATE-----\n"; 


void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

WiFiMulti WiFiMulti;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define LIGHT_SENSOR_PIN 36
#define SOIL_POWER_PIN   17 
#define SOIL_SIGNAL_PIN  39 
#define WATER_SIGNAL_PIN 32
#define WATER_POWER_PIN 1
#define RELAY_PIN 27
#define DHT_SENSOR_PIN 19 
#define DHT_SENSOR_TYPE DHT11

DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

#define WL_MAC_ADDR_LENGTH 6
String getUniqueDeviceName(String device){ 
    String uniqueDeviceName;
    uint8_t mac[WL_MAC_ADDR_LENGTH];
    WiFi.macAddress(mac);
    String smallMacAdress = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                    String(mac[WL_MAC_ADDR_LENGTH - 1], HEX)+ "Pro";
    smallMacAdress.toUpperCase();
    if(device=="Sprinkler"){
    uniqueDeviceName = "Sprinkler-" + smallMacAdress;
    }
    else if(device=="Router") {
    uniqueDeviceName = "Router-" + smallMacAdress;
    }
    Serial.println("Device-" + uniqueDeviceName);
    return uniqueDeviceName;
}

///////////////////////////////
// makes HTTPs POST to platform to CreateThing service using input string as the new Things's name. 
///////////////////////////////
int createThing(String nameOfThing,String thingTemplateName){
 WiFiClientSecure *client = new WiFiClientSecure;
 int httpsCode= -1; 
  if(client) {
    client -> setCACert(rootCACertificate);

    {
      // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is 
      HTTPClient https;
      String fullRequestURL = String(TWPlatformBaseURL) + "/Thingworx/Resources/EntityServices/Services/CreateThing?appKey=" + String(appKey);
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client,fullRequestURL )) {  // HTTPS
        Serial.print("[createThing] POST...\n");
        // start connection and sent HTTP header
        https.addHeader("Accept",ACCEPT_TYPE,false,false);
        https.addHeader("Content-Type","application/json",false,false);
        httpsCode = https.POST("{\"name\": \""+ nameOfThing +"\",\"thingTemplateName\": \""+ thingTemplateName +"\"}");
        if (httpsCode > 0) {
          // HTTP header has been sent and Server response header has been handled
          Serial.printf("[createThing] POST... code: %d\n", httpsCode);
          // file found at server
          if (httpsCode == HTTP_CODE_OK || httpsCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println(payload);
          }
        } else {
          Serial.println(httpsCode);
          Serial.printf("[createThing] POST... failed, error: %s\n", https.errorToString(httpsCode).c_str());
        }
  
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
    }//end client
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
  return httpsCode;
}
int getHttpCode(String sprinklerName, String property){
 WiFiClientSecure *client = new WiFiClientSecure;
 int httpCode= -1;
  if(client) {
    client -> setCACert(rootCACertificate);

    {
      HTTPClient https;
      String fullRequestURL = String(TWPlatformBaseURL) + "/Thingworx/Things/"+ sprinklerName +"/Properties/"+ property +"?appKey=" + String(appKey);
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client,fullRequestURL )) {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        https.addHeader("Accept",ACCEPT_TYPE,false,false);
        httpCode = https.GET();
        if (httpCode > 0) {
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println(payload);
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
    }
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
  return httpCode;
}
/////////////////////////////////////////////////////////////
// make HTTP POST to ThingWorx server Thing service        //
// nameOfThing - Name of Thing to POST to                  //
// endPoint - Services URL to invoke                       //
// postBody - Body of POST to send to ThingWorx platform   //
// returns HTTP response code from server                  //
/////////////////////////////////////////////////////////////
int postToThing(String nameOfThing, String endPoint, String postBody){

  WiFiClientSecure *client = new WiFiClientSecure;
  int httpCode= -1;
  if(client) {
    client -> setCACert(rootCACertificate);
    {
      HTTPClient https;
      String fullRequestURL = String(TWPlatformBaseURL) + "/Thingworx/Things/"+ nameOfThing +"/Services/"+ endPoint +"?appKey=" + String(appKey);
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client,fullRequestURL )) {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        // start connection and sent HTTP header
        https.addHeader("Accept",ACCEPT_TYPE,false,false);
        https.addHeader("Content-Type","application/json",false,false);
        httpCode = https.POST(postBody);
        if (httpCode > 0) {
          Serial.printf("[postToThing] response code: %d\n", httpCode);
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println(payload);
          }
        } else {
          Serial.printf("[postToThing] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
    }
    delete client;
  } else {
    Serial.println("Unable to create client");
  }  
  return httpCode;
}

/////////////////////////////////////////////////////////
// make HTTP PUT to ThingWorx server Thing service     //
// nameOfThing - Name of Thing                         //
// propertyName - Name of property                     //
// putBody - Body of PUT to send to ThingWorx platform //
// returns HTTP response code from server              //
/////////////////////////////////////////////////////////
int putToThing(String nameOfThing, String propertyName, String putBody){
  WiFiClientSecure *client = new WiFiClientSecure;
  int httpCode= -1;
  if(client) {
    client -> setCACert(rootCACertificate);
    { 
      HTTPClient https;
      String fullRequestURL = String(TWPlatformBaseURL) + "/Thingworx/Things/"+ nameOfThing +"/Properties/"+ propertyName +"?appKey=" + String(appKey);
      if (https.begin(*client,fullRequestURL )) {  
        https.addHeader("Accept",ACCEPT_TYPE,false,false);
        https.addHeader("Content-Type",CONTENT_TYPE,false,false);
        Serial.print("[putToThing] PUT body>");
        Serial.println(putBody);
        httpCode = https.PUT(putBody);
        if (httpCode > 0) {
          Serial.printf("[HTTPS] PUT... code: %d\n", httpCode);
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println(payload);
          }
        } else {
          Serial.println(httpCode);
          Serial.printf("[HTTPS] PUT... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
    }
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
  return httpCode;
}

String postToThingValue(String nameOfThing, String endPoint, String postBody){
  WiFiClientSecure *client = new WiFiClientSecure;
  int httpCode= -1;
  String payload = "";
  if(client) {
    client -> setCACert(rootCACertificate);
    {
      HTTPClient https;
      String fullRequestURL = String(TWPlatformBaseURL) + "/Thingworx/Things/"+ nameOfThing +"/Services/"+ endPoint +"?appKey=" + String(appKey);
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client,fullRequestURL )) {  // HTTPS
        Serial.print("[HTTPS] GET...\n");
        // start connection and send HTTP header
        https.addHeader("Accept",ACCEPT_TYPEJS,false,false);
        https.addHeader("Content-Type",ACCEPT_TYPEJS,false,false);
        httpCode = https.POST(postBody);
        if (httpCode > 0) {
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            payload = https.getString();
            Serial.println(payload);
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
    }
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
  return payload;
}
///////////////////////////////
// SETUP FUNCTION            //
///////////////////////////////
void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("HotspotIoT", "a27736fa6931");
  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");
  setClock(); 
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  display.display();
  delay(1000);
  display.clearDisplay();
  dht_sensor.begin();
  pinMode(SOIL_POWER_PIN, OUTPUT);
  digitalWrite(SOIL_POWER_PIN, LOW);
  pinMode(WATER_POWER_PIN, OUTPUT);
  digitalWrite(WATER_POWER_PIN, LOW);
  pinMode(RELAY_PIN, OUTPUT);
}

///////////////////////////////
// LOOP FUNCTION             //
///////////////////////////////
void loop() {
  String sprinklerName = getUniqueDeviceName("Sprinkler");
  String routerName = getUniqueDeviceName("Router");
 
  float hum  = dht_sensor.readHumidity();
  float tempC = dht_sensor.readTemperature();
  int   soilMoisture = readSoilMoisureSensor();
  int   soilMoisturePercent = map(soilMoisture, 4095, 250, 0, 100);
  int   lightIntensity = analogRead(LIGHT_SENSOR_PIN);
  int   lightIntensityPercent = map(lightIntensity, 4095, 0, 0, 100);
  int   waterLevel = readWaterLevelSensor(); 
  int   waterLevelPercent = map(waterLevel, 0, 1100, 0, 100);
  
  displayInSerialMonitor(soilMoisture,lightIntensity,waterLevel,soilMoisturePercent,lightIntensityPercent,tempC,hum,waterLevelPercent);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  displaySoilMoisture(soilMoisturePercent);
  displayLightDependentResistor(lightIntensityPercent);
  displayTemperatureAndHumidity(tempC,hum);
  displayWaterLevel(waterLevel,waterLevelPercent);
  display.display();
  display.clearDisplay();
  
  int getResponseCodeIsExist = getHttpCode(sprinklerName, "isExist");
  if ( getResponseCodeIsExist == 404 )
      { 
        createThing(sprinklerName,"MonitoringSystem.SprinklerThingTemplate");
        postToThing(sprinklerName,"EnableThing","");
        postToThing(sprinklerName,"RestartThing","");
        delay(1000);
        putToThing(sprinklerName,"isExist","{\"isExist\":\"true\"}"); //NUMBER
        putToThing(sprinklerName,"profileStatus","{\"profileStatus\":\"mediumMoisture\"}"); //NUMBER
        putToThing(sprinklerName,"city","{\"city\":\"Kielce\"}");
        postToThing(sprinklerName,"SetLocationArduino","");
        postToThing(sprinklerName,"RestartThing",""); //POST to RestartThing endpoint with no body
        
        createThing("Router-61DCPRO","MonitoringSystem.RouterThingTemplate");
        postToThing("Router-61DCPRO","EnableThing","");
        postToThing("Router-61DCPRO","RestartThing","");
        delay(1000);
        putToThing (routerName,"city","{\"city\":\"Kielce\"}"); 
        postToThing(routerName,"SetRouterLocationArduino","");
        postToThing(routerName,"AddSprinklerFromArduino","");
        postToThing(routerName,"RestartThing",""); //POST to RestartThing endpoint with no body
      }
  putToThing(sprinklerName,"temperature","{\"temperature\" : \""+(String)tempC+"\"}");    //NUMBER
  putToThing(sprinklerName,"humidity","{\"humidity\" : \""+(String)hum+"\"}");       //NUMBER
  putToThing(sprinklerName,"lightIntensity","{\"lightIntensity\" : \""+(String)lightIntensityPercent+"\"}"); //NUMBER
  putToThing(sprinklerName,"soilMoisture","{\"soilMoisture\" : \""+(String)soilMoisturePercent+"\"}"); //NUMBER
  putToThing(sprinklerName,"waterLevel","{\"waterLevel\" : \""+(String)waterLevelPercent+"\"}"); //NUMBER
  
  String getJson = postToThingValue(sprinklerName,"SendJson","");
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, getJson);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  return;
  }
  bool irrigationStatus = doc["irrigationStatus"]; 
  bool automationStatus = doc["automationStatus"];
  bool  weatherAutomationStatus = doc["weatherAutomationStatus"];
  bool  weatherAutomationIrrigation =doc["weatherAutomationIrrigation"];
  String selectedPrifle = doc["profileStatus"];
  
  if(automationStatus==false)
   {
    if(irrigationStatus==true)
    {
      irrigationStatus=true;
      pompActive();
    }
    else if(irrigationStatus==false)
    {
      irrigationStatus=false;
      pompDeactive();
    }
   }
  else if (automationStatus==true){
   irrigationStatus = automationIrrigation(soilMoisturePercent,selectedPrifle);
  }
  putToThing(sprinklerName,"irrigationStatus","{\"irrigationStatus\" : \""+(String)irrigationStatus+"\"}"); //BOOL
    if (weatherAutomationStatus==true)
  {
    if(weatherAutomationIrrigation = true){
      pompActive();
      irrigationStatus=true;
      putToThing(sprinklerName,"irrigationStatus","{\"irrigationStatus\" : \""+(String)irrigationStatus+"\"}"); //BOOL
      delay(5000);
      pompDeactive();
      irrigationStatus=false; 
      putToThing(sprinklerName,"irrigationStatus","{\"irrigationStatus\" : \""+(String)irrigationStatus+"\"}"); //BOOL 
    }
  }
    Serial.println();
    Serial.println("next round...");
}
////////////////////////////////////////////////////////////////////////////////////////////////
  bool automationIrrigation(int soilMoisturePercent,String selectedProfile){
    boolean irrigationState=false;
    if(selectedProfile=="lowMoisture"){
      irrigationState = setProfile(soilMoisturePercent,35,40);
    }
    else if(selectedProfile=="mediumMoisture"){
      irrigationState = setProfile(soilMoisturePercent,45,50);
    }
    else if(selectedProfile=="highMoisture"){
    irrigationState = setProfile(soilMoisturePercent,55,60);
    }
    return irrigationState;
  }
  bool setProfile(int soilMoisturePercent,int firstValue,int secondValue) {
    bool irrigationState=false;
    
      if(soilMoisturePercent >=5 && soilMoisturePercent <= firstValue){
        irrigationState = true;
        pompActive();
        Serial.print("Soil Moisture: ");
        Serial.print(soilMoisturePercent);
        Serial.println("%");
        Serial.println("Pomp actived...");
      }
      else if (soilMoisturePercent >secondValue || soilMoisturePercent == 0){
        irrigationState = false;
        pompDeactive();
        Serial.print("Soil Moisture: ");
        Serial.print(soilMoisturePercent);
        Serial.println("%");
        Serial.println("Pomp deactived...");
      }
    
    return irrigationState;
  }
  void pompActive(){
    digitalWrite(RELAY_PIN, HIGH);
  }
  void pompDeactive(){
    digitalWrite(RELAY_PIN, LOW);
  }
  
  int readSoilMoisureSensor() {
    digitalWrite(SOIL_POWER_PIN, HIGH);  
    delay(10);                      
    int soilValue = analogRead(SOIL_SIGNAL_PIN); 
    digitalWrite(SOIL_POWER_PIN, LOW);
    return soilValue;     
  }
  
  int readWaterLevelSensor() {
    digitalWrite(WATER_POWER_PIN, HIGH);  
    delay(10);                      
    int waterLevel = analogRead(WATER_SIGNAL_PIN); 
    digitalWrite(WATER_POWER_PIN, LOW);
    return waterLevel;     
  }
  int displayInSerialMonitor(int soilMoisture,int lightIntensity,int waterLevel, int soilMoisturePercent, int lightIntensityPercent, float tempC, float hum, int waterLevelPercent){
    Serial.print("Soil Moisture Sensor Value: ");
    Serial.println(soilMoisture);
    Serial.print("Soil Moisture Percent Map: ");
    Serial.print(soilMoisturePercent);
    Serial.println("%");
    
    Serial.print("LDR Sensor Value: ");
    Serial.println(lightIntensity);
    Serial.print("Light Intensity Percent Map:");
    Serial.print(lightIntensityPercent);
    Serial.println("%");
    
    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.println("°C");
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.println("%");

    Serial.print("Water Level Sensor Value: ");
    Serial.println(waterLevel);
    Serial.print("Water Level Percent Map: ");
    Serial.print(waterLevelPercent);
    Serial.println("%");
  }
/////////////////////////////////////////////////////////////////////////////////////////
//OLED FUNCTIONS
  void displayWaterLevel(int waterLevel,int waterLevelPercent){
    display.setCursor(0,40);
    display.print("Water lv:");
    if(waterLevel<70){
      display.print("0");
    }
    else{
      display.print(waterLevelPercent);
    }
    display.println("%");
  }
  void displaySoilMoisture(int soilmoisturePercent){
    display.setCursor(0,0);
    display.print("Soil M:");
    display.print(soilmoisturePercent);
    display.println("%");
  }
              
  void displayLightDependentResistor(int lightDependentResistorPercent){
    display.setCursor(0,10);
    display.print("Light:");
    display.print(lightDependentResistorPercent);
    display.println("%");
  }
  void displayTemperatureAndHumidity(float temperature,float humidity){
    display.setCursor(0,20);
    display.print("Temp:");
    display.print(temperature);
    display.setTextSize(1);
    display.cp437(true);
    display.write(167);
    display.println("C");
    display.setCursor(0,30);
    display.print("Humidity:");
    display.print(humidity);
    display.println("%");
  }
