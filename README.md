# Air Pollution Monitoring System

## Overview
This project is an IoT based Air Pollution Monitoring System designed to monitor environmental pollution levels in real time. The system uses gas sensors and temperature sensors connected to an ESP32 microcontroller. Data is displayed on an LCD screen and transmitted to cloud platforms for remote monitoring.

## Features
- Real-time air quality monitoring
- Temperature measurement
- LCD display output
- IoT monitoring using Blynk
- Data logging using ThingSpeak
- Alert system using buzzer and LEDs

## Hardware Components
- ESP32 Development Board
- ADS1115 ADC Module
- MQ135 Air Quality Sensor
- MQ6 LPG Gas Sensor
- MQ7 Carbon Monoxide Sensor
- LM35 Temperature Sensor
- 16x2 LCD Display with I2C
- Buzzer
- LEDs
- Power Supply

## Software Used
- Arduino IDE
- Blynk IoT Platform
- ThingSpeak Cloud

## Working Principle
Gas sensors detect different gases present in the environment. The analog signals from the sensors are converted into digital values using the ADS1115 ADC module. The ESP32 processes these signals and calculates the pollution level. The data is displayed on the LCD screen and sent to IoT platforms like Blynk and ThingSpeak for real-time monitoring.

## Applications
- Smart Cities
- Industrial Pollution Monitoring
- Indoor Air Quality Monitoring
- Environmental Research
