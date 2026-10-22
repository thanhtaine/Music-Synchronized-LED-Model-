#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define LED_DAY_PIN    27   
#define LED_VONG_PIN   26   
#define BUTTON_PIN     4    
#define NUM_LEDS_DAY   20   
#define NUM_LEDS_VONG  16   

Adafruit_NeoPixel stripDay(NUM_LEDS_DAY, LED_DAY_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripVong(NUM_LEDS_VONG, LED_VONG_PIN, NEO_GRB + NEO_KHZ800);

unsigned long lastUpdate = 0;
unsigned long stateStartTime = 0;
unsigned long lastColorChange = 0;
unsigned long lastStrobe = 0;
unsigned long debouncetime = 0;
bool strobeState = false;
bool lastButtonState = HIGH;
bool ButtonState = HIGH;
int colorToggle = 0;
int currentColorIdx = 0;
int breathLevel = 0;
int breathDir = 5; 
const unsigned long debouncedelay = 50;
uint16_t currentHue = 0;

enum EffectState { 
  WAIT_MODE, INTRO_MODE, LYRICS_FAST, LYRICS_FAST2, 
  LYRICS_FAST3, LYRICS_FAST4, LYRICS_FINAL,
  FINAL_EFFECT_1, FINAL_EFFECT_2, RADIANT_END,
  B2_PHASE_1, B2_PHASE_2, B2_PHASE_3, B2_PHASE_4,
  B2_PHASE_5, B2_PHASE_6, B2_PHASE_7 
};
EffectState state = WAIT_MODE;

uint32_t introColors[] = {
  stripDay.Color(0, 255, 0),   
  stripDay.Color(150, 0, 255), 
  stripDay.Color(255, 100, 0), 
  stripDay.Color(0, 255, 255)  
};

void effectAlternating();
void effectRGBSplit(int speed);
void effectSimultaneousBreath();
void effectLyrics(int colorSpeed, int updateSpeed, int flowStep);
void effectStrobeAndChange(int colorInterval, int strobeInterval);
void effectFastWave(int speed);
void effectFastChase(int speed);
void effectRadiantRainbow(int speed);
void effectBeatStrobe(uint32_t color, int speed);
void effectFastChaseDouble(int speed);
void effectSparkle(uint32_t baseColor, int speed);
void effectFire(int speed);
void effectMegaBounce(int speed);

struct Step 
{
  void (*effect)(void);
  unsigned long duration;
  EffectState nextState;
};
void runlyrics1(){effectLyrics(800, 800, 6);}
void runLyrics2() { effectLyrics(400, 400, 6); }
void runLyrics3() { effectLyrics(200, 200, 8); }
void runLyrics4() { effectLyrics(50, 50, 10); }
void runStrobeChange() { effectStrobeAndChange(1000, 150); }
void runFastWave() { effectFastWave(40); }
void runFastChase() { effectFastChase(30); }
void runRadiantEnd() { effectRadiantRainbow(5); }
void runBeatStrobe() { effectBeatStrobe(stripDay.Color(255, 0, 0), 100); }
void runFastChaseDouble() { effectFastChaseDouble(40); }
void runSparkle() { effectSparkle(stripDay.Color(0, 0, 255), 30); }
void runFire() { effectFire(60); }
void runRGBSplit() { effectRGBSplit(80); }
void runMegaBounce() { effectMegaBounce(30); }
void runRadiantEnd2() { effectRadiantRainbow(2); }

Step songscript[] = {
  {effectSimultaneousBreath, 5300, LYRICS_FAST},
  {runlyrics1, 15000 , LYRICS_FAST2},
  { runLyrics2,               7000,  LYRICS_FAST3 },
  { runLyrics3,               4000,  LYRICS_FAST4 },
  { runLyrics4,               6000,  LYRICS_FINAL },
  { runStrobeChange,          6500,  FINAL_EFFECT_1 },
  { runFastWave,              6500,  FINAL_EFFECT_2 },
  { runFastChase,             14500, RADIANT_END },
  { runRadiantEnd,            22000, B2_PHASE_1 },
  { runBeatStrobe,            11000, B2_PHASE_2 },
  { runFastChaseDouble,       5500,  B2_PHASE_3 },
  { runSparkle,               6000,  B2_PHASE_4 },
  { runFire,                  11500, B2_PHASE_5 },
  { runRGBSplit,              12000, B2_PHASE_6 },
  { runMegaBounce,            11000, B2_PHASE_7 },
  { runRadiantEnd2,           30000, WAIT_MODE }
};

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  stripDay.begin();
  stripDay.setBrightness(150); 
  stripDay.show();

  stripVong.begin();
  stripVong.setBrightness(100); 
  stripVong.show();
}



