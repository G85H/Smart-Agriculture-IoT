# Smart Agriculture System - Blynk IoT

## About the Project
This is an IoT-based Smart Agriculture System built using an ESP8266 NodeMCU. The system monitors crucial agricultural parameters—soil moisture, temperature, humidity, and light levels—in real-time. It features an automated watering system that turns on a water pump when the soil gets too dry, sending push notifications to the user via the Blynk app. Additionally, it features a unique algorithm that recommends suitable crops (specifically tailored for Indian climates) based on the real-time sensor data.

## Features
* **Real-time Monitoring:** Tracks Temperature, Humidity, Soil Moisture, and Light levels.
* **Smart Automation:** Automatically triggers a water pump (via relay) when soil moisture drops below 35%.
* **Remote Manual Override:** Allows the user to manually control the water pump from anywhere in the world using the Blynk app.
* **Intelligent Crop Recommendation:** Suggests suitable crops based on the current environmental data (temperature, humidity, moisture, and light conditions).
* **Push Notifications:** Alerts the user immediately when the soil is too dry, including the crop recommendation in the alert.

## Components Used
* **Microcontroller:** ESP8266 NodeMCU
* **Sensors:** 
  * Soil Moisture Sensor (Analog)
  * DHT11 Temperature & Humidity Sensor
  * LDR (Light Dependent Resistor) Module (Digital Output)
* **Actuators:** 5V Relay Module (Active Low) & Mini Water Pump
* **Platform:** Blynk IoT Platform

## Pin Mapping / Circuit Connections
| Component | NodeMCU ESP8266 Pin | Notes |
| :--- | :--- | :--- |
| Soil Moisture Sensor | `A0` (Analog In) | Needs calibration for wet/dry values |
| DHT11 Sensor | `D4` (GPIO 2) | |
| Relay Module (Pump) | `D5` (GPIO 14) | Active LOW (LOW = ON, HIGH = OFF) |
| LDR Module | `D6` (GPIO 12) | Digital Output used |

## Setup & Installation
To run this project, you need to add your own WiFi credentials and Blynk IoT tokens.

1. Create a [Blynk IoT](https://blynk.io/) account and set up a new Template and Device.
2. Set up the following Virtual Pins (Datastreams) in Blynk:
   * `V1`: Soil Moisture (%)
   * `V2`: Temperature (°C)
   * `V3`: Humidity (%)
   * `V4`: Light Status
   * `V5`: Pump Control (Switch)
3. Open the `.ino` code file in the Arduino IDE.
4. Replace the placeholders at the top of the code with your actual credentials:
   ```cpp
   #define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
   #define BLYNK_AUTH_TOKEN    "YOUR_AUTH_TOKEN"
   char ssid[] = "YOUR_WIFI_SSID";
   char pass[] = "YOUR_WIFI_PASSWORD";