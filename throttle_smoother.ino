/**
 * Akom's smooth throttle using an Arduino (Pro Micro 5V or similar)
 * 
 * Takes care of:
 * - Eliminating jerky throttle response
 * - Eliminating deadband
 * - Speed Limit (adjustable by potentiometer)
 */

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

int amt = 0;


void setup() {
    Serial.begin(115200);
    pinMode(PIN_IN, INPUT);
    pinMode(PIN_LIMIT, INPUT);
    pinMode(PIN_OUT, OUTPUT);

    // Set the switchPin as input with an internal pull-up resistor
    pinMode(switchPin, INPUT_PULLUP);

    outputValue = analogRead(PIN_IN); // initial value
}


int sigmoid_pwm(int pwm_value) {
    float x = pwm_value / 255.0;
    float y = 1.0 / (1.0 + exp(-10.0 * (x - 0.5))); // 10 controls the steepness of the curve
    return (int)(y * 255);
}

int transform_pwm_log(int pwm_value) {
    int transformed_pwm;
    
    // Ensure we only work within the valid range of PWM values (0 to 1024)
    if (pwm_value <= 0) {
        transformed_pwm = 0;
    } else if (pwm_value >= 1024) {
        transformed_pwm = 1024;
    } else {
        // Logarithmic transformation
        transformed_pwm = int(400.0 * log((pwm_value + 1) / 400.0) + 400);
    }

    return transformed_pwm;
}

void loop() {
    throttleValue = analogRead(PIN_IN);
    delta = throttleValue - outputValue; // error
    adjustmentAmount = (float) delta / (float) (delta > 0 ? INCREASE_ERROR_FACTOR : DECREASE_ERROR_FACTOR);   
    outputValue += adjustmentAmount;

     // Read the state of the switch
    switchState = digitalRead(switchPin);

    // throttle to output value map
    if (switchState == LOW) {
       //non fare nulla mi tengo la spinta warp
      
          mapped = map(
              outputValue,
              THROTTLE_MAP_IN_MIN,
              THROTTLE_MAP_IN_MAX,
              THROTTLE_MAP_OUT_MIN,
              THROTTLE_MAP_OUT_MAX
              );

    } else {
        
        //faccio uno sweep da 0 a 1024 e ritorno, per testare


    }
    
    int pwmMapped = mapped / 4; // PWM is 0-254 while our values are 0-1023
    analogWrite(
            PIN_OUT,
            pwmMapped
    );    
    
    if (Serial && (lastPrint + PRINT_DELAY) < millis()) 
    {
        lastPrint = millis();
        Serial.print("Input: ");
        Serial.print(throttleValue);
        Serial.print(" Output: ");
        Serial.print(outputValue);
        Serial.print(" Mapped: ");
        Serial.print(mapped);
        Serial.print(" +/-: ");
        Serial.print(adjustmentAmount);
        Serial.print(" pwm val: ");
        Serial.print(pwmMapped);        

        // // Print the state of the switch (0 = OFF, 1 = ON)
        if (switchState == LOW) {
          // Switch is ON (connected to GND)
          Serial.print(" Switch is ON");
        } else {
          // Switch is OFF (no connection to GND)
          Serial.print(" Switch is OFF");
        }

        Serial.println("");
    }

    delay(TICK_LENGTH_MS);

}