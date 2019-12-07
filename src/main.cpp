#include "Arduino.h"
#include "LiquidCrystal_I2C.h"
#include "EEPROM.h"
#include "MIDI.h"

const int debounce = 50;
const int longPress = 3000;

// bank buttons pin init
const int bankUp = 22;
const int bankDown = 23;

// preset selection buttons
const int preset1 = 30;
const int preset2 = 31;
const int preset3 = 32;

// loops buttons
const int loop1 = 40;
const int loop2 = 41;
const int loop3 = 42;
const int loop4 = 43;

// loops control
const int loopControl1 = 48;
const int loopControl2 = 49;
const int loopControl3 = 50;
const int loopControl4 = 51;

// bank and preset variables
int bankSelection = 1;
int presetSelection = 1;

// button states
bool bankUpState = false;
bool bankDownState = false;

bool preset1State = false;
bool preset2State = false;
bool preset3State = false;

bool loop1State = false;
bool loop2State = false;
bool loop3State = false;
bool loop4State = false;

int btnPressed = 0;
long pressTime = millis(); 


// 3 dimensional array for the preset storage (banks x presets x loops)
bool bank[3][3][4] = {
  {
    {false, false, false, false},
    {false, false, false, false},
    {false, false, false, false}
  },
  {
    {false, false, false, false},
    {false, false, false, false},
    {false, false, false, false}
  },
  {
    {false, false, false, false},
    {false, false, false, false},
    {false, false, false, false}
  }
};

LiquidCrystal_I2C lcd(0x27,20,4);
MIDI_CREATE_DEFAULT_INSTANCE();

void setLoops() {
  // set loop controls according to actual configuration
  digitalWrite(loopControl1, bank[bankSelection-1][presetSelection-1][0]);
  digitalWrite(loopControl2, bank[bankSelection-1][presetSelection-1][1]);
  digitalWrite(loopControl3, bank[bankSelection-1][presetSelection-1][2]);
  digitalWrite(loopControl4, bank[bankSelection-1][presetSelection-1][3]);

  // send midi message to set midi devices
  MIDI.sendProgramChange(((bankSelection-1) * 3) + presetSelection - 1, 1);

}

void updateLCD() {
  // function to write bank and preset to lcd screen
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Bank: ");
  lcd.setCursor(6,0);
  lcd.print(bankSelection);

  lcd.setCursor(0,1);
  lcd.print("Preset: ");
  lcd.setCursor(8,1);
  lcd.print(presetSelection);

  lcd.setCursor(0,2);
  lcd.print("MIDI Program: ");
  lcd.setCursor(14,2);
  lcd.print(((bankSelection-1) * 3) + presetSelection - 1);

}

void savePreset() {
  // save actual configuration of loops for pushed preset
  bank[bankSelection-1][presetSelection-1][0] = digitalRead(loopControl1);
  bank[bankSelection-1][presetSelection-1][1] = digitalRead(loopControl2);
  bank[bankSelection-1][presetSelection-1][2] = digitalRead(loopControl3);
  bank[bankSelection-1][presetSelection-1][3] = digitalRead(loopControl4);

  // store to eeprom
  EEPROM.put(0, bank);

  // LCD message
  lcd.setCursor(0,4);
  lcd.print("Preset saved!");
}

void setup() {
  // initialize inputs: 

  pinMode(bankUp, INPUT); 
  pinMode(bankDown, INPUT); 

  pinMode(loop1, INPUT); 
  pinMode(loop2, INPUT); 
  pinMode(loop3, INPUT); 
  pinMode(loop4, INPUT); 

  pinMode(preset1, INPUT); 
  pinMode(preset2, INPUT); 
  pinMode(preset3, INPUT); 
  
  // initialize outputs: 
  pinMode(loopControl1, OUTPUT); 
  pinMode(loopControl2, OUTPUT); 
  pinMode(loopControl3, OUTPUT); 
  pinMode(loopControl4, OUTPUT); 

  // load presets from eeprom
  EEPROM.get(0, bank);
  // initialize the lcd
  lcd.init();                       
  lcd.backlight();
  updateLCD();

  // init loop controls according to bank
  setLoops();

}

