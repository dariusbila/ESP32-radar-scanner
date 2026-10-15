#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <ESP32Servo.h>

// Pin definitions
#define SERVO_PIN  13
#define TRIG_PIN   33
#define ECHO_PIN   32
#define BUZZER_PIN 12

// TFT LCD display pins
#define TFT_CS    15
#define TFT_RST   4
#define TFT_DC    2
#define TFT_MOSI  23
#define TFT_SCLK  19

// ── Objects 
Servo myServo;
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Scanning data
int distances[181];
int currentAngle = 0;
int sweepDirection = 1;

// Measuring the distance (cm)
long measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duration == 0) return 400;
  long d = duration * 0.034 / 2;
  return d > 400 ? 400 : d; // clamp fix
}

void drawTitle() {
  tft.fillRect(0, 0, 128, 14, ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(1);
  tft.setCursor(16, 3);
  tft.print("RADAR SCANNER");
  tft.drawFastHLine(0, 13, 128, 0x07E0); // underline
}

// radar drawing on TFT
void drawRadar(int angle, int distance) {
  tft.fillRect(0, 15, 128, 115, ST77XX_BLACK);

  int cx = 64;
  int cy = 120;
  int radius = 56;

  // Rings
  tft.drawCircle(cx, cy, 14, ST77XX_GREEN);
  tft.drawCircle(cx, cy, 28, ST77XX_GREEN);
  tft.drawCircle(cx, cy, 42, ST77XX_GREEN);
  tft.drawCircle(cx, cy, 56, ST77XX_GREEN);

  // Angle lines
  for (int a = 0; a <= 180; a += 30) {
    float rad = radians(a);
    int x = cx + cos(rad) * radius;
    int y = cy - sin(rad) * radius;
    tft.drawLine(cx, cy, x, y, ST77XX_GREEN);
  }

  // Sweep line
  float rad = radians(angle);
  int sx = cx + cos(rad) * radius;
  int sy = cy - sin(rad) * radius;
  tft.drawLine(cx, cy, sx, sy, ST77XX_GREEN);

  // Detection dot
  if (distance < 400) {
    int r = map(distance, 0, 400, 0, radius);
    int px = cx + cos(rad) * r;
    int py = cy - sin(rad) * r;
    tft.fillCircle(px, py, 3, ST77XX_RED);
  }

  // Bottom text
  tft.fillRect(0, 130, 128, 30, ST77XX_BLACK);
  tft.setCursor(0, 135);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.print("Angle: ");
  tft.print(angle);
  tft.setCursor(0, 148);
  tft.print("Dist: ");
  if (distance < 400) {
    tft.print(distance);
    tft.print(" cm");
  } else {
    tft.print("---");
  }

}
//buzzer
  void handleBuzzer(int distance) {
  if (distance >= 25) {
    digitalWrite(BUZZER_PIN, LOW);
    return;}
    else
    if (distance<50) digitalWrite(BUZZER_PIN,HIGH);
    return;
  }


// ── Setup ──
void setup() {
  Serial.begin(115200);

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  tft.initR(INITR_GREENTAB);
  tft.setAddrWindow(0, 0, 127, 159);
  tft.setRotation(2);
  tft.fillScreen(ST77XX_BLACK);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN,OUTPUT);

  myServo.attach(SERVO_PIN, 500, 2400);
  myServo.write(0);
  delay(500);

  for (int i = 0; i <= 180; i++) distances[i] = 400;

  drawTitle(); // draw once, never touched again
}


// ── Main loop ──
void loop() {
  myServo.write(currentAngle);
  delay(30);

  distances[currentAngle] = measureDistance();
   handleBuzzer(distances[currentAngle]);
  drawRadar(currentAngle, distances[currentAngle]);

  Serial.print("Angle: ");
  Serial.print(currentAngle);
  Serial.print("° | Distance: ");
  Serial.print(distances[currentAngle]);
  Serial.println(" cm");

  currentAngle += sweepDirection;
  if (currentAngle >= 180) { currentAngle = 180; sweepDirection = -1; }
  else if (currentAngle <= 0) { currentAngle = 0; sweepDirection = 1; }
}