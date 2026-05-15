// ================================================
//   FINAL: Smart Agriculture System - Blynk IoT
//   ESP8266 NodeMCU + Soil Moisture + DHT11 + LDR Module + Relay (ACTIVE LOW)
//   Auto/Manual Pump + Push Notification ("Soil is too dry!")
//   Added: Crop Recommendation based on sensor data (India-specific)
// ================================================

// ── REPLACE THESE WITH YOUR BLYNK CREDENTIALS ──
#define BLYNK_TEMPLATE_ID   "YOUR_BLYNK_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "Smart Agriculture System"
#define BLYNK_AUTH_TOKEN    "YOUR_BLYNK_AUTH_TOKEN"  // Device Auth Token

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// ── REPLACE THESE WITH YOUR WIFI CREDENTIALS ──
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

// ── Pins ──
#define SOIL_PIN     A0         // Soil moisture analog
#define DHT_PIN      D4         // DHT11 data
#define RELAY_PIN    D5         // Relay IN (ACTIVE LOW: LOW = ON, HIGH = OFF)
#define LDR_DO_PIN   D6         // LDR module digital output
#define DHT_TYPE     DHT11

DHT dht(DHT_PIN, DHT_TYPE);

// ── Soil Moisture Calibration ──
const int SOIL_DRY_VALUE   = 1024;   // Fully dry in air
const int SOIL_WET_VALUE   =  440;   // Fully submerged in water
const int PUMP_ON_BELOW    =   35;   // Auto pump + alert below this %

// ── Virtual Pins (match your Blynk datastreams) ──
#define VPIN_MOISTURE  V1
#define VPIN_TEMP      V2
#define VPIN_HUMIDITY  V3
#define VPIN_LIGHT     V4
#define VPIN_PUMP      V5

BlynkTimer timer;

bool manualPump = false;                // Manual override from Blynk V5
static int lastAlertPercent = 100;      // Prevent alert spam

// ── Crop Recommendation Function (India-specific, based on temp, hum, moisture, light) ──
String getCropRecommendation(float temp, float hum, int moisturePercent, int ldrState) {
  String crops = "Recommended crops: ";

  // Temperature-based (common Indian crops)
  if (temp < 18) {  // Cold
    crops += "Wheat, Barley, Mustard";
  } else if (temp < 25) {  // Moderate
    crops += "Maize, Lentil, Chickpea";
  } else {  // Hot
    crops += "Rice, Cotton, Millet";
  }

  // Adjust for humidity
  if (hum < 50) {  // Low humidity - drought tolerant
    crops += ", Sorghum";
  } else if (hum > 80) {  // High humidity
    crops += ", Jute";
  }

  // Adjust for soil moisture
  if (moisturePercent < 35) {  // Low moisture
    crops += ", Millet (drought tolerant)";
  } else if (moisturePercent > 70) {  // High moisture
    crops += ", Sugarcane";
  }

  // Adjust for light (ldrState: assume 1 = low light/dark, 0 = high light/bright)
  if (ldrState == 1) {  // Low light
    crops += ", Coffee (shade tolerant)";
  }

  return crops;
}

// ── Send sensor data & handle logic ──
void sendSensorData() {
  // 1. Soil Moisture
  int soilRaw = analogRead(SOIL_PIN);
  int moisturePercent = map(soilRaw, SOIL_DRY_VALUE, SOIL_WET_VALUE, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  // 2. DHT11
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // 3. LDR Digital Output (0 = bright, 1 = dark or depending on your module)
  int ldrState = digitalRead(LDR_DO_PIN);

  // Debug to Serial Monitor
  Serial.printf("Raw Soil: %d | Moisture: %d%% | Temp: %.1f°C | Hum: %.1f%% | LDR: %d\n",
                soilRaw, moisturePercent, temp, hum, ldrState);

  // Send to Blynk dashboard
  Blynk.virtualWrite(VPIN_MOISTURE,  moisturePercent);
  Blynk.virtualWrite(VPIN_TEMP,       temp);
  Blynk.virtualWrite(VPIN_HUMIDITY,   hum);
  Blynk.virtualWrite(VPIN_LIGHT,      ldrState);

  // 4. Dry Soil Alert (Push Notification with Crop Recommendation)
  if (moisturePercent < PUMP_ON_BELOW && lastAlertPercent >= PUMP_ON_BELOW) {
    String crops = getCropRecommendation(temp, hum, moisturePercent, ldrState);
    String msg = "Soil is too dry! Current: " + String(moisturePercent) + "% → Pump turned ON. " + crops;
    Blynk.logEvent("drysoilalert", msg);  // Event code = drysoilalert (lowercase)
    Serial.println("ALERT SENT: " + msg);
    lastAlertPercent = moisturePercent;
  }

  // Reset alert when moisture recovers
  if (moisturePercent >= PUMP_ON_BELOW) {
    lastAlertPercent = moisturePercent;
  }

  // 5. Auto Pump Control (only if not in manual mode)
  if (!manualPump) {
    if (moisturePercent < PUMP_ON_BELOW) {
      digitalWrite(RELAY_PIN, LOW);   // Relay ON (active LOW)
      Serial.println("AUTO PUMP ON");
    } else {
      digitalWrite(RELAY_PIN, HIGH);  // Relay OFF
      Serial.println("AUTO PUMP OFF");
    }
  }
}

// ── Manual Pump Control from Blynk App (V5) ──
BLYNK_WRITE(VPIN_PUMP) {
  manualPump = param.asInt();
  digitalWrite(RELAY_PIN, manualPump ? LOW : HIGH); // LOW = ON
  String status = manualPump ? "ON" : "OFF";
  Serial.println("Manual PUMP " + status);

  // Optional: Send event when manually turned ON
  if (manualPump) {
    Blynk.logEvent("drysoilalert", "Manual Pump turned ON by user");
  }
}

// ── Setup ──
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n=== Smart Agriculture System - FINAL with Crop Recommendation ===\n");

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // Start with pump OFF (active LOW)

  pinMode(LDR_DO_PIN, INPUT);
  dht.begin();

  // Connect to Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Update sensors & send data every 8 seconds
  timer.setInterval(8000L, sendSensorData);
}

// ── Loop ──
void loop() {
  Blynk.run();
  timer.run();
}