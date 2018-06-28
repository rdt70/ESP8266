#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>


#define D0 16 //
#define D1 5  // I2C Bus SCL (clock)
#define D2 4  // I2C Bus SDA (data)
#define D3 0
#define D4 2  // Same as "LED_BUILTIN", but inverted logic
#define D5 14 // SPI Bus SCK (clock)
#define D6 12 // SPI Bus MISO
#define D7 13 // SPI Bus MOSI
#define D8 15 // SPI Bus SS (CS)
#define D9 3  // RX0 (Serial console)
#define D10 1 // TX0 (Serial console)

const char* ssid = "SSID here";
const char* password = "PWD here";
const String IFTTT_key = "IFTTT_Webook Key";
const String IFTTT_Event_Armed = "AlarmArmed";
const String IFTTT_Event_UnArmed = "AlarmUnArmed";
const String IFTTT_Event_Alarm = "Alarm";
const String IFTTT_Event_Failure = "Failure";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;
  
const int AlarmIntrusionPin  = D1; 	// -> To alarm connector 26 [NA] - OnSleep/OnAlarm = Not connected/Ground => INPUT_PULLUP 
const int AlarmArmedPin   = D2; 	// -> To alarm connector 32 [+OFF] - OnSleep/Armed = 12v/Not Connected => INPUT (with voltage divider)
const int AlarmFailurePin = D3; 	// -> To alarm connector 22 [OUT ] - OnSleep/OnFailure = Not connected/Ground => INPUT_PULLUP
const int led = D4;     
bool armedSent = 0;
bool alarmSent = 0;
bool failureSent = 0;

WiFiClient wifiClient;

void setup() {
  Serial.begin(115200);
  Serial.println("."); 
  
  checkWiFiConnect(); // connect to WiFi

  server.begin();

  // define pinmode
  pinMode(AlarmIntrusionPin, INPUT_PULLUP);	// To ground if active (intrusion detected)
  pinMode(AlarmArmedPin, INPUT);  		// Not connected if active (require external voltage divider) => INPUT
  pinMode(AlarmFailurePin, INPUT_PULLUP); 	// To ground if active (failure detected)
  pinMode(led, OUTPUT);

  
}

void checkWiFiConnect() {
	Serial.print("Connecting to ");
	Serial.println(ssid); 
  
	delay(10);
  
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
  
	Serial.println("");
	Serial.println("WiFi connected");  
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}

