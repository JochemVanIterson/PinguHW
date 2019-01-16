/*
happiness factor: 100 = happy 0 = dood
eten = knopje         -10%(x minuten niet knopje)          dood(y minuten niet knnopje)
aaien = druksensor    +10(zacht aaien)                     dood(wurgen)
praten (geluid)       -10(te hard geluid)

happiness <= 40:
led knipperen, afhankelijk van happiness

knopje 0 en 1

druksensor:
aaien   langer dan 5 sec  > 0.01 < 0.4
wurgen  langer dan 5 sec  1

mic boven de 2000 binnen 1 minuut
*/

#include <Audio.h>
#include <ADC.h>
#include <math.h>

// -------------------------------------------------------------------------- //
// ------------------------------ Audio files ------------------------------- //
// -------------------------------------------------------------------------- //
#include "sounds/AudioSampleAaien_nika_2.cpp"
#include "sounds/AudioSamplePurr_eva_1.cpp"

#include "sounds/AudioSampleAu_nika_1.cpp"
#include "sounds/AudioSampleAu_eva_1.cpp"

#include "sounds/AudioSampleEten_nika_1.cpp"
#include "sounds/AudioSampleNomnomnom_eva_1.cpp"

#include "sounds/AudioSampleHonger_nika_1.cpp"
#include "sounds/AudioSampleHonger_eva_1.cpp"

#include "sounds/AudioSampleHuilen_nika_2.cpp"
#include "sounds/AudioSampleHuilen_eva_1.cpp"

#include "sounds/AudioSampleDood.cpp"
AudioPlayMemory    sound0;

#define BUFFERSIZE 100

// -------------------------------------------------------------------------- //
// ---------------------------------- Pins ---------------------------------- //
// -------------------------------------------------------------------------- //
const int ledPin = 15;
const int micPin = A2;
const int buttonPin = 18;
const int pressurePin = A3;

// -------------------------------------------------------------------------- //
// ---------------------------------- ADCs ---------------------------------- //
// -------------------------------------------------------------------------- //
// --------------------- Analog Inputs ------------ //
ADC *adc = new ADC();

// --------------------- Mic ---------------------- //
AudioInputAnalog         adc1(micPin);
AudioAnalyzePeak         peak1;
AudioConnection          patchCord1(adc1, peak1);

// -------------------------------------------------------------------------- //
// ---------------------------------- DACs ---------------------------------- //
// -------------------------------------------------------------------------- //
// --------------------- Sine --------------------- //
AudioOutputAnalog        dac1;           //xy=396,136
AudioConnection          patchCord2(sound0, dac1);

// -------------------------------------------------------------------------- //
// ------------------------------ Input Values ------------------------------ //
// -------------------------------------------------------------------------- //
double pressureValue, micValue;
bool ledIsOn, buttonState = false;
double audioBuffer[BUFFERSIZE] = {0};
int bufferCursor = 0;
int ledState = 1; // 0:Off, 1:On, 2:Blink

// -------------------------------------------------------------------------- //
// ------------------------------ Life states ------------------------------- //
// -------------------------------------------------------------------------- //
double happiness = 40.0; // 0 <= happiness <= 100
bool dood = false;
unsigned long lastEatenTime = 0; // Button -> millis()
unsigned long firstAaiTime  = 0;
bool hasAaied = false;
unsigned long firstWurgTime = 0;
bool hasWurged = false;
unsigned long lastMillis = 0;
bool hasHardGeluid = false;

// -------------------------------------------------------------------------- //
// --------------------------------- Setup ---------------------------------- //
// -------------------------------------------------------------------------- //
void setup() {
    Serial.begin(9600);
    Serial.println("Started...");

    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);

    randomSeed(adc->adc1->analogRead(A20));

    AudioMemory(12);
}

// -------------------------------------------------------------------------- //
// ---------------------------------- Loop ---------------------------------- //
// -------------------------------------------------------------------------- //
void loop() {
    if(dood)return; // Do nothing when dead
    getVariables(true);
    if(lastMillis!=millis()){
      ledHandler();
      if ((millis() - lastEatenTime) == 10000){
        setHappiness(happiness*0.9);
        if(happiness>40){
          int randomVal = random(2);
          if(randomVal==0) sound0.play(AudioSampleHonger_nika_1);
          else if(randomVal==1) sound0.play(AudioSampleHonger_eva_1);
        } else {
          int randomVal = random(6);
          if(randomVal==0) sound0.play(AudioSampleHonger_nika_1);
          else if(randomVal==1) sound0.play(AudioSampleHonger_eva_1);
          else if(randomVal==2) sound0.play(AudioSampleHuilen_eva_1);
          else if(randomVal==3) sound0.play(AudioSampleHuilen_nika_2);
          else if(randomVal==4) sound0.play(AudioSampleHuilen_nika_2);
          else if(randomVal==5) sound0.play(AudioSampleHuilen_nika_2);
        }

        lastEatenTime = millis();
      }

      if (millis() - lastEatenTime == 60000 /*|| lawaaiCounter >= 5 || wurgCounter >= 10000 */){
        sterf();
        return;
      }
      lastMillis = millis();
    }
}

