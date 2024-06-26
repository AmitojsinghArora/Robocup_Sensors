#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Servo.h>

// Define the pin and number of LEDs in the ring
#define LED_PIN    6
#define NUM_LEDS   12 // Adjust this to the number of LEDs in your ring

// Define the pin for the single LED
#define LASER_PIN 13

// Define the pin for the servo motor
#define SERVO_PIN 9

// Define the digital pin for the SEN-KY024LM Hall Sensor
#define HALL_SENSOR_PIN 7

// Create a NeoPixel object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Create a Servo object
Servo myServo;

// Variables to store LED states, brightness, and servo angle
bool hallON = false;
bool laserOn = false;
bool ringLedOn = false;
int brightness = 255;
int servoAngle = 90;

void setup() {
  // Initialize the NeoPixel strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  // Initialize the single LED pin
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW); // Ensure single LED is off

  // Initialize the servo motor
  myServo.attach(SERVO_PIN);
  myServo.write(servoAngle); // Set initial servo position

  // Initialize serial communication
  Serial.begin(9600);
  Serial.println("Enter commands: LASER_ON, LASER_OFF, RING_ON, RING_OFF, HALL_ON, HALL_OFF, BRIGHTNESS <0-255>, SERVO <0-180>");
}

void loop() {
  // Check for incoming serial commands
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // Read the command from the serial port
    command.trim(); // Remove any trailing whitespace or newline characters

    // Handle the commands for the Laser
    if (command.equalsIgnoreCase("LASER_ON")) {
      laserOn = true;
      digitalWrite(LASER_PIN, HIGH); // Turn the Laser on
    //  Serial.println("Laser is ON");
    } else if (command.equalsIgnoreCase("LASER_OFF")) {
      laserOn = false;
      digitalWrite(LASER_PIN, LOW); // Turn the Laser off
    //  Serial.println("Laser is OFF");
    } 
    // Handle the commands for the LED ring
    else if (command.equalsIgnoreCase("RING_ON")) {
      ringLedOn = true;
      applyLEDState();
    //  Serial.println("LED ring is ON");
    } else if (command.equalsIgnoreCase("RING_OFF")) {
      ringLedOn = false;
      applyLEDState();
    //  Serial.println("LED ring is OFF");
    } else if (command.startsWith("BRIGHTNESS ") && ringLedOn) {
      int value = command.substring(11).toInt();
      if (value >= 0 && value <= 255) {
        brightness = value;
        applyLEDState();
      //  Serial.print("Brightness set to ");
      //  Serial.println(brightness);
      } else {
      //  Serial.println("Invalid brightness value. Use a value between 0 and 255.");
      }
    } 
    // Handle the commands for the servo motor
    else if (command.startsWith("SERVO ")) {
      int angle = command.substring(6).toInt();
      if (angle >= 0 && angle <= 180) {
        servoAngle = angle;
        myServo.write(servoAngle);
      //  Serial.println("Servo angle set to " + servoAngle);
      } else {
      //  Serial.println("Invalid servo angle. Use a value between 0 and 180.");
      }
    } 
    // Handle the commands for the Hall sensor
    else if (command.equalsIgnoreCase("HALL_ON")) {
      hallON = true;
      while (hallON) {
        // Read the Hall sensor value
        int magnetDetection = digitalRead(HALL_SENSOR_PIN);
        if (magnetDetection == HIGH) {
          Serial.println("Magnet detected");
        } else {
          Serial.println("No magnet detected");
        }
        delay(100); // Adjust the delay as needed

        // Check for incoming serial commands while the Hall sensor is on
        if (Serial.available() > 0) {
          String newCommand = Serial.readStringUntil('\n');
          newCommand.trim();
          if (newCommand.equalsIgnoreCase("HALL_OFF")) {
            hallON = false;
          }
        }
      }
    } else if (command.equalsIgnoreCase("HALL_OFF")) {
      hallON = false;//  Serial.println("Hall sensor is now disabled");
    } else if (command.equalsIgnoreCase("HELP") or command.equalsIgnoreCase("COMMANDS")) {
      Serial.println("Enter commands: LASER_ON, LASER_OFF, RING_ON, RING_OFF, HALL_ON, HALL_OFF, BRIGHTNESS <0-255>, SERVO <0-180>");
    }
  }
}

// Apply the current LED state and brightness for the LED ring
void applyLEDState() {
  if (ringLedOn) {
    strip.setBrightness(brightness);
    rainbowCycle(10); // Display the rainbow cycle with a delay of 10ms
  } else {
    strip.clear();
    strip.show();
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbowCycle(int wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    if (!ringLedOn) return; // Exit the loop if the LEDs are turned off
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}
