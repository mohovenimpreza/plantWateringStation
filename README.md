# Plant watering station (Arduino)

An Arduino station that measures **light, temperature, air humidity and soil moisture**, shows the readings on a Nokia 5110-type LCD, and switches a **small pump motor** on when it is bright, warm and the soil is dry. University project.

## Hardware
| Part | Pin |
|---|---|
| Nokia 5110-type LCD (84×48, PCD8544), software SPI | SCE D7, RESET D6, DC D5, SDIN D4, SCLK D3 |
| Light sensor | A0 |
| Temperature sensor | A1 |
| Air-humidity sensor (56 kΩ divider) | A2 |
| Soil-moisture sensor | A4 |
| Small pump motor (through a driver) | D9 |
| 1 V solar cell + battery | supplied ready to use (see below) |

## Solar cell (honest note)
The kit included a **1 V photovoltaic cell** meant to charge the battery. It was supplied ready to apply (I did not design that part) and it is far too weak to be practical: a full charge would take an unrealistic amount of time. So the station is **not truly off-grid**: the solar cell is part of the design, not something that makes it autonomous.

## What the code does
Sketch: [`plant_station/plant_station.ino`](plant_station/plant_station.ino). Every second:
1. Reads the four sensors with `analogRead`.
2. Shows the light level (*Tres Sombre … Tres Lumineux*), the temperature in °C, the air humidity in % and the soil state (*so sec … soooo humide*).
3. **Air humidity:** converts the sensor voltage to a percentage with a temperature-indexed lookup table (resistance values from the sensor's datasheet table, 0–50 °C × 20–90 %).
4. **Watering rule:** motor ON for 2 s when `light ≥ 250`, `soil ≤ 80` and `temperature ≥ 25 °C`; the thresholds are constants at the top of the sketch.

## Credits
The low-level LCD code (pin definitions, command bytes and the 5×7 font table) follows the widely shared Nokia 5110 Arduino example and is not original work.
