 Air Pollution Monitoring System - ESP32 + ADS1115 + LCD + IoT
    Sensors: MQ135(A0), MQ6(A1), MQ7(A2), LM35(A3)
    LCD shows readings in rotation (equal timing)
    LEDs + Buzzer indicate alert
    Data uploads to ThingSpeak every ~20 s
*/

#define BLYNK_TEMPLATE_ID "id-BKym0"
#define BLYNK_TEMPLATE_NAME "Air Pollution Monitoring Device"
#define BLYNK_AUTH_TOKEN "-toc--"

#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "ThingSpeak.h"
#include <BlynkSimpleEsp32.h>

// --- WiFi credentials (use only these, not duplicate) ---
char ssid[] = "someone";        // <-- put your actual WiFi name here
char pass[] = "pass";     // <-- put your actual WiFi password here

// ===== USER CONFIG =====
unsigned long myChannelNumber = ch id;   // ThingSpeak channel ID
const char* myWriteAPIKey = "api key";     // ThingSpeak Write API Key
// ========================

Adafruit_ADS1115 ads;
LiquidCrystal_I2C lcd(0x27, 16, 2);     // change address if needed (0x3F)
WiFiClient client;

// Output pins
const int redLED    = 25;
const int greenLED  = 26;
const int buzzerPin = 27;

// ADS1115 channel mapping
const uint8_t CH_MQ135 = 0;
const uint8_t CH_MQ6   = 1;
const uint8_t CH_MQ7   = 2;
const uint8_t CH_LM35  = 3;

// ADC conversion constant for GAIN_ONE (±4.096 V)
const float ADS1115_LSB_V = 0.000125f;

void setup() {
  Serial.begin(115200);
  delay(200);

  lcd.init();
  lcd.backlight();

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);
  digitalWrite(buzzerPin, LOW);

  // ---- ADS1115 ----
  if (!ads.begin()) {
    lcd.clear();
    lcd.print("ADS1115 ERR!");
    Serial.println("Failed to find ADS1115!. Check wiring!");
    while (1) delay(1000);
  }
  ads.setGain(GAIN_ONE); // ±4.096 V for 3.3–5 V sensors

  // ---- Wi-Fi ----
  lcd.clear();
  lcd.print("Connecting WiFi");
  WiFi.begin(ssid, pass);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  lcd.clear();
  if (WiFi.status() == WL_CONNECTED) {
    lcd.print("WiFi Connected");
    Serial.println("\nWiFi connected!");
  } else {
    lcd.print("WiFi Failed");
    Serial.println("\nWiFi connection failed.");
    // continue anyway; Blynk.begin will try too.
  }
  delay(1500);
  lcd.clear();

  // Initialize Blynk (this will also attempt to connect if WiFi not yet connected)
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  ThingSpeak.begin(client);

  // --- Smart Boot Animation ---
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Air pollution");
  lcd.setCursor(0, 1);
  lcd.print("Monitoring device");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing");
  for (int i = 0; i < 3; i++) 
  {
    lcd.print(".");
    delay(1000);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Calibrating");
  lcd.setCursor(0, 1);
  lcd.print("Sensors...");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  lcd.setCursor(0, 1);
  lcd.print("Device Active");
  delay(2000);
  lcd.clear();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Developed by");
  lcd.setCursor(0, 1);
  lcd.print("BIT  ECE Dept");
  delay(3000);
  lcd.clear();
}

// ---- helper: read ADS channel and return voltage ----
float readADSVoltage(uint8_t channel) {
  int16_t raw = ads.readADC_SingleEnded(channel);
  return raw * ADS1115_LSB_V;  // volts
}

void loop() {
  // Read sensors
  float v_mq135 = readADSVoltage(CH_MQ135);
  float v_mq6   = readADSVoltage(CH_MQ6);
  float v_mq7   = readADSVoltage(CH_MQ7);
  float v_lm35  = readADSVoltage(CH_LM35);

  // run Blynk
  Blynk.run();

  

  // LM35: 10 mV / °C
  float temperature = v_lm35 * 100.0f;

  const float SUPPLY_V = 3.3f;  // sensors powered at 3.3 V
  float airQuality = (v_mq135 / SUPPLY_V) * 100.0f;
  float lpgLevel   = (v_mq6   / SUPPLY_V) * 100.0f;
  float coLevel    = (v_mq7   / SUPPLY_V) * 100.0f;

  float aqi = map((int)((v_mq135 / SUPPLY_V) * 4095.0f), 0, 4095, 0, 500);

  // send to Blynk (you might want to send scaled values; here we send voltages)
  
  //Blynk.virtualWrite(V1, v_mq135); for volts
  //Blynk.virtualWrite(V2, v_mq6);
  //Blynk.virtualWrite(V3, v_mq7);
  Blynk.virtualWrite(V0, v_lm35 * 100.0f); // send temperature in °C
  Blynk.virtualWrite(V1, airQuality);  // in %
  Blynk.virtualWrite(V2, lpgLevel);    // in %
  Blynk.virtualWrite(V3, coLevel);     // in %

  // ---- Serial monitor ----
  Serial.println("------ Sensors ------");
  Serial.printf("Temp: %.2f C\n", temperature);
  Serial.printf("MQ135 V: %.3f  AQ%%: %.1f\n", v_mq135, airQuality);
  Serial.printf("MQ6   V: %.3f  LPG%%: %.1f\n", v_mq6, lpgLevel);
  Serial.printf("MQ7   V: %.3f  CO%%: %.1f\n", v_mq7, coLevel);
  Serial.printf("AQI (approx): %.0f\n", aqi);
  Serial.println("---------------------");

  // ---- Alert logic ----
  bool alert = (aqi > 220.0f || lpgLevel > 40.0f || coLevel > 80.0f);
  if (alert) {
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(buzzerPin, HIGH);
  } else {
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
    digitalWrite(buzzerPin, LOW);
  }

  // ---- LCD Display ----
  // Screen 1: Temp + AQI
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.print(temperature, 1);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("AQI:");
  lcd.print((int)aqi);
  delay(5000);   // equal timing

  // Screen 2: Gas levels
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Air:");
  lcd.print((int)airQuality);
  lcd.print("% ");
  lcd.print("LPG:");
  lcd.print((int)lpgLevel);
  lcd.print("%");
  lcd.setCursor(0, 1);
  lcd.print("CO:");
  lcd.print((int)coLevel);
  lcd.print("%");
  delay(5000);   // same duration

  // ---- Alert message (if active) ----
  if (alert) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("!!! ALERT !!!");
    lcd.setCursor(0, 1);
    lcd.print("Pollution Detected");
    delay(3000);
  }

  // ---- ThingSpeak upload (~20 s cycle) ----
  if (WiFi.status() == WL_CONNECTED) {
    ThingSpeak.setField(1, temperature);
    ThingSpeak.setField(2, airQuality);
    ThingSpeak.setField(3, lpgLevel);
    ThingSpeak.setField(4, coLevel);
    int response = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (response == 200) Serial.println("ThingSpeak: OK");
    else Serial.printf("ThingSpeak error: %d\n", response);
  } else {
    Serial.println("WiFi not connected - skipping ThingSpeak.");
  }

  // ensure total loop ~20s (5 + 5 + maybe 3 alert + ThingSpeak + this delay)
  delay(7000);
}
