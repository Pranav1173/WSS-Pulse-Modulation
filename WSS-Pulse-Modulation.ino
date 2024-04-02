#include <LiquidCrystal.h>

LiquidCrystal lcd(3, 4, 5, 6, 7, 8);
#define pulse_ip 2
#define ledFreqPin 10   // LED for frequency indication
#define ledDutyPin 11   // LED for duty cycle indication
#define start 12
#define reset 13

unsigned long onTime, offTime;
int pulseCount = 0;
float duty, freq, period;
int consecutiveZeroRecords = 0;
int validDutyCount = 0;
const float minFreqThreshold = 35;      // Minimum frequency threshold in Hz
const float maxFreqThreshold = 52;      // Maximum frequency threshold in Hz
const float minDutyThreshold = 49.00; // Minimum duty cycle threshold in percentage
const float maxDutyThreshold = 51.00; // Maximum duty cycle threshold in percentage

void setup() {
  Serial.begin(115200);  // Start serial communication
  pinMode(pulse_ip, INPUT);
  pinMode(ledFreqPin, OUTPUT);
  pinMode(ledDutyPin, OUTPUT);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Freq:");
  lcd.setCursor(0, 1);
  lcd.print("Duty:");
}

void loop() {
  // Measure the pulse duration using pulseIn for the rising edge
  onTime = pulseIn(pulse_ip, HIGH);

  // Print the ON time
  Serial.print("ON time: ");
  Serial.print(onTime);
  Serial.println(" microseconds");

  // Increment pulse counter

  // Measure the pulse duration using pulseIn for the falling edge
  offTime = pulseIn(pulse_ip, LOW);

  // Print the OFF time
  Serial.print("OFF time: ");
  Serial.print(offTime);
  Serial.println(" microseconds");

  // Increment pulse counter
  if (onTime > 0 && offTime > 0 && freq >= minFreqThreshold && freq <= maxFreqThreshold) {
    pulseCount++;
    consecutiveZeroRecords = 0;
  } else {
    // Check for 10 consecutive records with onTime and offTime equal to 0
    static int consecutiveZeroRecords = 0;
    if (++consecutiveZeroRecords >= 10) {
      pulseCount = 0;
      consecutiveZeroRecords = 0;
    }
  }

  // Print the pulse count
  Serial.print("Pulse count: ");
  Serial.println(pulseCount);

  // Calculate frequency and duty cycle
  period = onTime + offTime;
  freq = 1000000.0 / period;
  duty = (onTime / (float)period) * 100;

  // LED indication based on frequency range
  if (freq >= minFreqThreshold && freq <= maxFreqThreshold) {
    digitalWrite(ledFreqPin, HIGH);  // Turn on the frequency LED if within the desired range
  } else {
    digitalWrite(ledFreqPin, LOW);   // Turn off the frequency LED if outside the desired range
  }
  /*if(pulseCount > 0 && ( freq < minFreqThreshold || freq > maxFreqThreshold)){
    pulseCount = 0;
    validDutyCount = 0;
  }*/

  // LED indication based on duty cycle
  if (freq >= minFreqThreshold && freq <= maxFreqThreshold && duty >= minDutyThreshold && duty <= maxDutyThreshold) {
    validDutyCount++;
    //digitalWrite(ledDutyPin, HIGH);  // Turn on the duty cycle LED if within the desired range
  } else {
    //digitalWrite(ledDutyPin, LOW);   // Turn off the duty cycle LED if outside the desired range
  }

  //Reseting the validDutyCount after Pulse Counter becomes zero due to any condition
  if(pulseCount == 0){
    validDutyCount = 0;
  }

  if ((pulseCount) == 90) {
    pulseCount = 0;
    if(validDutyCount >= 80 ) {
      lcd.clear();
      lcd.print("OK");
      Serial.println("OK");
      digitalWrite(ledDutyPin, HIGH);  // Turn on the duty cycle LED for 5 seconds
      delay(5000);
      digitalWrite(ledDutyPin, LOW);
      // Turn off the duty cycle LED
      validDutyCount = 0;
    } else {
      lcd.clear();
      lcd.print("Not OK");
      Serial.println("Not OK");
      validDutyCount = 0;
      delay(5000);
    }
  }

  // Print frequency, duty cycle, and pulse durations
  Serial.print("Frequency: ");
  Serial.print(freq, 2);
  Serial.print(" Hz   Duty Cycle: ");
  Serial.print(duty, 2); // Print duty cycle with 2 decimal points
  Serial.print("%   PulseIn (Rising): ");
  Serial.print(onTime);
  Serial.print(" PulseIn (Falling): ");
  Serial.println(offTime);
  Serial.println("Valid Duty Cycles: ");
  Serial.println(validDutyCount);

  // Clear LCD only when frequency is above the threshold
  lcd.clear();
  lcd.print("Freq:");

  lcd.setCursor(0, 1);
  lcd.print("Duty:");

  lcd.setCursor(6, 0);
  lcd.print("           ");
  lcd.setCursor(6, 0);

  if (freq < 1000) {
    lcd.print(freq, 2);
    lcd.print("Hz");
  } else if (freq >= 1000 && freq < 1000000) {
    lcd.print(freq / 1000);
    lcd.print("KHz");
  } else if (freq >= 1000000) {
    lcd.print(freq / 1000000);
    lcd.print("MHz");
  }

  lcd.setCursor(6, 1);
  lcd.print("           ");
  lcd.setCursor(6, 1);
  lcd.print(duty, 2); // Print duty cycle on LCD with 2 decimal points
  lcd.print('%');
}

