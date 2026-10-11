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

## Hardware Notes