void loop(){
  bool reading = digitalRead(BUTTON_PIN);
  if(reading != lastButtonState){
    debouncetime = millis();
  }
  if(millis() - debouncetime > debouncedelay){
    if(reading != ButtonState){
      ButtonState = reading;
    if( ButtonState == LOW ){
    state = INTRO_MODE;
    stateStartTime = millis();
    lastUpdate = millis();
    lastColorChange = millis();
    breathLevel = 0;
    breathDir = 5;
    currentColorIdx = 0;
    currentHue = 0;
    stripDay.clear();
    stripVong.clear();
    }
  }
}
  lastButtonState = reading;

  switch (state)
  {
  case WAIT_MODE:
     effectAlternating();
    break;
  
  default:{
    int stepIdx = state - INTRO_MODE; 
    
    // 1. Chạy hiệu ứng tương ứng trong mảng
    songscript[stepIdx].effect(); 
    
    if (millis() - stateStartTime >= songscript[stepIdx].duration) {
      state = songscript[stepIdx].nextState;
      stateStartTime = millis();
  }
    break;
    }
  
}
}

void effectAlternating() {
  if (millis() - lastUpdate > 1000) { 
    lastUpdate = millis();
    for (int i = 0; i < NUM_LEDS_DAY; i++) {
      stripDay.setPixelColor(i, ((i + colorToggle) % 2 == 0) ? stripDay.Color(150, 150, 0) : stripDay.Color(0, 0, 150));
    }
    for (int i = 0; i < NUM_LEDS_VONG; i++) {
      stripVong.setPixelColor(i, ((i + colorToggle) % 2 == 0) ? stripVong.Color(150, 150, 0) : stripVong.Color(0, 0, 150));
    }
    stripDay.show(); stripVong.show();
    colorToggle = !colorToggle;
  }
}

void effectRGBSplit(int speed) {
  if (millis() - lastUpdate > speed) {
    lastUpdate = millis();
    strobeState = !strobeState;
    
    // Màu sắc
    uint32_t c1 = stripDay.Color(255, 0, 255); // Tím
    uint32_t c2 = stripDay.Color(0, 255, 0);   // Xanh lá

    // LED Dây: Chia cụm 5 bóng
    for (int i = 0; i < NUM_LEDS_DAY; i++) {
      stripDay.setPixelColor(i, ((i / 5) % 2 == 0) ? (strobeState ? c1 : c2) : (strobeState ? c2 : c1));
    }
    
   
    for (int i = 0; i < NUM_LEDS_VONG; i++) {
      stripVong.setPixelColor(i, ((i / 4) % 2 == 0) ? (strobeState ? c1 : c2) : (strobeState ? c2 : c1));
    }
    
    stripDay.show(); 
    stripVong.show();
  }
}


void effectSimultaneousBreath() {
  if (millis() - lastUpdate > 20) {
    lastUpdate = millis();
    uint32_t baseColor = introColors[currentColorIdx];
    byte r = (baseColor >> 16) & 0xFF; byte g = (baseColor >> 8) & 0xFF; byte b = baseColor & 0xFF;
    uint32_t newColor = stripDay.Color((r * breathLevel) / 255, (g * breathLevel) / 255, (b * breathLevel) / 255);
    for (int i = 0; i < NUM_LEDS_DAY; i++) stripDay.setPixelColor(i, newColor);
    for (int i = 0; i < NUM_LEDS_VONG; i++) stripVong.setPixelColor(i, newColor);
    stripDay.show(); stripVong.show();
    breathLevel += breathDir;
    if (breathLevel >= 255 || breathLevel <= 0) {
      breathDir *= -1;
      if (breathLevel <= 0) { breathLevel = 0; currentColorIdx = (currentColorIdx + 1) % 4; }
    }
  }
}



