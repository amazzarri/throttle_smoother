
// Input pin for throttle signal
#define PIN_IN A3
// Output pin to the controller
#define PIN_OUT 10
// Input pin for the speed limiting potentiometer
#define PIN_LIMIT A4
// ms delay between debugging prints
#define PRINT_DELAY 100

/**
 * All the ranges below can be determined by watching the serial console and twisting the throttle
 * Note that they will be slightly wrong if the controller supplies less than 5v to throttle.
 */

/* fine tune the throttle range to eliminate deadband */
// Normal range of throttle
#define THROTTLE_MAP_IN_MIN 0
#define THROTTLE_MAP_IN_MAX 4095

// Range we want to send to the controller
// values from "no movement yet" to "max speed":
//#define THROTTLE_MAP_OUT_MIN 390
//#define THROTTLE_MAP_OUT_MAX 800

#define THROTTLE_MAP_OUT_MIN 0
#define THROTTLE_MAP_OUT_MAX 1024


/* Jerkiness Mitigation */
// how quickly to adjust output, larger values are slower
//#define INCREASE_ERROR_FACTOR 170
//#define DECREASE_ERROR_FACTOR 20

#define INCREASE_ERROR_FACTOR 20
#define DECREASE_ERROR_FACTOR 20


// Basically delay between loops:
#define TICK_LENGTH_MS 1

// operational variables

int throttleValue = 0;      //input value from 3-wire throttle 
int limitValue = 0;         //input value from potentiometer 
int delta = 0;              //computed error from last setting (outputValue)
float outputValue = 0;      // 0-1024, later mapped to 0-255
float adjustmentAmount = 0; // 
int mapped = 0;             // outputValue after mapping to controller range
unsigned long lastPrint = 0;

const int switchPin = 15;  // Pin where the switch is connected
int switchState = 0;       // Variable to store the state of the switch
bool lastButtonState = HIGH;

int amt = 0;


int steppedOutput = 0; //test output da mandare al motore


void setup() {

    Serial.begin(115200);

    //setup pin acceleratore
    pinMode(PIN_IN, INPUT);

    //setup pin output motore (non pwm, si usa l'output analogico anche se è più impreciso, secondo documentazione Esp32. Nell'S3 mancano le due uscite DAC per cui per
    //gestire l'output digitale servirebbe un filtro passabasso in uscita)
    pinMode(PIN_OUT, OUTPUT);

    // Set the switchPin as input with an internal pull-up resistor
    pinMode(switchPin, INPUT_PULLUP);
}


void loop() 
{
  //legge l'eventuale input dello switch
  bool currentState = digitalRead(switchPin);

   // Detect button press (falling edge)
  if ((lastButtonState == HIGH && currentState == LOW) || (lastButtonState == LOW && currentState == HIGH)) 
  {
    //manda output al motore per tot secondi, durante i quali la manetta va portata al livello desiderato e accettabile per quella velocità di rotazione del motore
    //ogni secondo deve stampare i valori di manetta e output, assieme a quelli del ciclo precedente, in modo da avere lo storico nel serial monitor
    for (int i = 0; i < 20; i++) {  
        throttleValue = analogRead(PIN_IN);
        analogWrite(PIN_OUT, steppedOutput); 
        Serial.print("Output: "); 
        Serial.print(steppedOutput); 
        Serial.print(" --- Input: "); 
        Serial.println(throttleValue); 
        delay(1000); // Wait for 1 second
    }
    steppedOutput += 10;
  }

  lastButtonState = currentState;

  delay(TICK_LENGTH_MS);

}











