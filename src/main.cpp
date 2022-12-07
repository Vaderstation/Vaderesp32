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

const int sensorPin = 12; // define the sensor pin
MQ135 gasSensor = MQ135(sensorPin,RZERO,RLOAD); // create an MQ135 object

//--------------------

const char* ssid = "esp32komhit";
const char* password = "vaderstation";

char user[] = "dbpro"; // MySQL user login username
char passwordSQL[] = "dbprodbpro"; // MySQL user login password

int setupDone = 0;
MySQL_Cursor *cur_mem;

IPAddress server_addr(192,168,137,1); // IP of the MySQL server here

WiFiServer  server(80);
void setup() {
  Serial.begin(9600); //115200 For sql
  dht.begin();

  connectToNetwork();
}

void loop() {
  float gasConcentration = gasSensor.getPPM();//*10; // read the gas concentration in ppm
  float temperature = dht.readTemperature(); // read the temperature in degrees Celsius
  float humidity = dht.readHumidity(); // read the humidity in percent
  Serial.print("PPM ");
  Serial.print(gasConcentration); // print the gas concentration to the serial monitor
  Serial.print(" Temperature: ");
  Serial.print(temperature);
  Serial.print(" Fuktighet ");
  Serial.print( humidity);
  Serial.println("%");
  delay(1000); // wait for 1 second
  WiFiClient client;
  MySQL_Connection conn((Client *)&client);
  
    if (conn.connect(server_addr, 3306, user, passwordSQL)) {
      Serial.println("Database connected.");
      // Initiate the query class instance
      cur_mem = new MySQL_Cursor(&conn);
      String stringOne = "INSERT INTO test.jeppe (datum,amount) VALUES (NOW(), ";
      stringOne += humidity;
      stringOne += ");";
      char INSERT_SQL[70];
      stringOne.toCharArray(INSERT_SQL, 70);
      
      cur_mem->execute(INSERT_SQL);

    }
    else{
      Serial.println("Connection failed.");
    }
}
void connectToNetwork() {
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
 
  Serial.println("Connected to network");
 
}
