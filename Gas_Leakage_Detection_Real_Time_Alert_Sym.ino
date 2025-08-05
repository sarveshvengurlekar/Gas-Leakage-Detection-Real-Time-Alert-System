

#include <ESP8266WiFi.h> 
#include <WiFiClientSecure.h> 
#include <Servo.h> 
#include <OneWire.h> 
#include <DallasTemperature.h> 
#include <LiquidCrystal_I2C.h>  // Include the LiquidCrystal library 

// WiFi creden als 
const char* ssid = "Wifi Credentials";           
// Replace with your WiFi SSID 
const char* password = "Wifi SSID";   // Replace with your WiFi Password 
// Twilio creden als 
const char* twilioAccountSid = "twilioAccountSid"; 
const char* twilioAuthToken = "twilioAuthToken"; 
const char* twilioPhoneNumber = "twilioPhoneNumber";  // Your Twilio phone number 
const char* recipientPhoneNumber = "recipientPhoneNumber"; // Des na on phone number 
const char* server = "Twilio API server";    // Twilio API server 

// Func on to encode base64 
String base64Encode(const String &input) { 
String encoded = ""; 
char base64Table[] = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; 
int val = 0, valb = -6; 
for (unsigned char c : input) { 
val = (val << 8) + c; 
valb += 8; 
while (valb >= 0) { 
encoded += base64Table[(val >> valb) & 0x3F]; 
valb -= 6; 
    } 
  } 
  if (valb > -6) encoded += base64Table[((val << 8) >> (valb + 8)) & 0x3F]; 
  while (encoded.length() % 4) encoded += '='; 
  return encoded; 
} 
 
// Func on to send SMS 
void sendSMS() { 
  WiFiClientSecure client; 
  client.setInsecure();  // Disable SSL cer ficate valida on (not recommended for produc on) 
 
  if (client.connect(server, 443)) { 
    String message = "Alert: Gas/Fire Detec on!"; 
    String data = "To=" + String(recipientPhoneNumber) + "&From=" + String(twilioPhoneNumber) + 
"&Body=" + message; 
     
    // Create the Basic Auth string 
    String auth = String(twilioAccountSid) + ":" + String(twilioAuthToken); 
    String authEncoded = base64Encode(auth); 
 
    client.println("POST /2010-04-01/Accounts/" + String(twilioAccountSid) + "/Messages.json 
HTTP/1.1"); 
    client.println("Host: " + String(server)); 
    client.println("Authoriza on: Basic " + authEncoded); 
    client.println("Content-Type: applica on/x-www-form-urlencoded"); 
    client.print("Content-Length: "); 
    client.println(data.length()); 
    client.println(); 
    client.println(data); 
 
    while (client.connected()) { 
      String line = client.readStringUn l('\n'); 
      if (line == "\r") { 
        break; 
      } 
    } 
 
    while (client.available()) { 
      String line = client.readString(); 
      Serial.println(line);  // Print response to Serial monitor 
    } 
 
    client.stop(); 
  } else { 
    Serial.println("Connec on failed"); 
  } 
} 
 
// Constants for pin assignments 
#define MQ2_PIN A0   // Define the analog pin connected to MQ-2 
const int buzzerPin = D8;   // Pin for the buzzer 
const int servoPin = D6;    // Pin connected to the servo motor 
#define ONE_WIRE_BUS D7     // Data wire for DS18B20 is connected to GPIO4 (D7) 
#define BUTTON_PIN D5       // Pin where the pushbu on is connected (new pin) 
 
// DS18B20 Temperature sensor setup 
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire); 
Servo myServo; 
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Ini alize the LCD at address 0x27 with 20x4 display 
 
// Siren frequency parameters 
int frequencyStep = 10;      // Step by which the frequency will increase or decrease 
int maxFrequency = 1000;     // Adjusted maximum frequency for the siren 
int minFrequency = 300;      // Adjusted minimum frequency for the siren 
// Timing for MQ-2 sensor warm-up 
unsigned long warmUpTime = 120000;  // 2 minutes warming up 
unsigned long startTime; 
// Variable to ensure SMS is sent only once per alert 
bool smsSent = false; 
// Variable to track the alert state 
bool alertAc ve = false; 
void setup() { 
// Start the serial communica on 
Serial.begin(115200); 
// Ini alize buzzer, sensors, servo, and bu on 
pinMode(buzzerPin, OUTPUT); 
pinMode(ONE_WIRE_BUS, INPUT_PULLUP); // Enable internal pull-up resistor on the data pin for 
DS18B20 
sensors.begin(); // Start up the DS18B20 sensor library 
pinMode(BUTTON_PIN, INPUT_PULLUP); // Set the bu on pin as INPUT with an internal pull-up 
resistor 
pinMode(MQ2_PIN, INPUT);  // Set MQ-2 pin as input 
myServo.a ach(servoPin); 
myServo.write(0);  // Ini alize the servo posi on to 0 degrees 
// Start the LCD 
lcd.init(); 
lcd.clear(); 
  lcd.backlight();  // Make sure backlight is on 
 
  // Display system informa on 
  lcd.setCursor(7, 0); 
  lcd.print("FCRIT"); 
  lcd.setCursor(4, 1); 
  lcd.print("Gas Leakage"); 
  lcd.setCursor(2, 2); 
  lcd.print("Detec on System"); 
  delay(5000);  // Wait for 5 seconds 
  lcd.clear();  // Clear the display a er the delay 
 
  // Display MQ sensor ini alizing message for 2 minutes 
  lcd.setCursor(0, 1); 
  lcd.print("Ini alizing System"); 
  startTime = millis();  // Record the star ng me for the 2-minute countdown 
 
  // Connect to WiFi for Twilio SMS sending 
  WiFi.begin(ssid, password); 
   
  while (WiFi.status() != WL_CONNECTED) { 
    delay(1000); 
    Serial.println("Wifi Status : Connec ng to"); 
    Serial.print(ssid); 
  } 
  Serial.println("Connected to WiFi"); 
  Serial.print(ssid); 
} 
 
