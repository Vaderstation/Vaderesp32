#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <WiFi.h>
#include "MQ135.h"
#include "DHT.h"

#define RZERO 76.63
#define RLOAD 10

const int dhtPin = 5; // define the sensor pin
const int dhtType = DHT22; // define the sensor type (DHT22 is also known as AM2302)
DHT dht(dhtPin, dhtType); // create a DHT object

//--------------------

const int sensorPin = 34; // needs to be 34 when wifi is used
MQ135 gasSensor = MQ135(sensorPin,RZERO,RLOAD); // create an MQ135 object

//--------------------

const char* ssid = "esp32komhit";
const char* password = "vaderstation";

char user[] = "dbpro"; // MySQL user login username
char passwordSQL[] = "dbprodbpro"; // MySQL user login password

MySQL_Cursor *cur_mem;
int sqlFail = 0;

IPAddress server_addr(192,168,137,1); // IP of the MySQL server here

WiFiServer  server(80);
void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  Serial.println("Setup done");
  Serial.begin(9600); //115200 For sql
  dht.begin();

}

void loop() {
  float gasConcentration = gasSensor.getPPM(); // read the gas concentration in ppm
  float temperature = dht.readTemperature(); // read the temperature in degrees Celsius
  float humidity = dht.readHumidity(); // read the humidity in percent
  Serial.println("PPM ");
  Serial.println(gasConcentration); // print the gas concentration to the serial monitor
  delay(1000); // wait for 1 second
  WiFi.begin(ssid, password);
  delay(1000); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
    sqlFail++;
    if (sqlFail>8) {
       ESP.restart();
    }
  }
  WiFiClient client;
  MySQL_Connection conn((Client *)&client);
  
    if (conn.connect(server_addr, 3306, user, passwordSQL)) {
      Serial.println("Database connected.");
      // Initiate the query class instance
      cur_mem = new MySQL_Cursor(&conn);
      for (int i =1; i<4; i++) {
          String stringOne = "INSERT INTO test.Measure VALUES (NOW(), ";
           if (i==1) {
              stringOne += temperature;
           }
           if (i==2) {
              stringOne += humidity;
           }
           if (i==3) {
              stringOne += gasConcentration;
           }
          stringOne += ", ";
          stringOne += i;
          stringOne += ");";
          char INSERT_SQL[80];
          stringOne.toCharArray(INSERT_SQL, 80);
      
          cur_mem->execute(INSERT_SQL);
          Serial.println("Executed SQL ---");
          delay(1000);
        }
        conn.close();
    }
    else{
      Serial.println("Connection failed.");
    }
  
}
