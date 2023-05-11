#include <TM1637Display.h> 

const int LAMP1 = 2; 
const int LAMP2 = 0; 
const int LAMP3 = 4; 
const int LAMP4 = 16; 
const int LAMP5 = 17; 
const int Startknopf = 5; 
const int CLK = 18; 
const int DIO = 19; 
const int SECRET_BUTTON = 21;      //Das ist der zum deaktivieren vom Delay!!!
const int SECRET_SOURCE = 23;
bool lastTimeSecretPressed = false;
bool ReactionTimeActive = false; 

unsigned long old_time;
const int stepSize = 20;
const int maxDelay = 60;
const int minDelay = -20;
int actualArtificialDelay = 0; 

TM1637Display display(CLK, DIO); 

void setup() { 
  Serial.begin(9600); 

  pinMode(LAMP1, OUTPUT); 
  pinMode(LAMP2, OUTPUT); 
  pinMode(LAMP3, OUTPUT); 
  pinMode(LAMP4, OUTPUT); 
  pinMode(LAMP5, OUTPUT); 
  pinMode(Startknopf, INPUT_PULLDOWN); 
  pinMode(CLK, INPUT); 
  pinMode(DIO, INPUT); 
  pinMode(SECRET_BUTTON, INPUT_PULLDOWN); 
  pinMode(SECRET_SOURCE, OUTPUT); 


  digitalWrite(SECRET_SOURCE, HIGH);
  clearAll(); 
  display.setBrightness(0x0a); 

} 

void startSequence() { 
  Serial.println("LED 1 activated"); 
  digitalWrite(LAMP1, HIGH); 
  delay(1000); 
  Serial.println("LED 2 activated"); 
  digitalWrite(LAMP2, HIGH); 
  delay(1000); 
  Serial.println("LED 3 activated"); 
  digitalWrite(LAMP3, HIGH); 
  delay(1000); 
  Serial.println("LED 4 activated"); 
  digitalWrite(LAMP4, HIGH); 
  delay(1000); 
  Serial.println("LED 5 activated"); 
  allLEDsOn(); 
} 

void earlyStart() { 
  Serial.println("Early start detected! Aborting..."); 
  for (int i = 0; i < 4; i++) { 
    allLEDsOn(); 
    delay(500); 
    clearLEDs(); 
    delay(500); 
  } 
} 

void loop() {
  if (digitalRead(SECRET_BUTTON) == HIGH && lastTimeSecretPressed == false && ReactionTimeActive == false) {

    if (actualArtificialDelay >= maxDelay) {
      actualArtificialDelay = minDelay;
    } else {
      actualArtificialDelay += stepSize;
    }

    Serial.println("Artificial delay set to " +  String(actualArtificialDelay)+ "ms!");
    lastTimeSecretPressed = true;

    display.showNumberDecEx(actualArtificialDelay);
    delay(500);
    display.clear();

  }
  else if (digitalRead(SECRET_BUTTON) == LOW && lastTimeSecretPressed == true) {
    lastTimeSecretPressed = false;
  }

  if (digitalRead(Startknopf) == HIGH && ReactionTimeActive == false) { 
    Serial.println("\n\nGame initialized: "+String(actualArtificialDelay)+"ms"); 
    bool wasEarly = false; 
    ReactionTimeActive = true; 
    startSequence(); 

    for (int i = 0; i < 2000+random(2000); i++){ 
      if (digitalRead(Startknopf) == HIGH && wasEarly == false) { 
        earlyStart(); 
        wasEarly = true; 
        break; 
      } 
      if (!wasEarly) {
        delay(1); 
      }
    } 

    if (wasEarly) { 
      ReactionTimeActive = false; 
      return; 
    } 

    clearAll(); 
    old_time = millis()-actualArtificialDelay; 
  } 
  else if (digitalRead(Startknopf) == HIGH && ReactionTimeActive == true) { 
    ReactionTimeActive = false; 
    Serial.println("Start Button pressed!"); 
    showResult(millis()-old_time); 
    Serial.println(millis()-old_time); 

    delay(3000); 
    clearAll(); 
  } 
  else { 
    if (ReactionTimeActive == true) { 
      int time = millis()-old_time;
      showResult(time);
    } 
    if (ReactionTimeActive == true && millis()-old_time>= 10000) { 
      display.showNumberDecEx(1000, 0b01000000, true);
      earlyStart();
      ReactionTimeActive = false;
      clearAll();
    } 
  } 
} 

void clearLEDs() { 
  digitalWrite(LAMP1, LOW); 
  digitalWrite(LAMP2, LOW); 
  digitalWrite(LAMP3, LOW); 
  digitalWrite(LAMP4, LOW); 
  digitalWrite(LAMP5, LOW); 
  Serial.println("LEDs Cleared!"); 
} 

void clearAll() { 
  display.clear(); 
  clearLEDs(); 
  Serial.println("Everything cleared!"); 
} 

void allLEDsOn() { 
  digitalWrite(LAMP1, HIGH); 
  digitalWrite(LAMP2, HIGH); 
  digitalWrite(LAMP3, HIGH); 
  digitalWrite(LAMP4, HIGH); 
  digitalWrite(LAMP5, HIGH); 
} 

int getTimePassed() {
  return millis()-old_time;
}

void showResult(int millisecounds) { 
  display.showNumberDecEx(millisecounds, 0b10000000 ,true); 
}