// =================== Send Alarm to IFTTT ===========================
void sendAlarmtoIFTTT (String event) {
	WiFiClient client;
  	const char* host = "maker.ifttt.com";
	String urlIFTTT = String("/trigger/") + event + "/with/key/" + IFTTT_key;

	checkWiFiConnect(); // to verify still connected to WiFi
	
	if (!client.connect(host, 80)) {
    		Serial.println("connection failed");
    		return;
  	}
  	Serial.println("Connection Established");
  	// This will send the request to the server
  	client.print(String("GET ") + urlIFTTT + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");

	delay(10);

  	// Read all the lines of the reply from server and print them to Serial
  	while(client.available()){
    		String line = client.readStringUntil('\r');
    		Serial.print(line);
  	}
}

// =================== Send POST Alarm to IFTTT ===========================
void sendPOSTAlarmtoIFTTT (String event) {
	WiFiClient client;
  	const char* host = "maker.ifttt.com";
	String urlIFTTT = String("/trigger/") + event + "/with/key/" + IFTTT_key;

	checkWiFiConnect(); // to verify still connected to WiFi
	
	if (!client.connect(host, 80)) {
    		Serial.println("connection failed");
    		return;
  	}
  	Serial.println("Connection Established");
  	// from : https://github.com/esp8266/Arduino/issues/1390
	String PostData = "value1=192.168.1.1&value2=param2Value&value3=param3Value"; //Parameters here
	client.println(String("POST ") + urlIFTTT + " HTTP/1.1")
	client.println("Host: " + host)
	//client.println("Cache-Control: no-cache");	// not required
	client.println("Content-Type: application/x-www-form-urlencoded");
	client.print("Content-Length: "); 
	client.println(PostData.length()); 	// example should be 56
	client.println();					//required before data
	client.println(PostData);
	client.println(); 					// ----- added to verify if close the request
	
	delay(10);

  	// Read all the lines of the reply from server and print them to Serial
  	while(client.available()){
    		String line = client.readStringUntil('\r');
    		Serial.print(line);
  	}
}

String pageHead() {
	String htmlHead ="";
	
	htmlHead += "<!DOCTYPE html>";
	htmlHead += "<html>";
	htmlHead += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
	htmlHead += "<link rel=\"icon\" href=\"data:,\">";

	htmlHead += "<style> ";
	htmlHead += "html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}";
	htmlHead += ".buttonON { background-color: #f44336; border: none; color: white; padding: 15px 32px; text-decoration: none; font-size: 16px; margin: 2px; cursor: pointer;}";
	htmlHead += ".buttonOFF {background-color: #4CAF50;}";
	htmlHead += "</style>";
	htmlHead += "<meta http-equiv=\"refresh\" content=\"5\">";
	htmlHead += "</head>";
	
	return htmlHead;

} // pageHead

// Add a button to the HTML page based on the value in "OnOff variable
String pageButton(String pinName, int OnOff) {
	String buttonStr = "";
  
	buttonStr += "<p>" + pinName + " Status </p>";
	
	switch (OnOff)
	{
		case 1:
			buttonStr += "<p><a href=\"/" + pinName + "/OFF\"><button class=\"buttonON\">" + pinName + "</button></a></p>";
		break;
		case 0:
			buttonStr += "<p><a href=\"/" + pinName + "/ON\"><button class=\"buttonON buttonOFF\">" + pinName + "</button></a></p>";
		break;
	}

  Serial.println(buttonStr);
	return buttonStr;
	
} // pageButton

String clientAction(String header, String action)  {
// Include here any action required on specific input from client
	String outputState = ""; 

	if (header.indexOf("GET /" + action +"/ON") >= 0) {
        Serial.println(action + " ON");
        outputState = "ON";
	}
	if (header.indexOf("GET /" + action +"/OFF") >= 0) {
        Serial.println(action + " OFF");
        outputState = "OFF";
	}
	return outputState;
	
} // clientAction

String readValue(String header, String parameter)  {
// Include here any action required on specific input from client
	String value = ""; 
	int start = 0;
	int end = 0;
	int length = header.length();
	int found = header.indexOf(parameter);
	
	if (found >= 0) {
		start = header.indexOf("=", found + 1); // start
		end = max(header.indexOf("&", found + 1), length); //end
		value = header.substring(start + 1, end); 
		Serial.println(parameter + "=" + value);
	}

	return value;	
} // readValue

// =================== Web Server ===========================
void webServerListen() {
  // Variable to store the HTTP request
  String header;

  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
	     // Analize the client query
			Serial.println(clientAction(header,"button1")); // Do some action here instead of print...
			Serial.println(clientAction(header,"button2")); // Do some action here instead of print...
			Serial.println(clientAction(header,"button3")); // Do some action here instead of print...
			
            // Display the HTML web page
            client.println(pageHead());
                        		
            // Web Page Heading
            client.println("<body><h1>EURO5K Alarm Web Server</h1>");
            
			client.println(pageButton("Intrusion", !digitalRead(AlarmIntrusionPin)));
			client.println(pageButton("Armed", !digitalRead(AlarmArmedPin)));
			client.println(pageButton("Failure", !digitalRead(AlarmFailurePin)));
			
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else {                // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {   // if you got anything else but a carriage return character,
          currentLine += c;       // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
} // webServerListen


// ======================= Check the Alarm Status ============================
void checkAlarm() {
  // put your main code here, to run repeatedly:

  digitalWrite(led, LOW);
  
  delay(1000);
  
  bool OnAlarm  = !digitalRead(AlarmIntrusionPin);
  bool Armed    = !digitalRead(AlarmArmedPin);
  bool Failure  = !digitalRead(AlarmFailurePin);

  Serial.print("PIN D1 = "); Serial.print(!OnAlarm);
  
  //============= Check if alarm raised
  if (OnAlarm && !alarmSent) {
        Serial.println("Centrale ALLARME!");
        sendAlarmtoIFTTT(IFTTT_Event_Alarm);
         alarmSent = 1;  //Remember if already sent to avoid sending twice
  } else { alarmSent = OnAlarm; } // alarmSent = 0 se non allarme altrimenti rimane 1
  Serial.println();

  //============= Check if alarm armed
  Serial.print("PIN D2 = "); Serial.print(!Armed);
  if (Armed && !armedSent) {
       Serial.println("Centrale Inserita!");
      sendAlarmtoIFTTT(IFTTT_Event_Armed); 
      armedSent = 1;    //Remember if already sent to avoid sending twice
  } else {
      if (!Armed && armedSent) {
          Serial.println("Centrale Disinserita!");
          sendAlarmtoIFTTT(IFTTT_Event_UnArmed);
      }
      armedSent = Armed; //armedSent = 0 se non Armed altrimenti rimane 1 
  }
  Serial.println();
  
  //============= Check if failure (for example if no power)
  Serial.print("PIN D3 = "); Serial.print(!Failure);
  if (Failure && !failureSent) {
        Serial.println("FAILURE!");
        sendAlarmtoIFTTT(IFTTT_Event_Failure); 
        failureSent = 1; //Remember if already sent to avoid sending twice
  } else {
    failureSent = Failure;   //da ora si può mandare nuova segnalazione
  }
  Serial.println();
  Serial.println(WiFi.localIP());
  
  Serial.println("-------------------");
  
  digitalWrite(led, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:

  checkAlarm();
  
  delay(2000);

  webServerListen();

  //ESP.deepSleep(10);

}
