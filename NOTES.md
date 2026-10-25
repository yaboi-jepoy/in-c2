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
| GPIO17 | GPS module TX -> ESP32 RX |
| GPIO16 | GPS module RX <- ESP32 TX |

## Hardware Notes

The GPS module uses UART1 at 9600 baud. Its TX wire connects to ESP32 GPIO17
(RX), and its RX wire connects to GPIO16 (TX).