void effectLyrics(int colorSpeed, int updateSpeed, int flowStep) {
  static uint16_t flowOffset = 0;
  if (millis() - lastColorChange > colorSpeed) { lastColorChange = millis(); currentHue += 6000; }
  if (millis() - lastUpdate > updateSpeed) {
    lastUpdate = millis();
    for (int i = 0; i < NUM_LEDS_DAY; i++) {
      int br = 40 + (int)((sin(i * 0.8 + flowOffset * 0.1) + 1.0) * 107.5);
      stripDay.setPixelColor(i, stripDay.ColorHSV(currentHue, 255, br));
    }
    for (int i = 0; i < NUM_LEDS_VONG; i++) {
      int br = 30 + ( (sin(i * 0.6 + flowOffset * 0.1) + 1.0) * 112.5 );
      stripVong.setPixelColor(i, stripVong.ColorHSV(currentHue, 255, br));
    }
    stripDay.show(); stripVong.show();
    flowOffset += flowStep;
  }
}


void effectStrobeAndChange(int colorInterval, int strobeInterval) {
  if (millis() - lastColorChange > colorInterval) { lastColorChange = millis(); currentHue += 10000; }
  if (millis() - lastStrobe > strobeInterval) {
    lastStrobe = millis();
    strobeState = !strobeState;
    if (strobeState) {
      uint32_t color = stripDay.ColorHSV(currentHue, 255, 255);
      for (int i = 0; i < NUM_LEDS_DAY; i++) stripDay.setPixelColor(i, color);
      for (int i = 0; i < NUM_LEDS_VONG; i++) stripVong.setPixelColor(i, color);
    } else {
      stripDay.clear(); stripVong.clear();
    }
    stripDay.show(); stripVong.show();
  }
}


void effectFastWave(int speed) {
  static uint16_t offset = 0;
  if (millis() - lastUpdate > speed) {
    lastUpdate = millis();
    currentHue += 3000;
    for (int i = 0; i < NUM_LEDS_DAY; i++) {
      int v = (sin(i * 0.8 + offset * 0.4) + 1.0) * 127;
      stripDay.setPixelColor(i, stripDay.ColorHSV(currentHue, 255, v));
    }
    for (int i = 0; i < NUM_LEDS_VONG; i++) {
      int v = (sin(i * 0.8 + offset * 0.4) + 1.0) * 127;
      stripVong.setPixelColor(i, stripVong.ColorHSV(currentHue, 255, v));
    }
    stripDay.show(); stripVong.show();
    offset++;
  }
}


void effectFastChase(int speed) {
  static int pos = 0;
  if (millis() - lastUpdate > speed) {
    lastUpdate = millis();
    stripDay.clear(); stripVong.clear();
    currentHue += 5000;
    for (int i = 0; i < 3; i++) stripDay.setPixelColor((pos + i * 7) % NUM_LEDS_DAY, stripDay.ColorHSV(currentHue, 255, 255));
    for (int i = 0; i < 4; i++) stripVong.setPixelColor((pos + i * 4) % NUM_LEDS_VONG, stripVong.ColorHSV(currentHue, 255, 255));
    stripDay.show(); stripVong.show();
    pos = (pos + 1) % NUM_LEDS_DAY;
  }
}


void effectRadiantRainbow(int speed) {
  static uint16_t hueOffset = 0; 
  static uint16_t flowShift = 0; 
  static bool strobeToggle = false;
  if (millis() - lastUpdate > speed) {
    lastUpdate = millis();
    strobeToggle = !strobeToggle;
    if (strobeToggle) {
      int center = NUM_LEDS_DAY / 2;
      for (int i = 0; i < NUM_LEDS_DAY; i++) {
        uint16_t pixelHue = hueOffset + (abs(center - i) * 1200) - flowShift;
        stripDay.setPixelColor(i, stripDay.ColorHSV(pixelHue, 255, 255));
      }
      for (int i = 0; i < NUM_LEDS_VONG; i++) {
        uint16_t pixelHue = hueOffset + (i * (65536 / NUM_LEDS_VONG)) - flowShift;
        stripVong.setPixelColor(i, stripVong.ColorHSV(pixelHue, 255, 255));
      }
    } else {
      stripDay.clear(); stripVong.clear();
    }
    stripDay.show(); stripVong.show();
    hueOffset += 3000; flowShift += 5000;
  }
}

