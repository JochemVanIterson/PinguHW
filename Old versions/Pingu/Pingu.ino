#include <Audio.h>
#include <ADC.h>

// --------------------- Pins --------------------- //
const int ledPin = 15;
const int micPin = A2;
const int buttonPin = 18;
const int presPin = A3;

// -------------------- Values ------------------- //
ADC *adc = new ADC();

AudioInputAnalog adc1(micPin);
AudioAnalyzePeak peak1;
AudioConnection  patchCord1(adc1, peak1);

double pressureValue, micValue;

double happiness = 100;

double duration;
bool ledState = true;
bool dood = false;

unsigned long foodCounter = 0;

double soundCounter = 0;
double lawaaiCounter = 0;
int recentValue[5] = {0, 0, 0, 0, 0}; // kijk sketch_nov22_stap4

double aaiCounter = 0;
double wurgCounter = 0;



/*
happiness factor: 100 = happy 0 = dood
eten = knopje         -10%(x minuten niet knopje)          dood(y minuten niet knnopje)
aaien = druksensor    +10(zacht aaien)                     dood(wurgen)
praten (geluid)       -10(te hard geluid)


knopje 0 en 1

druksensor:
aaien   langer dan 5 sec  > 0.01 < 0.4
wurgen  langer dan 5 sec  1

mic boven de 2000 binnen 1 minuut
*/

void setup() {
  Serial.begin(9600);
  Serial.println("Started...");

  AudioMemory(32);

  pinMode(ledPin,OUTPUT);
  pinMode(micPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  if (dood) {
    return;
  }

  pressureValue = adc->adc1->analogRead(presPin)/65535.;
  Serial.print("Pressure: ");
  Serial.println(pressureValue);

  if (peak1.available()) {
    micValue = peak1.read()*4096.0;
    Serial.print("Mic: ");
    Serial.println(micValue);
  }

  int buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
      foodCounter = millis();
  }

  if (millis() - foodCounter == 20000){
    happiness -= happiness * 0.1;
    //HONGER!
  }

  if (millis() - foodCounter == 60000 || lawaaiCounter >= 5 || wurgCounter >= 10000){
    dead();
    return;
  }

  if (ledState == true){
    digitalWrite(ledPin, HIGH);
  }
}

void dead() {
  ledState = false;
  digitalWrite(ledPin, LOW);
  //Dood geluid
  dood = true;
}
