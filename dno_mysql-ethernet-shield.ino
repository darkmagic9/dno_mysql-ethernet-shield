/*
  MySQL Connector/Arduino Example : query results

  This example demonstrates how to issue a SELECT query and how to read columns
  and rows from the result set. Study this example until you are familiar with how to
  do this before writing your own sketch to read and consume query results.
  
  For more information and documentation, visit the wiki:
  https://github.com/ChuckBell/MySQL_Connector_Arduino/wiki.

  NOTICE: You must download and install the World sample database to run
          this sketch unaltered. See http://dev.mysql.com/doc/index-other.html.

  INSTRUCTIONS FOR USE

  1) Change the address of the server to the IP address of the MySQL server
  2) Change the user and password to a valid MySQL user and password
  3) Connect a USB cable to your Arduino
  4) Select the correct board and port
  5) Compile and upload the sketch to your Arduino
  6) Once uploaded, open Serial Monitor (use 115200 speed) and observe

  Note: The MAC address can be anything so long as it is unique on your network.

  Created by: Dr. Charles A. Bell
*/
#include <Ethernet.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <avr/wdt.h>

String result; 
String temp = "0"; 
int led = 3;
int led1 = 7;
int vr = A5;

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress server_addr(192,168,1,128);  // IP of the MySQL *server* here
char user[] = "namiki";              // MySQL user login username
char password[] = "namiki";        // MySQL user login password

// Sample query
char query[] = "SELECT * FROM `fidesla`.`account` WHERE `ACCOUNT_NO` = 19 LIMIT 12";

EthernetClient client;
MySQL_Connection conn((Client *)&client);

void setup() {
  wdt_enable(WDTO_8S);
  
  pinMode(led1, OUTPUT);
  pinMode(led, OUTPUT);
  
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect

  if (Ethernet.begin(mac_addr) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for (;;)
      ;
  }
  // print your local IP address:
  Serial.print("IP = ");
  Serial.println(Ethernet.localIP());
  
  Serial.println("Connecting...");
  if (conn.connect(server_addr, 3306, user, password)) {
    delay(1000);
  }
  else
    Serial.println("Connection failed.");
}

void loop() {
  delay(2000);

  Serial.println("\nRunning SELECT and printing results\n");

  if (conn.connected()) {
    // Initiate the query class instance
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    // Execute the query
    cur_mem->execute(query);
    // Fetch the columns and print them
    column_names *cols = cur_mem->get_columns();
    for (int f = 0; f < cols->num_fields; f++) {
      Serial.print(cols->fields[f]->name);
      if (f < cols->num_fields-1) {
        Serial.print(", ");
      }
    }
    Serial.println();
    // Read the rows and print them
    row_values *row = NULL;
    do {
      row = cur_mem->get_next_row();
      if (row != NULL) {
        result = row->values[1];
        for (int f = 0; f < cols->num_fields; f++) {
          Serial.print(row->values[f]);
          if (f < cols->num_fields-1) {
            Serial.print(", ");
          }
        }
        Serial.println();
      }
    } while (row != NULL);
    // Deleting the cursor also frees up memory used
    delete cur_mem;
  
    int val = analogRead(vr);
    int output = map(val, 0, 1023, 0, 255);
    
    Serial.print(output);
    Serial.print("\t");
    Serial.print(val);
    Serial.print("\n");
  
    if (temp != result || output == 0) {
      temp = result;
      analogWrite(led, output);
      if (result == "ON") {
        Serial.println("LED : " + result);
      } else {
        Serial.println("LED : " + result);
      }
    }
  
    if (result == "ON") {
      digitalWrite(led1, HIGH);
    } else if (result == "OFF") {
      digitalWrite(led1, LOW);
    } else {
      Serial.println("Show value : " + result);
    }  
    wdt_reset(); //Reset การจับเวลาใหม่
  } else {
    Serial.println("Connect failed. Trying again on next iteration.");
    conn.close();
  }
}
