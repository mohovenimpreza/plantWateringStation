/*
  Plant watering station (Arduino)
  Reads light, temperature, air humidity and soil moisture, shows them on a
  Nokia 5110-type LCD (84x48, PCD8544) and switches a motor (watering) on
  when it is bright, warm and the soil is dry.

  Wiring: LCD SCE=D7, RESET=D6, DC=D5, SDIN=D4, SCLK=D3
          light sensor A0, temperature sensor A1, air-humidity sensor A2,
          soil-moisture sensor A4, motor (via driver) D9
*/

// ---------- LCD (PCD8544, software SPI) ----------
// The low-level LCD code (pins, command bytes, 5x7 font table) follows the widely
// shared Nokia 5110 Arduino example; see "Credits" in the README.
#define PIN_SCE   7
#define PIN_RESET 6
#define PIN_DC    5
#define PIN_SDIN  4
#define PIN_SCLK  3

#define LCD_C     LOW
#define LCD_D     HIGH

#define LCD_X     84
#define LCD_Y     48

static const byte ASCII[][5] =
{
  {0x00, 0x00, 0x00, 0x00, 0x00} // 20  
,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c backslash
,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j 
,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e
,{0x78, 0x46, 0x41, 0x46, 0x78} // 7f
};

void lcd_goto(byte x, byte y);

void LcdWrite(byte dc, byte data)
{
  digitalWrite(PIN_DC, dc);
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  digitalWrite(PIN_SCE, HIGH);
}

void LcdCharacter(char character)
{
  LcdWrite(LCD_D, 0x00);
  for (int index = 0; index < 5; index++)
  {
    LcdWrite(LCD_D, ASCII[character - 0x20][index]);
  }
  LcdWrite(LCD_D, 0x00);
}

void LcdString(const char *characters)
{
  while (*characters)
  {
    LcdCharacter(*characters++);
  }
}

void LcdClear(void)
{
  for (int index = 0; index < LCD_X * LCD_Y / 8; index++)
  {
    LcdWrite(LCD_D, 0x00);
  }
  lcd_goto(0, 0);
}

void lcd_goto(byte x, byte y)
{
  LcdWrite(LCD_C, 0x80 | x); // Column
  LcdWrite(LCD_C, 0x40 | y); // Row
}

void LcdInitialise(void)
{
  pinMode(PIN_SCE, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DC, OUTPUT);
  pinMode(PIN_SDIN, OUTPUT);
  pinMode(PIN_SCLK, OUTPUT);
  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_RESET, HIGH);
  LcdWrite(LCD_C, 0x21); // Extended commands
  LcdWrite(LCD_C, 0xBF); // Contrast
  LcdWrite(LCD_C, 0x04); // Temp coefficient
  LcdWrite(LCD_C, 0x14); // Bias mode
  LcdWrite(LCD_C, 0x20); // Basic commands
  LcdWrite(LCD_C, 0x0C); // Normal mode
}

// ---------- Sensors and watering logic ----------
const int PIN_LIGHT     = A0;   // light sensor
const int PIN_TEMP      = A1;   // temperature sensor
const int PIN_HUMID     = A2;   // air-humidity sensor
const int PIN_SOIL      = A4;   // soil-moisture sensor
const int PIN_MOTOR     = 9;    // watering motor (through a driver)

// Watering rule: motor ON when it is bright, the soil is dry and it is warm.
const int   LIGHT_MIN_FOR_WATERING = 250;
const int   SOIL_MAX_FOR_WATERING  = 80;
const float TEMP_MIN_FOR_WATERING  = 25.0;
const unsigned long WATERING_MS    = 2000;

