
# Gas-Leakage-Detection-Real-Time-Alert-System

An industrial-grade safety system designed to detect gas leaks and trigger immediate multi-channel alerts using ESP8266, MQ-5 gas sensor, and Twilio API integration.

## 🚀 Features

- Real-time gas concentration monitoring using MQ-5 sensor
- Temperature monitoring with DS18B20 sensor
- Multi-channel alert system:
  - Visual alerts via 20x4 LCD display
  - Audible alerts through buzzer with variable frequency
  - Automated SMS alerts via Twilio API
  - Emergency valve control using servo motor
- Manual emergency trigger button
- 2-minute sensor warm-up period
- Automatic system reset when conditions normalize

## 🛠️ Hardware Requirements

- ESP8266 microcontroller
- MQ-5 gas sensor
- DS18B20 temperature sensor
- 20x4 LCD display with I2C interface
- Servo motor
- Buzzer
- Push button
- Power supply
- Connecting wires

## 📌 Pin Configuration

```cpp
- MQ-5 Sensor: A0
- Buzzer: D8
- Servo Motor: D6
- DS18B20 Temperature Sensor: D7
- Emergency Button: D5
- LCD: I2C (SDA/SCL)
```

## 🔧 Software Dependencies

```cpp
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
```

## ⚙️ Setup and Configuration

1. Clone this repository
2. Open the `.ino` file in Arduino IDE
3. Install all required libraries
4. Configure the following parameters:
   ```cpp
   // WiFi Settings
   const char* ssid = "Your_WiFi_SSID";
   const char* password = "Your_WiFi_Password";

   // Twilio Configuration
   const char* twilioAccountSid = "Your_Account_SID";
   const char* twilioAuthToken = "Your_Auth_Token";
   const char* twilioPhoneNumber = "Your_Twilio_Number";
   const char* recipientPhoneNumber = "Emergency_Contact_Number";
   ```

## 🚦 Alert Triggers

The system triggers alerts under the following conditions:
- Gas concentration ≥ 190 PPM
- Temperature ≥ 45°C
- Emergency button press
- Combined conditions: Temperature ≥ 45°C AND Gas concentration ≥ 225 PPM

## 🔄 System Response

When an alert is triggered:
1. Servo motor opens the safety valve (180 degrees)
2. Buzzer activates with varying frequency (300Hz - 1000Hz)
3. LCD displays alert messages and current readings
4. SMS alert is sent via Twilio (once per alert)
5. System continues monitoring until conditions normalize

## 📊 Display Information

The LCD screen shows:
- Gas concentration in PPM
- Temperature in both Celsius and Fahrenheit
- Safety valve status
- Alert messages

## 🔙 Reset Conditions

The system automatically resets when:
- Temperature drops below 45°C
- Gas concentration falls below 190 PPM
Reset actions include:
- Closing safety valve (servo to 0 degrees)
- Stopping buzzer
- Clearing alert messages
- Resetting SMS flag

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🤝 Contributing

Contributions, issues, and feature requests are welcome! Feel free to check [issues page](https://github.com/sarveshvengurlekar/Gas-Leakage-Detection-Real-Time-Alert-System/issues).

## ⚠️ Safety Disclaimer

This system is designed for monitoring and alerting purposes. While it can help detect gas leaks and high temperatures, it should not be the only safety measure in place. Always follow proper safety protocols and regulations for handling dangerous gases and high-temperature environments.