void loop() { 
  // MQ-2 sensor warming up phase (display ini aliza on for 2 minutes) 
  if (millis() - startTime < warmUpTime) { 
    Serial.println("Sensor Status : Ini alizing"); 
    delay(1000);  // Print every second 
    return;  // Skip rest of loop during warming up 
    lcd.clear();  // Clear LCD a er ini aliza on is done 
    Serial.print("Sensor Status : Ini alized"); 
  } 
 
  // Read the state of the pushbu on 
  int bu onState = digitalRead(BUTTON_PIN); 
 
  // DS18B20 sensor readings 
  sensors.requestTemperatures();  // Request temperature from DS18B20 sensors 
 
  // Fetch and print temperature in Celsius and Fahrenheit 
  float TempC = sensors.getTempCByIndex(0); 
  Serial.print("TempC: "); 
  Serial.print(TempC); 
  Serial.println(" °C"); 
 
  float TempF = sensors.getTempFByIndex(0); 
  Serial.print("TempF: "); 
  Serial.print(TempF); 
  Serial.println(" °F"); 
 
  // Read MQ-2 gas sensor value 
  float ppm = analogRead(MQ2_PIN);  // Analog output from MQ-2 
  Serial.print("Gas concentra on: "); 
  Serial.print(ppm); 
  Serial.println(" PPM"); 
 
  // Update the LCD with temperature and gas readings 
  lcd.setCursor(0, 0); 
  lcd.print("Gas :"); 
  lcd.setCursor(6, 0); 
  lcd.print(ppm); 
  lcd.print(" PPM");  // Update gas reading 
  lcd.setCursor(0, 1); 
  lcd.print("Temp:"); 
  lcd.setCursor(5, 1); 
  lcd.print(TempC); 
  lcd.print("C"); 
  lcd.setCursor(11,1); 
  lcd.print(" "); 
  lcd.setCursor(12, 1); 
  lcd.print("|"); 
  lcd.print(TempF); 
  lcd.print("F"); 
 
  // Check if bu on is pressed or other alert condi ons are met 
  if (bu onState == LOW || (TempC >= 45 && ppm >= 225)) { 
    Serial.println("Bu on State : 1"); 
    Serial.println("Trigger Alert: SOS Bu on"); 
    triggerAlert(); // Perform alert ac ons (servo, buzzer, LCD, SMS) 
  }  
  // Check if temperature is above or equal to 45°C 
  else if (TempC >= 45) { 
    Serial.println("Trigger Alert : Temp>=45°C"); 
    triggerAlert(); // Perform alert ac ons (servo, buzzer, LCD, SMS) 
  }  
  else if (ppm >= 1000) { 
    Serial.println("Trigger Alert : Gas Concentra on>=190 PPM"); 
    triggerAlert(); // Perform alert ac ons (servo, buzzer, LCD, SMS) 
  } 
 
  // Reset the system if alert condi ons are no longer met 
  if (alertAc ve && TempC < 45 && ppm < 190) { 
    resetSystem(); // Reset ac ons (servo, buzzer, LCD) 
  } 
 
  delay(1000); // Delay before the next loop itera on 
} 
 
// Func on to trigger alert ac ons 
void triggerAlert() { 
  myServo.write(180); // Move the servo to 180 degrees 
  Serial.println("Safety Valve: Open"); 
 
  // Siren tone rising (wail) 
  for (int freq = minFrequency; freq <= maxFrequency; freq += frequencyStep) { 
    tone(buzzerPin, freq);  // Generate the tone 
    delay(20); 
  } 
 
  delay(100); // Brief pause at the peak of the siren 
 
  // Siren tone falling (wail) 
  for (int freq = maxFrequency; freq >= minFrequency; freq -= frequencyStep) { 
    tone(buzzerPin, freq);  // Generate the tone 
    delay(20); 
  } 
 
  delay(100); // Brief pause at the low point of the siren 
 
  // Update the LCD with alert messages 
  lcd.setCursor(0, 2); 
  lcd.print("Safety Valve:Open"); 
  lcd.setCursor(0, 3); 
  lcd.print("Alert:Gas/FireDetect"); 
 
  // Send SMS only if it hasn't been sent yet 
  if (!smsSent) { 
    sendSMS();  // Send SMS alert 
    smsSent = true;  // Mark SMS as sent 
  } 
 
  alertAc ve = true;  // Set alert state ac ve 
} 
 
// Func on to reset the system 
void resetSystem() { 
  // Stop the buzzer and reset the servo 
  noTone(buzzerPin);  // Turn off the buzzer 
  myServo.write(0);   // Move the servo back to 0 degrees 
  Serial.println("Safety Valve: Closed"); 
 
  // Clear the LCD alert messages 
  lcd.setCursor(0, 2); 
  lcd.print("Safety Valve:Closed");   
  lcd.setCursor(0, 3); 
  lcd.print("                    "); // Clear the line 
 
  // Reset the SMS sent flag so that a new alert can trigger SMS 
  smsSent = false; 
// Reset alert state 
alertAc ve = false; 
} 

