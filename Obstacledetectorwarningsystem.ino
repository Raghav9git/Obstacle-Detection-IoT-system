String ssid     = "Simulator Wifi";  // WiFi network name to connect the ESP8266 module
String password = "";                 // Password for the WiFi network (empty means no password)
String host     = "api.thingspeak.com"; // Server to send sensor data to (ThingSpeak API)
const int httpPort   = 80;                // Standard HTTP port
String url     = "/update?api_key=YOUR_API_KEY&field1="; // API endpoint to update ThingSpeak data


#include<LiquidCrystal.h>           // Library for LCD display control
LiquidCrystal lcd(12,11,5,4,3,2);  // Initialize LCD pins (RS, E, D4, D5, D6, D7)

#include <SoftwareSerial.h>         // Enable serial communication on other pins
SoftwareSerial espSerial(8, 9);    // RX and TX pins for communicating with ESP8266 module


int trigPin = A5;                  // Trigger pin for ultrasonic sensor (output)
int echoPin = A4;                  // Echo pin for ultrasonic sensor (input)
int BuzzerLed = 7;                 // Pin connected to buzzer and LED indicator

long distance;                    // Variable to hold measured distance value
long duration;                    // Duration of ultrasonic pulse travel


// Function to initialize ESP8266 and connect it to WiFi network
int setupESP8266(void) {
  Serial.println("Connecting to WiFi...");
  espSerial.begin(9600);           // Start serial comms with ESP8266 at 9600 baud
  espSerial.println("AT");         // Send simple AT command to check if ESP is responsive
  delay(5000);                    // Wait for ESP8266 to process and respond
  if (!espSerial.find("OK")) return 1;  // If no OK response, fail early
    
  // Attempt to connect to the specified WiFi network
  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
  delay(5000);                   // Wait for connection attempt
  if (!Serial.find("OK")) return 2;  // Fail if WiFi connection not successful
  
  return 0;                     // Return 0 on success
}


// Sends distance data to ThingSpeak server using HTTP GET request
void anydata(long distance) {
  
  Serial.println("Connecting to ThingSpeak...");
  
  // Open TCP connection to ThingSpeak API host
  Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
  delay(2000);
  if (!Serial.find("OK")){ return; }  // If connection fails, exit early
  
  // Construct HTTP GET request with current distance value
  String httpPacket = "GET /update?api_key=S0A7GHQUNOB1L1IS&field1=" + String(distance) + " HTTP/1.1\r\nHost: api.thingspeak.com\r\n\r\n";
  int length = httpPacket.length();
  
  // Notify ESP8266 of the length of data to send
  Serial.print("AT+CIPSEND=");
  Serial.println(length);
  delay(2000);                   // Wait for prompt to send actual data
  if (!Serial.find(">")) return;  // Ensure ESP is ready to receive
  
  // Send the HTTP GET request
  Serial.print(httpPacket);
  delay(2000);                  // Wait for data transmission
  if (!Serial.find("SEND OK\r\n")) {return;} // Confirm data sent successfully
  
  Serial.println("Data sent successfully.");

  // Close the TCP connection
  Serial.println("AT+CIPCLOSE");
  delay(1000);
}


// Triggers an IFTTT webhook when an obstacle is detected within range
void triggerIFTTT() {
  String eventName = "obstacle_detected";    // Name of IFTTT event to trigger
  String key = "My_IFTTT_APIKEY";             // User's unique IFTTT Webhooks API key (replace with actual key)
  String server = "maker.ifttt.com";          // IFTTT Webhooks server address
  
  // Build the HTTP GET request to trigger the event
  String request = "GET /trigger/" + eventName + "/with/key/" + key + " HTTP/1.1\r\n" +
                   "Host: " + server + "\r\n" + 
                   "Connection: close\r\n\r\n";
  
  // Open TCP connection to IFTTT server
  espSerial.println("AT+CIPSTART=\"TCP\",\"" + server + "\",80");
  delay(2000);
  if (!espSerial.find("OK")) return;  // Abort if connection fails

  // Send the length of the HTTP request
  espSerial.print("AT+CIPSEND=");
  espSerial.println(request.length());
  delay(2000);
  if (!espSerial.find(">")) return;  // Wait for prompt

  // Send the actual HTTP GET request
  espSerial.print(request);
  delay(2000);
  espSerial.println("AT+CIPCLOSE");  // Close the connection after sending
}


void setup()
{
  pinMode(A5,OUTPUT);    // Set trigPin as output to send ultrasonic pulse
  pinMode(A4,INPUT);     // Set echoPin as input to receive echo pulse
  pinMode(7,OUTPUT);     // Set buzzer/LED pin as output
  digitalWrite(BuzzerLed,LOW); // Turn buzzer/LED off initially
  Serial.begin(9600);     // Start serial communication with PC
  
  lcd.begin(16,2);       // Initialize 16x2 LCD display
  
  setupESP8266();        // Initialize WiFi module and connect to WiFi
}


void loop()
{
  // Send ultrasonic pulse by setting trigPin HIGH for 10 microseconds
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);

  // Read the echo pulse duration (timeout after 30ms to avoid lockup)
  duration = pulseIn(echoPin, HIGH, 30000); 
  // Calculate distance in cm: speed of sound / 2 (round trip), 29.1 converts to cm
  distance = (duration / 2.0) / 29.1;  

  // If obstacle is detected within 30 cm
  if(distance > 0 && distance < 30)
  {
    digitalWrite(BuzzerLed,HIGH);  // Turn on buzzer/LED as alert
    triggerIFTTT();                // Trigger IFTTT webhook for notification
  }
  else
  {
    digitalWrite(BuzzerLed,LOW);   // Turn off buzzer/LED if no obstacle nearby
  }
  
  Serial.print(distance);
  Serial.println("cm");    // Print distance value for debugging

  delay(200);              // Small delay before updating LCD

  // Display distance on LCD
  lcd.setCursor(5,0);
  lcd.print("Distance ");
  lcd.print(distance);
  lcd.setCursor(4,1);
  lcd.print("cm");

  delay(200);              // Additional delay for stable LCD updates
  
  anydata(distance);       // Send distance data to ThingSpeak

  delay(16000);            // Wait 16 seconds between each data upload to respect API rate limits
}
