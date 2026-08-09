# gest-hs

**Gesture Encoding for Sign Translation in Healthcare Sector**

GEST-HS is an AI-assisted smart glove designed to help bedridden or physically constrained patients communicate basic healthcare needs through hand gestures.

The system uses an **ESP32** and **flex sensors** to detect finger movements and classify predefined gestures using **Edge Impulse**. The detected gesture can be mapped to a specific healthcare alert or patient requirement.

## Features

- ESP32-based gesture sensing
- Flex-sensor-based finger movement detection
- AI-based gesture classification using Edge Impulse
- Real-time gesture recognition
- Healthcare-oriented gesture-to-action mapping
- Dataset collection for gesture classification

## Hardware

- ESP32
- 3 Flex Sensors
  - Thumb
  - Index
  - Middle
- 10 kΩ Resistors
- 16×2 I2C LCD
- Glove

## Gesture Mapping

| Gesture | Healthcare Action |
|---|---|
| Fist | Emergency |
| Thumbs Up | Glucose Low |
| Peace | Need Assistance |
| Pointing | Pain / Discomfort |
| Open Hand | Stable |

## Repository Structure

```text
GEST-HS/
│
├── GEST_HS_ESP32.ino
├── dataset_gest_hs.csv
└── README.md
