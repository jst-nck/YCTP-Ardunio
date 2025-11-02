
#define LED_PIN 11 // led strip pin
#define LED_NUM 3 // leds amount (1 for each question)

#define OK_BTN 12 // OK button pin

#include <FastLED.h> // led strip
#include <LiquidCrystal_I2C.h> // lcd
#include <Keypad.h> // keypad

const byte ROWS = 4; // Number of rows
const byte COLS = 3; // Number of columns

// keypad layout
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'C','0','-'}
};

byte rowPins[ROWS] = {7, 8, 9, 10}; // Connect to Arduino pins
byte colPins[COLS] = {4, 3, 2}; // Connect to Arduino pins

CRGB leds[LED_NUM]; 
Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
LiquidCrystal_I2C lcd(0x27,16,2); 

int question = 0; // question #

int num1 = 9; // first number
int num2 = 9; // second number

bool pressedOk = false; // flag for OK button

int answer = 0; // right answer

String inputAnswer = ""; // answer from user

bool answeredWrong = false; // flag if any question was answered incorrectly

void setup() {
  pinMode(OK_BTN, INPUT_PULLUP); // ok button

  Serial.begin(9600); // start serial

  // lcd initialization
  lcd.init();
  lcd.backlight();

  // led strip initialization
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, LED_NUM);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 500);
  FastLED.setBrightness(50);

  // debugging
  leds[0] = CRGB::Red;
  leds[1] = CRGB::Green;
  leds[2] = CRGB::Blue;
  FastLED.show();

  lcd.setCursor(0,0);
  lcd.print("LOAD LOAD");
  lcd.setCursor(0,1);
  lcd.print("STILL LOAD");
  delay(1000);

  // clear leds
  FastLED.clear();
  FastLED.show();

  question = 1;
  generateQuestion();
}

void loop() {
  if (digitalRead(OK_BTN) == LOW){ // if button pressed:
    if (!pressedOk){ // if flag set to false
      checkAnswer();
      pressedOk = true; // set flag to true 
    }
  }
  else if (digitalRead(OK_BTN) == HIGH){ // else if button is released
    pressedOk = false; // reset flag
  }
  char customKey = customKeypad.getKey(); // Read for a key press

  if (customKey){ // If a key is pressed
    if (customKey == 'C'){
        inputAnswer = "";
    }
    else if (customKey == '-'){
        if (inputAnswer.charAt(0) == "-"){
            inputAnswer.remove(0, 1);
            Serial.println(inputAnswer);
        }
        else {
            Serial.println(inputAnswer.toInt() * -1);
            inputAnswer = String(inputAnswer.toInt() * -1);
        }
    }
    else {
        inputAnswer += customKey;
    }
    Serial.println(inputAnswer);
    lcd.setCursor(16-inputAnswer.length(),1);
    lcd.print(inputAnswer);
  }
}

// question generation
void generateQuestion(){
  randomSeed(5); // seed for randomization (A0 STAYS DISCONNECTED!!!)analogRead(0)

  num1 = random(0,10);
  num2 = random(0,10);
  String Operator = "+";

  if (random(0,2) == 1){
    Operator = "+";
  }
  else {
    Operator = "-";
  }

  if (Operator == "+"){ // if addition, then add
    answer = num1 + num2;
  }
  else { // else, subtract
    answer = num1 - num2;
  }

  // QUESTION Q#:
  Serial.println("ANSWER: ");
  Serial.println(answer);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("QUESTION Q");
  lcd.setCursor(10, 0);
  lcd.print(question);
  lcd.setCursor(11, 0);
  lcd.print(":");

  // a+b=
  lcd.setCursor(0,1);
  lcd.print(num1);
  lcd.setCursor(1,1);
  lcd.print(Operator);
  lcd.setCursor(2, 1);
  lcd.print(num2);
  lcd.setCursor(3, 1);
  lcd.print("=");
}

// check if the answer is correct
void checkAnswer(){
  // if correct
  if (inputAnswer.toInt() != answer){
    leds[3-question] = CRGB::Red; // set led to red
    FastLED.show(); // update strip
    answeredWrong = true; // set flag to true
  }
  else {
    leds[3-question] = CRGB::Green; // set led to green
    FastLED.show(); // update strip
  }

  

  if (question < 3){ // if its not third question
    question = question + 1; // next question
    generateQuestion();
    inputAnswer = "";
  }

  else{
    lcd.clear();
    lcd.setCursor(0, 0);
    if (!answeredWrong){ // if user is smart
      // WOW! YOU EXIST!
      lcd.print("WOW! YOU EXIST!");
    }
    else{
      // I HEAR EVERY 
      // DOOR YOU OPEN
      lcd.print("I HEAR EVERY");
      lcd.setCursor(0, 1);
      lcd.print("DOOR YOU OPEN");
    }
  }
}