// -------------------------------------------------------------------------- //
// ----------------------------- Get variables ------------------------------ //
// -------------------------------------------------------------------------- //
void getVariables(bool print){
  micValueGetter(print);
  pressureValueGetter(print);
  buttonValueGetter(print);
}

void micValueGetter(bool print){
  if (peak1.available()) {
    double newValue = peak1.read();

    audioBuffer[bufferCursor] = newValue;
    bufferCursor = (bufferCursor+1)%BUFFERSIZE;

    if(newValue==micValue)return;
    micValue = newValue;
    handleMicChanged();
    if(print){
      Serial.print("Mic: ");
      Serial.println(micValue);
    }
  }
}
void pressureValueGetter(bool print){
  if((millis()%10)!=0)return;
  double newValue = adc->adc1->analogRead(pressurePin)/65535.;
  if(pressureValue==newValue)return;
  if( fabs(pressureValue-newValue)<=0.01 )return;
  pressureValue = newValue;
  handlePressureChanged();
  if(print){
    Serial.print("Pressure: ");
    Serial.println(pressureValue, 10);
  }
}
void buttonValueGetter(bool print){
  bool newValue = !digitalRead(buttonPin);
  if(buttonState==newValue)return;
  buttonState = newValue;
  handleButtonChanged();
  if(print){
    Serial.print("Button: ");
    Serial.println(buttonState?"On":"Off");
  }
}

// -------------------------------------------------------------------------- //
// ------------------------ Variable changed handler ------------------------ //
// -------------------------------------------------------------------------- //
void handleButtonChanged(){
  if(!buttonState){
    // TODO only every 5 seconds
    // if ((millis() - lastEatenTime) >= 5000){
      lastEatenTime = millis();
      int randomVal = random(2);
      if(randomVal==0) sound0.play(AudioSampleEten_nika_1);
      else if(randomVal==1) sound0.play(AudioSampleNomnomnom_eva_1);
    // }
  }
}
void handlePressureChanged(){
  if(pressureValue>=0.01 && pressureValue<=0.4){
    if(firstAaiTime==0) firstAaiTime = millis();
    if((firstAaiTime - millis())>= 5000 && !hasAaied){
      hasAaied = true;
      setHappiness(happiness*1.1);
      int randomVal = random(2);
      if(randomVal==0) sound0.play(AudioSampleAaien_nika_2);
      else if(randomVal==1) sound0.play(AudioSamplePurr_eva_1);
    }
  } else if(pressureValue>=1.){
    if(firstWurgTime==0) firstWurgTime = millis();

    int randomVal = random(2);
    if(randomVal==0) sound0.play(AudioSampleAu_nika_1);
    else if(randomVal==1) sound0.play(AudioSampleAu_eva_1);

    if((firstWurgTime - millis())>= 5000){
      sterf();
    }
  } else {
    hasAaied = false;
    firstAaiTime = millis();
    firstWurgTime = millis();
  }
}
void handleMicChanged(){
  double average = averageVolume();
  if(!hasHardGeluid && average>=0.5){
    hasHardGeluid = true;
    setHappiness(happiness*0.9);
    Serial.println("HARD GELUID");
    int randomVal = random(2);
    if(randomVal==0) sound0.play(AudioSampleAu_nika_1);
    else if(randomVal==1) sound0.play(AudioSampleHuilen_eva_1);
  } else if(hasHardGeluid && average<0.2){
    hasHardGeluid = false;
  }
}

// -------------------------------------------------------------------------- //
// ----------------------------- Timed actions ------------------------------ //
// -------------------------------------------------------------------------- //
void ledHandler(){
  if(happiness>40){
    digitalWrite(ledPin, HIGH);
    return;
  }

  if( (millis() % (450 - (int)happiness*10)) == 0){
    if(ledIsOn){
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }
    ledIsOn = !ledIsOn;
  }
}

// -------------------------------------------------------------------------- //
// -------------------------------- Actions --------------------------------- //
// -------------------------------------------------------------------------- //
void sterf(){
  ledState = false;
  digitalWrite(ledPin, LOW);
  Serial.println("DOOOOOOOOOOOOOOOD");
  sound0.play(AudioSampleDood);
  dood = true;
}
void setHappiness(double newValue){
  if(newValue<0.||newValue>100.)return;
  happiness = newValue;
  Serial.print("Happiness: ");
  Serial.println(happiness);
}
double averageVolume(){
  double value = 0;
  for(int walker = 0; walker < BUFFERSIZE; walker++){
    value+=audioBuffer[walker];
  }
  if(bufferCursor==0){
    Serial.print("Average: ");
    Serial.print(value);
    Serial.print(", ");
    Serial.println(value/BUFFERSIZE);

    int bufferPrintCursor = 0;
    while (bufferPrintCursor<BUFFERSIZE){
      Serial.print(audioBuffer[bufferPrintCursor]);
      Serial.print(", ");
      bufferPrintCursor++;
    }
    Serial.println("");
  }
  return value/BUFFERSIZE;
}
