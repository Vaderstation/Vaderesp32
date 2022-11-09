#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <WiFi.h>

const char* ssid = ""; // Wifi name
const char* password = ""; // Wifi password

char user[] = ""; // MySQL user login username
char passwordSQL[] = ""; // MySQL user login password

char INSERT_SQL[] = "INSERT INTO test.testarn (id, namee) VALUES ('1', 'test');";

IPAddress server_addr(192,168,137,1); // IP of the MySQL server here


WiFiServer  server(80);
void setup() {
  Serial.begin(115200);

  connectToNetwork();
  mySqlLoop();
}

void loop() {
}
void connectToNetwork() {
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
 
  Serial.println("Connected to network");
 
}
void mySqlLoop(){
  WiFiClient client;
  MySQL_Connection conn((Client *)&client);
  if (conn.connect(server_addr, 3306, user, passwordSQL)) {
    Serial.println("Database connected.");
  }
  else{
    Serial.println("Connection failed.");
    Serial.println("Recording data.");
  }
  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Execute the query
  cur_mem->execute(INSERT_SQL);
  // Note: since there are no results, we do not need to read any data
  // Deleting the cursor also frees up memory used
  delete cur_mem;
  Serial.println("closing connection\n");
  //client.stop();
}
