#define POT_PIN A0  // Define the pin where the potentiometer is connected
#define PWM_PIN 13   // Define the pin where the PWM signal will be output
#include <SAMD21turboPWM.h>

#define POT_PIN A0 // Define the pin where the potentiometer is connected

TurboPWM pwm;
const int voltagePin = A1; // analog input pin for voltage detection
const int numReadings = 10; // number of readings for averaging
const int relayPin = 10; // the base of the transistor attached to

const int BUTTON_PIN = 2;  // Connect the Button to pin 7 or change here
const int RELAY_PIN    = 11; // Connect the LED to pin 3 or change here
  
// variables will change:
int relayState = LOW;     // tracks the current state of LED
int lastButtonState;    // the previous state of button
int currentButtonState;

float readings[numReadings]; // array to store readings
int readIndex = 0; // current reading index
float total = 0; // total of readings
float average = 0; // average of readings

void setup() {
  pinMode(POT_PIN, INPUT); // Set the potentiometer pin as input
  pwm.setClockDivider(200, false); // Main clock divided by 200 => 240KHz
  pwm.timer(2, 4, 50, false);   // Use timer 2 for pin 13, divide clock by 4, resolution 60000, dual-slope PWM
  pinMode(POT_PIN, INPUT); // Set the potentiometer pin as input
  pinMode(PWM_PIN, OUTPUT); // Set the PWM pin as output
  pinMode(relayPin, OUTPUT);   // initialize the relayPin as an output
  digitalWrite(relayPin, LOW); // initialize the relay to be off
  Serial.begin(9600); // initialize serial communication for debugging
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }

  Serial.begin(9600);                // initialize serial
  pinMode(BUTTON_PIN, INPUT_PULLUP); // set arduino pin to input pull-up mode
  pinMode(RELAY_PIN, OUTPUT);          // set arduino pin to output mode
  
  currentButtonState = digitalRead(BUTTON_PIN);


}

void loop() {
  int potValue = analogRead(POT_PIN); // Read the value from the potentiometer
  int dutyCycle = map(potValue, 0, 1023, 0, 1000); // Map the potentiometer value to a duty cycle value
  pwm.analogWrite(13, dutyCycle); // Set the duty cycle based on the potentiometer value

  int potValue = analogRead(POT_PIN); // Read the value from the potentiometer
  int dutyCycle = map(potValue, 0, 1023, 0, 255); // Map the potentiometer value to a duty cycle value (0-255 for 8-bit PWM)
  analogWrite(PWM_PIN, dutyCycle); // Set the duty cycle based on the potentiometer value
  total = total - readings[readIndex];
  // Read the voltage input
  readings[readIndex] = analogRead(voltagePin) * (4.8 / 1023.0);

  // Handle floating pin (optional)
  if (readings[readIndex] < 0.01) {
    readings[readIndex] = 0.0; // Assume floating pin as 0V
  }

  // Add the reading to the total
  total = total + readings[readIndex];
  // Advance to the next position in the array
  readIndex = readIndex + 1;

  // If we're at the end of the array, wrap around to the beginning
  if (readIndex >= numReadings) {
    readIndex = 0;
  }

  // Calculate the average
  average = total / numReadings;

  // Debugging output
  Serial.print("Average Voltage: ");
  Serial.println(average);

  // Check if the average voltage is greater than 0.5V
  if (average > 3.28) {
    digitalWrite(relayPin, LOW); // turn on the relay
  } else {
    digitalWrite(relayPin, HIGH); // turn off the relay
  }

  delay(100);

  lastButtonState    = currentButtonState;      // save the last state
  currentButtonState = digitalRead(BUTTON_PIN); // read new state
  
  if(lastButtonState == HIGH && currentButtonState == LOW) {
    Serial.println("The button is pressed");
  
    // toggle state of LED
    if(relayState == LOW)
       relayState = HIGH;
    else
      relayState = LOW;
  
    // control relay arccoding to the toggled state
    digitalWrite(RELAY_PIN, relayState); 
  }

}