void effectBeatStrobe(uint32_t color, int speed) {
  if (millis() - lastUpdate > speed) {
    lastUpdate = millis();
    strobeState = !strobeState;
    if (strobeState) {
      for(int i=0; i<NUM_LEDS_DAY; i++) stripDay.setPixelColor(i, color);
      for(int i=0; i<NUM_LEDS_VONG; i++) stripVong.setPixelColor(i, color);
    } else {
      stripDay.clear(); stripVong.clear();
    }
    stripDay.show(); stripVong.show();
  }
}


void effectFastChaseDouble(int speed) {
  static int pos = 0;
  if (millis() - lastUpdate > speed) {
    lastUpdate = millis();
    stripDay.clear(); stripVong.clear();
    for (int i = 0; i < 3; i++) {
      stripDay.setPixelColor((pos + i) % NUM_LEDS_DAY, stripDay.Color(255, 255, 0));
      stripDay.setPixelColor((NUM_LEDS_DAY - 1 - (pos + i)) % NUM_LEDS_DAY, stripDay.Color(0, 255, 255));
      stripVong.setPixelColor((pos + i) % NUM_LEDS_VONG, stripVong.Color(255, 255, 0));
      stripVong.setPixelColor((NUM_LEDS_VONG - 1 - (pos + i)) % NUM_LEDS_VONG, stripVong.Color(0, 255, 255));
    }
    stripDay.show(); stripVong.show();
    pos = (pos + 1) % NUM_LEDS_DAY;
  }
}

void effectSparkle(uint32_t baseColor, int speed) {
  if (millis() - lastUpdate > speed) {
    lastUpdate = millis();
    for (int i = 0; i < NUM_LEDS_DAY; i++) stripDay.setPixelColor(i, baseColor);
    for (int i = 0; i < 10; i++) stripDay.setPixelColor(random(NUM_LEDS_DAY), stripDay.Color(255, 255, 255));
    for (int i = 0; i < NUM_LEDS_VONG; i++) stripVong.setPixelColor(i, baseColor);
    for (int i = 0; i < 4; i++) stripVong.setPixelColor(random(NUM_LEDS_VONG), stripVong.Color(255, 255, 255));
    stripDay.show(); stripVong.show();
  }
}


void effectFire(int speed) {
  if (millis() - lastUpdate > speed) {
    lastUpdate = millis();
    for (int i = 0; i < NUM_LEDS_DAY; i++) stripDay.setPixelColor(i, stripDay.Color(random(180, 255), random(0, 80), 0));
    for (int i = 0; i < NUM_LEDS_VONG; i++) stripVong.setPixelColor(i, stripVong.Color(random(180, 255), random(0, 80), 0));
    stripDay.show(); stripVong.show();
  }
}

void effectMegaBounce(int speed) {
  static int pos = 0;
  static int dir = 1;
  if (millis() - lastUpdate > speed) {
    lastUpdate = millis();
    stripDay.clear(); stripVong.clear();
    for(int i=0; i<4; i++) {
      int idxD = (pos + i) % NUM_LEDS_DAY;
      stripDay.setPixelColor(idxD, stripDay.ColorHSV(currentHue, 255, 255));
      int idxV = (pos + i) % NUM_LEDS_VONG;
      stripVong.setPixelColor(idxV, stripVong.ColorHSV(currentHue, 255, 255));
    }
    stripDay.show(); stripVong.show();
    pos += dir;
    if (pos >= NUM_LEDS_DAY - 4 || pos <= 0) dir *= -1;
    currentHue += 1000;
  }
}