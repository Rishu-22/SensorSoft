#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"

// Replace with your network credentials
const char* ssid = "Shiv";
const char* password = "123456789";

#include <DHT.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(D3, DHT11);
#define Buzzer D0
#define MQ2 A0

void gassensor();
void DHT11sensor(); 


float humidity,temperature;
int gasResistance;
unsigned long lastTime = 0;  
unsigned long timerDelay = 2000;  // send readings timer


AsyncWebServer server(80);
AsyncEventSource events("/events");

    <h3>GAS WEB SERVER</h3>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card temperature">
        <h4><i class="fas fa-thermometer-half"></i> TEMPERATURE</h4><p><span class="reading"><span id="temp">%TEMPERATURE%</span> &deg;C</span></p>
      </div>
      <div class="card humidity">
        <h4><i class="fas fa-tint"></i> HUMIDITY</h4><p><span class="reading"><span id="hum">%HUMIDITY%</span> &percnt;</span></p>
      </div>
      //<div class="card pressure">
       // <h4><i class="fas fa-angle-double-down"></i> PRESSURE</h4><p><span class="reading"><span id="pres">%PRESSURE%</span> hPa</span></p>
      //</div>
      <div class="card gas">
        <h4><i class="fas fa-wind"></i> GAS</h4><p><span class="reading"><span id="gas">%GAS%</span> K&ohm;</span></p>
      </div>
    </div>
  </div>
<script>


String processor(const String& var){
  gassensor();DHT11sensor();
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(temperature);
  }
  else if(var == "HUMIDITY"){
    return String(humidity);
  }
  else if(var == "GAS"){
    return String(gasResistance);
  }
  return String();
}

void setup() 
{
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(Buzzer, OUTPUT);
  dht.begin();
  
  
  // Set device as a Wi-Fi Station
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Setting as a Wi-Fi Station..");
  }
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  lcd.setCursor(0, 0);
  lcd.print("  IP Address   ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(4000);
  lcd.clear();
  
}
//Get the MQ2 sensor values
void gassensor() 
{
  gasResistance = analogRead(MQ2);
  Serial.println(gasResistance);
  gasResistance = map(gasResistance, 0, 1024, 0, 100);
  if (gasResistance <= 55) 
  {
    digitalWrite(Buzzer, LOW);
  } else if (gasResistance > 55) 
  {
    digitalWrite(Buzzer, HIGH);
  }
  
  lcd.setCursor(0, 0);
  lcd.print("G :             ");
  lcd.print(gasResistance);
}
//Get the DHT11 sensor values
void DHT11sensor() 
{
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) 
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  lcd.setCursor(0, 1);
  lcd.print("T:      ");
  lcd.print(temperature);
  lcd.setCursor(8, 1);
  lcd.print("H:      ");
  lcd.print(humidity);
}

void loop() 
{
  if ((millis() - lastTime) > timerDelay)
  {  
    gassensor();
    DHT11sensor();
    // Send Events to the Web Server with the Sensor Readings
    events.send("ping",NULL,millis());
    events.send(String(temperature).c_str(),"temperature",millis());
    events.send(String(humidity).c_str(),"humidity",millis());
    events.send(String(gasResistance).c_str(),"gas",millis());
    lastTime = millis();
  }
 
}