void loop() {

  // get button states
  bankUpState = digitalRead(bankUp);
  bankDownState = digitalRead(bankDown);

  preset1State = digitalRead(preset1);
  preset2State = digitalRead(preset2);
  preset3State = digitalRead(preset3);

  loop1State = digitalRead(loop1);
  loop2State = digitalRead(loop2);
  loop3State = digitalRead(loop3);
  loop4State = digitalRead(loop4);

  if(btnPressed == 0){
    // no button is pressed - check for button press

    // bank changes
    if (bankUpState){ 
      btnPressed = bankUp;
      pressTime = millis();
    }
    if (bankDownState){ 
      btnPressed = bankDown;
      pressTime = millis();
    }

    // preset changes
    if (preset1State){ 
      btnPressed = preset1;
      pressTime = millis();
    }
    if (preset2State){ 
      btnPressed = preset2;
      pressTime = millis();
    }
    if (preset3State){ 
      btnPressed = preset3;
      pressTime = millis();
    }

    // loop changes
    if (loop1State){ 
      btnPressed = loop1;
      pressTime = millis();
    }
    if (loop2State){ 
      btnPressed = loop2;
      pressTime = millis();
    }
    if (loop3State){ 
      btnPressed = loop3;
      pressTime = millis();
    }
    if (loop4State){ 
      btnPressed = loop4;
      pressTime = millis();
    }

  }else{
    // some button is pressed
    // bank changes
    if (btnPressed == bankUp && !bankUpState){ 
      if(millis() - pressTime > debounce){
      // change bank, update loops
        bankSelection = (bankSelection % 3) + 1;
        setLoops();
        updateLCD();
      }
      btnPressed = 0;
    }

    if (btnPressed == bankDown && !bankDownState){ 
      if(millis() - pressTime > debounce){
      // change bank, update loops
        bankSelection = ((bankSelection + 1) % 3) + 1;
        setLoops();
        updateLCD();
      }
      btnPressed = 0;
    }

    // preset changes
    if (btnPressed == preset1 && !preset1State){ 
      if(millis() - pressTime > longPress){
        // save loops
        savePreset();
      }else if(millis() - pressTime > debounce){
        // change preset, update loops
        presetSelection = 1;
        setLoops();
        updateLCD();
      }
      btnPressed = 0;
    }
    if (btnPressed == preset2 && !preset2State){ 
      if(millis() - pressTime > longPress){
        // save loops
        savePreset();
      }else if(millis() - pressTime > debounce){
        // change preset, update loops
        presetSelection = 2;
        setLoops();
        updateLCD();
      }
      btnPressed = 0;
    }
    if (btnPressed == preset3 && !preset3State){ 
      if(millis() - pressTime > longPress){
        // save loops
        savePreset();
      }else if(millis() - pressTime > debounce){
        // change preset, update loops
        presetSelection = 3;
        setLoops();
        updateLCD();
      }
      btnPressed = 0;
    }
  
    // loop changes
    if (btnPressed == loop1 && !loop1State){ 
      if(millis() - pressTime > debounce){
      // change loop
        digitalWrite(loopControl1, !digitalRead(loopControl1));
      }
      btnPressed = 0;
    }
    if (btnPressed == loop2 && !loop2State){ 
      if(millis() - pressTime > debounce){
      // change loop
        digitalWrite(loopControl2, !digitalRead(loopControl2));
      }
      btnPressed = 0;
    }
    if (btnPressed == loop3 && !loop3State){ 
      if(millis() - pressTime > debounce){
      // change loop
        digitalWrite(loopControl3, !digitalRead(loopControl3));
      }
      btnPressed = 0;
    }
    if (btnPressed == loop4 && !loop4State){ 
      if(millis() - pressTime > debounce){
      // change loop
        digitalWrite(loopControl4, !digitalRead(loopControl4));
      }
      btnPressed = 0;
    }
  }
}