// Air-humidity sensor: resistance (kOhm) per temperature row (0..50 C, step 5)
// and per humidity column (20..90 %, step 5), from the sensor's datasheet table.
const int RH_ROWS = 11, RH_COLS = 15;
float rh[RH_ROWS][RH_COLS] =
{
  {0,0,12000,5200,2800,720,382,200,108,64,38,23,16,10.2,6.9},
  {0,19800,9800,4700,2000,510,271,149,82,48,29,18,12,8.2,5.4},
  {0,16000,7200,3200,1400,386,211,118,64,38,24,15,10.2,6.9,4.7},
  {21000,10500,5100,2350,1050,287,159,91,51,31,19,12,8.1,5.5,4.1},
  {13500,6700,3300,1800,840,216,123,70,40,25,16,10,7.2,4.7,3.2},
  {9800,4803,2500,1300,630,166,95,55,31,20,13,8.5,5.7,4,2.8},
  {8000,3900,2000,980,470,131,77,44,25,17,10.5,7.2,5,3.6,2.5},
  {6300,3100,1500,750,385,104,63,38,21,13,9,6.4,4.4,3.2,2.3},
  {4600,2300,1100,575,282,80,52,32,17,11,8.2,5.8,4,2.9,2.1},
  {3800,1850,900,430,210,66,45,30,14,9,7.1,5,3.3,2.4,1.8},
  {3200,1550,750,350,170,51,38,24,12,8,6,4.1,2.9,2,1.5}
};

float v0[RH_ROWS][RH_COLS];     // expected divider voltage for each table entry (computed once)

void buildVoltageTable() {
  for (int i = 0; i < RH_ROWS; i++)
    for (int j = 0; j < RH_COLS; j++)
      v0[i][j] = 5 * (56000.0 / (56000.0 + rh[i][j] * 1000));   // 56 kOhm divider resistor
  // entries missing from the datasheet table
  v0[0][0] = 0; v0[0][1] = 0; v0[1][0] = 0; v0[2][0] = 0;
}

void showLight(int val) {
  LcdClear();
  if (val < 5)       LcdString("Tres Sombre");
  else if (val < 30) LcdString("Sombre");
  else if (val < 65) LcdString("Lumineux");
  else               LcdString("Tres Lumineux");
}

// Converts the temperature sensor reading to Celsius, shows it, returns it.
float showTemperature(int raw) {
  float T = raw * 100.0 / 1023 - 9;          // -9: calibration offset found by comparison
  char buf[8];
  itoa((int)T, buf, 10);
  LcdString(buf);
  LcdString(" C");
  return T;
}

// Converts the air-humidity reading to a percentage using the table, shows it, returns it.
int showAirHumidity(int raw, float tempC) {
  float H = raw * 5.0 / 1023;                // measured voltage
  Serial.println(H);

  int row = 0;                               // nearest table row at or above the temperature
  int tmp = 0;
  while (tempC > tmp) tmp += 5;
  row = tmp / 5;
  if (row > RH_ROWS - 1) row = RH_ROWS - 1;  // above 50 C: use the last row

  int col = 0;
  while (H > v0[row][col] && col < RH_COLS - 1) col++;

  int humidity = col * 5 + 20;
  char buf[8];
  itoa(humidity, buf, 10);
  LcdString(buf);
  LcdString("%Humidite");
  return humidity;
}

void showSoil(int raw) {
  if (raw < 50)       LcdString("so sec");
  else if (raw < 200) LcdString("sec");
  else if (raw < 500) LcdString("humide");
  else if (raw < 800) LcdString("so humide");
  else                LcdString("soooo humide");
}

void waterIfNeeded(int light, int soil, float tempC) {
  if (light >= LIGHT_MIN_FOR_WATERING && soil <= SOIL_MAX_FOR_WATERING && tempC >= TEMP_MIN_FOR_WATERING) {
    digitalWrite(PIN_MOTOR, HIGH);
    delay(WATERING_MS);
  }
  digitalWrite(PIN_MOTOR, LOW);
}

void setup(void)
{
  Serial.begin(9600);
  LcdInitialise();
  LcdClear();
  LcdString("Plant station");
  delay(2000);
  LcdClear();
  pinMode(PIN_MOTOR, OUTPUT);
  digitalWrite(PIN_MOTOR, LOW);
  buildVoltageTable();
}

void loop()
{
  int light = analogRead(PIN_LIGHT);
  int tempRaw = analogRead(PIN_TEMP);
  int humidRaw = analogRead(PIN_HUMID);
  int soil = analogRead(PIN_SOIL);

  showLight(light);              // clears the screen, line 0
  lcd_goto(0, 1);
  float tempC = showTemperature(tempRaw);
  lcd_goto(0, 2);
  showAirHumidity(humidRaw, tempC);
  lcd_goto(0, 3);
  showSoil(soil);

  waterIfNeeded(light, soil, tempC);
  delay(1000);
}
