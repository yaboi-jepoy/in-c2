# IN-C2 GUIDE

_by tnznt_

## Core Idea

- create a "task" for each action
- each task must own their data
- use mutexes

| Core 0          | Core 1          |
| --------------- | --------------- |
| Wifi Connection | Sensing Tasks   |
| Database Upload | Data Formatting |
|                 | Screen Update   |

## Data Structure

| Data Type | Variable                                           |
| --------- | -------------------------------------------------- |
| float     | <ul><li> pH <li> DO <li> temperature <li> pressure |
| double    | <ul><li> longitude <li> latitude                   |
| boolean   | isWifiConnected                                    |

## Tasks

| Category  | Description                            |
| --------- | -------------------------------------- |
| Sensing   | pH, DO, temp, BME280                   |
| GPS       | handle serial stream of GPS            |
| Display   | update screen                          |
| Telemetry | handle WiFi and cloud database uploads |

## Essential GPIO Pinout

| PIN    | Component |
| ------ | --------- |
| GPIO4  | DS18B20   |
| ADS1115 A0 | pH sensor analog output |
| ADS1115 A1 | DO sensor analog output |
| GPIO16 | GPS module TX -> ESP32 RX |
| GPIO17 | GPS module RX <- ESP32 TX |
| GPIO5  | TFT CS |
| GPIO27 | TFT DC |
| GPIO26 | TFT RST |
| GPIO13 | ULN2003 IN1 |
| GPIO14 | ULN2003 IN2 |
| GPIO25 | ULN2003 IN3 |
| GPIO33 | ULN2003 IN4 |

## Hardware Notes

The GPS module uses UART2 at 9600 baud. Its TX wire connects to ESP32 GPIO16
(RX), and its RX wire connects to GPIO17 (TX).
