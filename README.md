# Obstacle Detection Warning System  
Arduino + ESP8266 + ThingSpeak + IFTTT

---

## 1. Project Description

This project is an IoT-based Obstacle Detection and Warning System built using Arduino UNO, HC-SR04 ultrasonic sensor, ESP8266 WiFi module, 16x2 LCD display, and a buzzer indicator.

The system continuously measures distance using ultrasonic sensing.  
When an obstacle is detected within 30 cm:

- Buzzer and LED alert are activated  
- An IFTTT webhook notification is triggered  
- Distance data is uploaded to ThingSpeak cloud  
- Distance is displayed on a 16x2 LCD  

The system combines embedded hardware interfacing with cloud-based IoT communication.

---

## 2. System Architecture

Hardware Layer:
- Ultrasonic sensor measures distance
- Arduino processes sensor data
- LCD displays live readings
- Buzzer provides local alert

Communication Layer:
- ESP8266 (AT command mode) provides WiFi connectivity
- HTTP GET requests used for cloud communication

Cloud Layer:
- ThingSpeak stores and visualizes distance data
- IFTTT sends real-time mobile notifications

---

## 3. Hardware Components Used

- Arduino UNO
- ESP8266 WiFi module (AT firmware)
- HC-SR04 Ultrasonic Sensor
- 16x2 LCD Display (Parallel mode)
- Buzzer
- LED
- Resistors
- Breadboard
- Jumper wires
- External power supply

---

## 4. Pin Configuration

### Ultrasonic Sensor
- Trig → A5  
- Echo → A4  

### ESP8266 (SoftwareSerial)
- RX → Pin 8  
- TX → Pin 9  

### LCD (16x2 in 4-bit mode)
```cpp
LiquidCrystal lcd(12,11,5,4,3,2);
```

### Buzzer + LED
- Pin 7  

---

## 5. Working Principle

1. Arduino sends a 10 microsecond pulse to the ultrasonic sensor trigger pin.
2. Echo pulse duration is measured using `pulseIn()`.
3. Distance is calculated using:

```
Distance (cm) = (duration / 2) / 29.1
```

Explanation:
- Division by 2 accounts for round-trip travel of sound.
- 29.1 converts microseconds to centimeters based on speed of sound.

4. If distance < 30 cm:
   - Buzzer is activated
   - IFTTT webhook is triggered
5. Distance is printed on LCD.
6. Data is sent to ThingSpeak every 16 seconds.

---

## 6. WiFi and Cloud Communication

### ESP8266 Setup

The ESP8266 module is controlled using AT commands via SoftwareSerial.

Key commands used:
- `AT` – Check communication
- `AT+CWJAP` – Connect to WiFi
- `AT+CIPSTART` – Start TCP connection
- `AT+CIPSEND` – Send HTTP request
- `AT+CIPCLOSE` – Close connection

---

### ThingSpeak Integration

Server:
```
api.thingspeak.com
```

HTTP Request Format:
```
GET /update?api_key=YOUR_API_KEY&field1=distance HTTP/1.1
Host: api.thingspeak.com
```

Data is uploaded every 16 seconds to respect the free API limit.

---

### IFTTT Webhook Integration

When obstacle is detected, a webhook is triggered:

```
GET /trigger/obstacle_detected/with/key/YOUR_IFTTT_KEY HTTP/1.1
Host: maker.ifttt.com
```

This can generate:
- Mobile notification
- Email alert
- SMS alert

---

## 7. LCD Output

The LCD continuously displays:

```
Distance XX
cm
```

This provides local real-time monitoring without cloud access.

---

## 8. Simulation Environment

The project was designed and tested in:

Tinkercad Circuits Simulator

Simulation verifies:
- Sensor readings
- LCD display logic
- Buzzer activation
- Serial debugging output

---

## 9. API Rate Limiting

ThingSpeak free tier allows 1 update every 15 seconds.

The code uses:

```cpp
delay(16000);
```

This ensures compliance with cloud rate limits.

---

## 10. Key Learning Outcomes

- Ultrasonic sensor interfacing
- Embedded C programming
- Serial communication
- AT command handling
- HTTP protocol basics
- IoT cloud integration
- Real-time alert systems
- System-level embedded design

---

## 11. Future Improvements

- Replace ESP8266 AT mode with NodeMCU/ESP32
- Use MQTT protocol instead of HTTP
- Add battery backup system
- Add multiple ultrasonic sensors
- Develop Android dashboard app
- Implement deep sleep for power optimization

---

## 12. Author

Raghav Sathe  
Electronics and Telecommunication Engineering  
Focus: IoT, Embedded Systems, Robotics, Semiconductor Systems

---

If you find this project useful, consider starring the repository.
