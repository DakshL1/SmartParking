#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <Wire.h>

const char* ssid = "gg"; // Replace with your WiFi SSID
const char* password = "dhruv12345"; // Replace with your WiFi password

#define TRIG1 D5
#define ECHO1 D7
#define TRIG2 D6
#define ECHO2 D8

WiFiServer server(80);
LiquidCrystal_I2C lcd(0x27, 16, 2); 

unsigned long previousMillis = 0; // Timer for non-blocking LCD updates
const unsigned long lcdInterval = 4000; // Time interval for each LCD update (4 seconds)
int lcdState = 0; // Current state for LCD updates

void setup() {
  Serial.begin(9600);
  Serial.println("Setup started");

  // Initialize LCD
  lcd.begin();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Setup started");

  // Initialize sensor pins
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  lcd.setCursor(0, 1);
  lcd.print("Connecting...");

  WiFi.begin(ssid, password);

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) { // Timeout after 10 seconds
    delay(100);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println();
    Serial.println("Failed to connect to Wi-Fi");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connect Fail");
  } else {
    Serial.println();
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connected:");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
  }

  // Start the server
  server.begin();
  Serial.println("Server started");
}

long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2;

  return distance;
}

void updateLCD(bool slot1Occupied, bool slot2Occupied, bool slot3Occupied, bool slot4Occupied, bool slot5Occupied) {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= lcdInterval) {
    previousMillis = currentMillis;
    lcd.clear();

    switch (lcdState) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("Slot 1: ");
        lcd.print(slot1Occupied ? "Occupied" : "Empty");
        lcd.setCursor(0, 1);
        lcd.print("Slot 2: ");
        lcd.print(slot2Occupied ? "Occupied" : "Empty");
        break;

      case 1:
        lcd.setCursor(0, 0);
        lcd.print("Slot 3: ");
        lcd.print(slot3Occupied ? "Occupied" : "Empty");
        lcd.setCursor(0, 1);
        lcd.print("Slot 4: ");
        lcd.print(slot4Occupied ? "Occupied" : "Empty");
        break;

      case 2:
        lcd.setCursor(0, 0);
        lcd.print("Slot 5: ");
        lcd.print(slot5Occupied ? "Occupied" : "Empty");
        break;
    }

    // Move to the next state
    lcdState = (lcdState + 1) % 3; // Cycle between 0, 1, 2
  }
}

void loop() {
  WiFiClient client = server.available(); // Check for incoming client
  bool slot1Occupied = false;
  bool slot2Occupied = false;
  bool slot3Occupied = false;
  bool slot4Occupied = false;
  bool slot5Occupied = false;

  if (client) {
    Serial.println("New Client.");
    String request = client.readStringUntil('\r');
    Serial.println("Request: " + request); // Log the request
    client.flush();

    long distance1 = getDistance(TRIG1, ECHO1);
    long distance2 = getDistance(TRIG2, ECHO2);

    // Simulate distances for additional slots
    long distance3 = 200; // Simulated distance > hideThreshold (e.g., 150 cm)
    long distance4 = 200; // Simulated distance > hideThreshold
    long distance5 = 200; // Simulated distance > hideThreshold

    // Threshold distance to determine if a slot is occupied (e.g., 10 cm)
    bool slot1Occupied = distance1 < 10;
    bool slot2Occupied = distance2 < 10;
    bool slot3Occupied = distance3 < 10;
    bool slot4Occupied = distance4 < 10;
    bool slot5Occupied = distance5 < 10;

    // Print the distances and statuses to the serial monitor
    // Serial.print("Distance from Sensor 1: ");
    // Serial.print(distance1);
    // Serial.println(" cm");
    // Serial.print("Slot 1 is ");
    // Serial.println(slot1Occupied ? "Occupied" : "Empty");

    // Serial.print("Distance from Sensor 2: ");
    // Serial.print(distance2);
    // Serial.println(" cm");
    // Serial.print("Slot 2 is ");
    // Serial.println(slot2Occupied ? "Occupied" : "Empty");

    // Serial.print("Distance for Slot 3: ");
    // Serial.print(distance3);
    // Serial.println(" cm");
    // Serial.print("Slot 3 is ");
    // Serial.println(slot3Occupied ? "Occupied" : "Empty");

    // Serial.print("Distance for Slot 4: ");
    // Serial.print(distance4);
    // Serial.println(" cm");
    // Serial.print("Slot 4 is ");
    // Serial.println(slot4Occupied ? "Occupied" : "Empty");

    // Serial.print("Distance for Slot 5: ");
    // Serial.print(distance5);
    // Serial.println(" cm");
    // Serial.print("Slot 5 is ");
    // Serial.println(slot5Occupied ? "Occupied" : "Empty");

    // Update the LCD display
    updateLCD(slot1Occupied, slot2Occupied, slot3Occupied, slot4Occupied, slot5Occupied);

    // Send JSON response with the slot status
    String jsonResponse = "{ \"distance1\": " + String(distance1) +
                          ", \"distance2\": " + String(distance2) +
                          ", \"distance3\": " + String(distance3) +
                          ", \"distance4\": " + String(distance4) +
                          ", \"distance5\": " + String(distance5) + " }";

    // Send CORS headers
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Access-Control-Allow-Origin: *"); // Allow all origins
    client.println("Connection: close");
    client.println();
    client.println(jsonResponse);

    delay(1);
    Serial.println("Client disconnected.");
  }

  // Continuously update the LCD display
  // updateLCD(slot1Occupied, slot2Occupied, slot3Occupied, slot4Occupied, slot5Occupied);
}
