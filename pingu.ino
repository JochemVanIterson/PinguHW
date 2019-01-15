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

#define BUFFERSIZE 8
#define MIN_OFFSET 5
#define MAX_OFFSET 25

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
AudioInputAnalog       adc1(micPin);
AudioAnalyzePeak       peak1;
AudioConnection        patchCord1(adc1, peak1);

// -------------------------------------------------------------------------- //
// ---------------------------------- DACs ---------------------------------- //
// -------------------------------------------------------------------------- //
// --------------------- Sine --------------------- //
AudioSynthWaveformSine   sine1;          //xy=156,145
AudioOutputAnalog        dac1;           //xy=396,136
AudioConnection          patchCord2(sine1, dac1);

// -------------------------------------------------------------------------- //
// ------------------------------ Input Values ------------------------------ //
// -------------------------------------------------------------------------- //
double pressureValue, micValue;
bool ledIsOn, buttonState = false;
int audioBuffer[BUFFERSIZE] = {0};
int ledState = 1; // 0:Off, 1:On, 2:Blink

// -------------------------------------------------------------------------- //
// ------------------------------ Life states ------------------------------- //
// -------------------------------------------------------------------------- //
double happiness = 100; // 0 <= happiness <= 100
bool dood = false;
unsigned long lastEatenTime = 0; // Button -> millis()
unsigned long firstAaiTime  = 0;
bool hasAaied = false;
unsigned long firstWurgTime = 0;
bool hasWurged = false;

// -------------------------------------------------------------------------- //
// --------------------------------- Setup ---------------------------------- //
// -------------------------------------------------------------------------- //
void setup() {
    Serial.begin(9600);
    Serial.println("Started...");

    pinMode(buttonPin, INPUT_PULLUP);

    AudioMemory(12);
}

// -------------------------------------------------------------------------- //
// ---------------------------------- Loop ---------------------------------- //
// -------------------------------------------------------------------------- //
void loop() {
    if(dood)return; // Do nothing when dead
    getVariables(true);
    ledHandler();

    if (millis() - lastEatenTime == 2000){
      setHappiness(happiness*0.9);
      //HONGER!
    }

    if (millis() - lastEatenTime == 60000 /*|| lawaaiCounter >= 5 || wurgCounter >= 10000 */){
      sterf();
      return;
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
    double newValue = peak1.read()*4096.0;
    if(newValue==micValue)return;
    micValue = newValue;
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
    lastEatenTime = millis();
  }
}
void handlePressureChanged(){
  if(pressureValue>=0.01 && pressureValue<=0.4){
    if(firstAaiTime==0) firstAaiTime = millis();
  } else {
    firstAaiTime = 0;
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
  //FIXME

  // else if( millis() % (happiness*10.) == 0){ // 500 ms
  //   if(ledIsOn = !ledIsOn){
  //     digitalWrite(ledPin, HIGH);
  //   } else {
  //     digitalWrite(ledPin, LOW);
  //   }
  // }
}

// -------------------------------------------------------------------------- //
// -------------------------------- Actions --------------------------------- //
// -------------------------------------------------------------------------- //
void sterf(){
  ledState = false;
  digitalWrite(ledPin, LOW);
  //TODO: Dood geluid
  dood = true;
}
void setHappiness(int newValue){
  if(newValue<0||newValue>100)return;
  happiness = newValue;
  Serial.print("Happiness: ");
  Serial.println(happiness);
}
