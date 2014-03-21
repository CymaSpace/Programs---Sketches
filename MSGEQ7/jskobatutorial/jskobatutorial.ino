int analogPin = 0; // read from multiplexer using analog input 0
int strobePin = 2; // strobe is attached to digital pin 2
int resetPin = 3; // reset is attached to digital pin 3
int spectrumValue[7]; // to hold a2d values

void setup()
{
 Serial.begin(9600);
 pinMode(analogPin, INPUT);
 pinMode(strobePin, OUTPUT);
 pinMode(resetPin, OUTPUT);
 analogReference(DEFAULT);

 digitalWrite(resetPin, LOW);
 digitalWrite(strobePin, HIGH);

 Serial.println("MSGEQ7 test by J Skoba");
}

void loop()
{
 digitalWrite(resetPin, HIGH);
 digitalWrite(resetPin, LOW);

 for (int i = 0; i < 7; i++)
 {
 digitalWrite(strobePin, LOW);
 delayMicroseconds(30); // to allow the output to settle
 spectrumValue[i] = analogRead(analogPin);

 // comment out/remove the serial stuff to go faster
 // - its just here for show
 if (spectrumValue[i] < 10)
 {
 Serial.print(" ");
 Serial.print(spectrumValue[i]);
 }
 else if (spectrumValue[i] < 100 )
 {
 Serial.print(" ");
 Serial.print(spectrumValue[i]);
 }
 else
 {
 Serial.print(" ");
 Serial.print(spectrumValue[i]);
 }

 digitalWrite(strobePin, HIGH);
 }
 Serial.println();
}
