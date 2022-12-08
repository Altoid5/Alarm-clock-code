#include <DS3231.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h> 
#include <Keypad.h>
 
DS3231 rtc(SDA, SCL);
LiquidCrystal_I2C lcd(0x27,16,2);
const byte ROWS = 4; 
const byte COLS = 4; 
char* password="4567";
int currentposition=0;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
 
byte rowPins[ROWS] = {7,8,9,10}; 
byte colPins[COLS] = {A3,A2,A1,A0}; 
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
 
 
String phNumber;
String storedHour;
String storedMinute;
int buzzer = 13;
void storeMinute(String);
void storeHour(String);
 
void setup() {
  pinMode(4, OUTPUT);
  lcd.init();                      
  // Print a message to the LCD.
  lcd.backlight();
  Serial.begin(9600);
  rtc.begin();
  lcd.begin(16, 2);
  lcd.clear();
 
 
   //rtc.setDOW(WEDNESDAY);    
   //rtc.setTime(12, 20, 0);     
   //rtc.setDate(11, 23, 2022);  
 
  //Getting alarm time
  storedHour = getHour();
  storedMinute = getMinute();
 
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
 
}
 
void loop() {
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm:");
  String storedAlarmTime = storedHour + ":" + storedMinute + ":00";
  lcd.print(storedAlarmTime);
  lcd.setCursor(0, 1);
  lcd.print("*:set alarm");
    
 
   
 char key = keypad.getKey();
  if (key) {
    Time();
  digitalWrite(buzzer, HIGH);
    delay(100);
    digitalWrite(buzzer, LOW);
    //Set alarm
    if (key == '*') {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("*Set Hour:");
      String enteredHour = getInputFromKeyBoard(2);
 
      if (enteredHour.length() == 2) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("*Set Minute:");
       
        String enteredMinute = getInputFromKeyBoard(2);
       
        if (enteredMinute.length() == 2) {
         
          storeHour(enteredHour);
          storeMinute(enteredMinute);
          storedHour = enteredHour;
          storedMinute = enteredMinute;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Alarm set to");
          lcd.setCursor(0, 1);
          String alarmTime = enteredHour + ":" + enteredMinute + ":00";
          lcd.print(alarmTime);
          delay(1000);
         
        }
      }
    } else {
      lcd.clear();
      lcd.print("Invalid Option!");
     
    }
 
  }
 
 
 
  String timeString = rtc.getTimeStr();
 
  String realHour = timeString.substring(0, 2);
  String realMinute = timeString.substring(3, 5);
 
  String currentTime = realHour+":"+realMinute+":00";
 
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time:"+currentTime);
 
  Serial.print("Time ");
  Serial.println(currentTime);  
   
 
  while(realHour.equals(storedHour) && realMinute.equals(storedMinute)) {
   
   soundAlarm();
   int l ;
   char key = keypad.getKey();
   if(key!=NO_KEY)
   {
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("PASSWORD:");
     lcd.setCursor(7,1);
     lcd.print(" ");
     lcd.setCursor(7,1);
     for(l=0;l<=currentposition;++l)
    {
     lcd.print("*");
    }
    if (key==password[currentposition])
    {
    ++currentposition;
    if(currentposition==4)
    {
   unlockDoor();
   currentposition=0;
   break;
    }
    }else{
      InvalidCode();
      currentposition=0;
      
   
   {
   
    }
      }
    }
  }
}
 
 
/*..........................................................................*/
String getInputFromKeyBoard(int n) {
 
  String num = "";
  String number;
  String confirmNumber;
  int i = 0;
 
  while (1) {
    char key = keypad.getKey();
    if (key) {
 
      if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9') {
        i++;
        if (i <= n) {
          num += key;
        }
 
        Serial.println(num);
       
        lcd.setCursor(14, 0);
        //lcd.clear();
        lcd.print(num);
       
        lcd.setCursor(0, 1);
        lcd.print("*Cancel #Confirm");
 
      }
 
      if (key == '#' && i == n) {
        break;
      }
 
      if (key == '*') {
        number = "";
        confirmNumber = "";
        num = "";
        return "";
      }
 
      if (( key == 'A' || key == 'B' || key == 'C' || key == 'D') && i < n) {
        lcd.clear();
        lcd.print("Invalid Number!");
        delay(2000);
        number = "";
        confirmNumber = "";
        num = "";
        return "";
 
      }
 
      if (i > n) {
        lcd.clear();
        lcd.print("Invalid Option!");
        delay(2000);
        number = "";
        confirmNumber = "";
        num = "";
        return "";
      }
 
      if (i == n && (key == 'A' || key == 'B' || key == 'C' || key == 'D')) {
        lcd.clear();
        lcd.print("Invalid Option!");
        delay(2000);
        number = "";
        confirmNumber = "";
        num = "";
        return "";
 
      }
 
    }
 
  }
  return num;
}
 
/*..........................................................................*/
 
void storeHour(String n) {
  for (int k = 0; k < 2; k++) {
    EEPROM.write(k, String(n[k]).toInt());
  }
 
}
 
/*..........................................................................*/
 
void storeMinute(String n) {
  for (int k = 0; k < 2; k++) {
    EEPROM.write(k+2, String(n[k]).toInt());
  }
 
}
 
/*..........................................................................*/
 
String getHour() {
  String newHour = "";
  for (int i = 0; i < 2; i++) {
    newHour += EEPROM.read(i);
    Serial.println(EEPROM.read(i));
  }
  return newHour;
}
 
/*---------------------------------------------------------------------------*/
 
String getMinute() {
  String newMinute = "";
  for (int i = 2; i <4; i++) {
    newMinute += EEPROM.read(i);
    Serial.println(EEPROM.read(i));
  }
  return newMinute;
 
}
 
/*---------------------------------------------------------------------------*/
 
void soundAlarm() {
tone(13,440);
tone(12,1440);
tone(11,1140);
  digitalWrite(4, HIGH);
  delay(150); // Wait for 300 millisecond(s)
  digitalWrite(4, LOW);
  delay(150); // Wait for 300 millisecond(s)
}
 
/*--------------------------------------------------------------------------*/
void unlockDoor() {
displayCodeEntrySreen();
noTone(buzzer);

}
/*--------------------------------------------------------------------------*/
void InvalidCode(){
delay(500);
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Wake up");
lcd.setCursor(0,1);
lcd.print("Time For School");

 
 
}
/*--------------------------------------------------------------------------*/
void displayCodeEntrySreen()
{lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alarm:");
  String storedAlarmTime = storedHour + ":" + storedMinute + ":00";
  lcd.print(storedAlarmTime);
    lcd.setCursor(0, 1);
  lcd.print("*:set alarm");
String timeString = rtc.getTimeStr();
 
  String realHour = timeString.substring(0, 2);
  String realMinute = timeString.substring(3, 5);
 
  String currentTime = realHour+":"+realMinute+":00";
 
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time:"+currentTime);
 
  Serial.print("Time ");
  Serial.println(currentTime);    
}
/*--------------------------------------------------------------------------*/
void Time()
{


}
