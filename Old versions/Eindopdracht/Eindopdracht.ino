#define BUFFERSIZE 8
#define MIN_OFFSET 5
#define MAX_OFFSET 25

#include <Audio.h>
#include <SerialFlash.h>

#include <ADC.h>

// --------------------- Pins --------------------- //
const int ledPin = 15;
const int micPin = A2;
const int buttonPin = 18;
const int pressurePin = A3;

// --------------------- Mic ---------------------- //
AudioInputAnalog       adc1(micPin);
AudioAnalyzePeak       peak1;
AudioConnection        patchCord1(adc1, peak1);

// --------------------- Sine --------------------- //
//AudioSynthWaveformSine   sine1;          //xy=156,145
//AudioOutputAnalog        dac1;           //xy=396,136
//AudioConnection          patchCord2(sine1, dac1);

// --------------------- Analog Inputs ---------------------- //
ADC *adc = new ADC();

// --------------------- Values ------------------- //
bool ledState = false;
bool buttonState = false;
int  pressureState = 0;

// --------------------- Setup -------------------- //
void setup() {
 pinMode(ledPin,      OUTPUT);
 pinMode(micPin,      INPUT);
//  pinMode(buttonPin,   INPUT_PULLUP);
//  pinMode(pressurePin, INPUT);

 Serial.begin(9600);

 AudioMemory(12);
//  sine1.amplitude(0.);
//  sine1.frequency(1000);
}

long timeValue = 0;

void loop() {
 // --------- Timed functions --------- //
 if(timeValue!=millis()){
   timeValue = millis();
   ledBlink();
   pressureHandler(true);
 }
 micHandler(true);
//  buttonHandler(false);

 delay(1);
}

void ledBlink(){
 if((millis()%500)==0){ // 500 ms
   if(ledState = !ledState){
     digitalWrite(ledPin, HIGH);
   } else {
     digitalWrite(ledPin, LOW);
   }
 }
}

void micHandler(bool serialMic){
 if (peak1.available()) {
   double monoPeak = peak1.read()*4096.0;
   if(serialMic){
     Serial.print("Mic: ");
     Serial.println(monoPeak);
   }
 }
}

//void buttonHandler(bool serialButton){
//  bool newValue = digitalRead(buttonPin);
//  if(buttonState==newValue)return;
//  buttonState = newValue;
//  if(serialButton){
//    Serial.println(buttonState);
//  }
//  if(buttonState){ // Released
//    sine1.amplitude(0.0);
//  } else { // Pressed
//    sine1.amplitude(0.05);
//  }
//}
void pressureHandler(bool serialPressure){
 if((millis()%10)!=0)return;
 int newValue = adc->analogRead(pressurePin, ADC_1);
 if(pressureState==newValue)return;
 pressureState = newValue;
 if(serialPressure){
   Serial.print("Pressure: ");
   Serial.println(pressureState);
 }
}
