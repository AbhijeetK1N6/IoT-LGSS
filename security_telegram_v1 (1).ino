//message can be A or a, B or b,C or c
//A = card acess granted
//a = card acess denied
//B = finger print matched
//b = finger not matched
//C = otp verified.
//c = otp fail
//if alert = 0 then message A ,a,B,bC or c will follow
//if alert = 1 then send alert message
//http://www.arduinoproject.in/telegram/security.php?message=A&alert=0
#include <Adafruit_Fingerprint.h>
#include <HardwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <Keypad.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include <ESP32Servo.h> 

Servo myservo;  // create servo object to control a servo

int servoPin = 13;      

String host = "http://www.arduinoproject.in/telegram/security.php?message=" ;
String response = "";
long rnd = 0;
String selectedeUser = "";
String inputPassword = "";


HTTPClient http;

#define ROW_NUM     4 // four rows
#define COLUMN_NUM  4 // four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte pin_rows[ROW_NUM]      = {23,19, 18, 4}; 
byte pin_column[COLUMN_NUM] = {0, 2, 15};   

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );


// Set the LCD address to 0x27 for a 16 chars and 2 line display
//each user will record 3 finger's
//in reconize mode each user have will place same finger's in same order

//A = wrong rfid card used
//a = rfid card mathced

//B = finger order mismatch;
//b = all finger matched

//C = wrong otp entered
//c = otp accepted

LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "VIRUSR";
const char* password =  "12345678";

char user[12] = "35789";
char userP[12];
int cnt = 0; 
int colCnt = 0;

#define RXD2 16
#define TXD2 17

#define RXD1 9
#define TXD1 10

#define m1a 27
#define m1b 26

#define m2a 25
#define m2b 33

#define laser 32



#define user1 36
#define user2 39
#define user3 34
#define modeSw 35
#define laserInput 36


HardwareSerial rfid(1); 
HardwareSerial myFinger(2); 
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&myFinger);
 
char id[13] = "2700228CF079";
char cardId[13];
char serdata;
bool cardOk = false;
bool learnMode = false;
int fingerCounter = 0;
int userNo = 0;
int fingerNumber = 0;
bool fingerStored = false;
bool level2Ok = false;
bool level3Ok = false;
bool breakFinger = false;
bool laserBreak = false;
int attempt = 0;
char key ;
long userOtp =0;
int rowCnt = 0;
/*
 * get user number
 */

 int getUser(){
  bool ok = false;
  do{
    if (digitalRead(user1) == false){
      ok  = true;
      return 1;
    }
    if (digitalRead(user2) == false){
      ok = true;
      return 4;
    }
    if (digitalRead(user3) == false){
      ok = false;
      return 7;
    }
    
    
  }while (ok == false);
 }
/*
 * read rfid card
 */

void readRfidCard(){
  //receive card id
  for (int i = 0; i < 12; i++){
    serdata = rfid.read();
    cardId[i] = serdata;
    delay(1);
  }
}

/*
 * match rfid card
 */

bool matchCard(){
  
  byte c = 0;
  for (int i = 0; i< 12; i++){
    if (cardId[i] == id[i]){
      c++;
    }
  }
  
  if (c == 12){
    return true;
  }else{
    return false;
  }

}

void openDoor1(){
  digitalWrite(m1a,HIGH);
  delay(800);
  digitalWrite(m1a,LOW);
}

void closeDoor1(){
  digitalWrite(m1b,HIGH);
  delay(800);
  digitalWrite(m1b,LOW);
}

void openDoor2(){
  digitalWrite(m2a,HIGH);
  delay(800);
  digitalWrite(m2a,LOW);
}

void closeDoor2(){
  digitalWrite(m2b,HIGH);
  delay(800);
  digitalWrite(m2b,LOW);
}

bool matchPassword(){
  int m = 0;
  //itoa(rnd, user, 6);//convert int to char array
  String x = String(rnd);
  Serial.println(x);
  x.toCharArray(user,10);
  
  for (int i =0; i<6; i++){
    Serial.print(userP[i]);
    Serial.print("\t");
    Serial.println(user[i]);
  }
  for (int i = 0; i < 5; i++){
    if (user[i] == userP[i]){
      m++;
      Serial.println("increase");
    }else{
      Serial.println("no increase");
    }
  }
  Serial.print("value:");
  Serial.println(m);
  if (m == 5){
    return true;
  }else{
    return false;
  }
}


//void IRAM_ATTR laserInterrupt(){
//  Serial.println("interrupt found");
//  laserBreak = true;
//}
  
void setup() {
delay(500);  
lcd.begin();
lcd.backlight();
lcd.clear();
lcd.home();
lcd.print("  IOT Security");
lcd.setCursor(0,1);
lcd.print("    System");
delay(2000);
pinMode(laserInput,INPUT);
pinMode(user1,INPUT);
pinMode(user2,INPUT);
pinMode(user3,INPUT);
pinMode(modeSw,INPUT);
pinMode(m1a,OUTPUT);
pinMode(m1b,OUTPUT);
pinMode(m2a,OUTPUT);
pinMode(m2b,OUTPUT);
pinMode(laser,OUTPUT);
digitalWrite(m1a,LOW);
digitalWrite(m1b,LOW);
digitalWrite(m2a,LOW);
digitalWrite(m2b,LOW);
digitalWrite(laser,HIGH);
//attachInterrupt(laserInput, laserInterrupt, RISING);
myservo.setPeriodHertz(50);// Standard 50hz servo
myservo.attach(servoPin, 500, 2400); 
myservo.write(0);
Serial.begin(9600);
while (!Serial);  
delay(100);
Serial.println("IoT Security System V1");
rfid.begin(9600, SERIAL_8N1, 14, 12);
while(!rfid);
delay(200);
lcd.clear();
lcd.home();
lcd.print("Init Fing Sensor");
myFinger.begin(57600);
delay(200);
while(!myFinger);
lcd.setCursor(0,1);
finger.begin(57600);
delay(5);
if (finger.verifyPassword()) {
  Serial.println("Found fingerprint sensor!");
  lcd.print("Sensor Found");
  
} 
else {
  Serial.println("Did not find fingerprint sensor :(");
  lcd.print("Sensor Not Found");
  while (1) { delay(1); }
}
delay(2000);
closeDoor1();
closeDoor2();

//digitalWrite(laser,HIGH);

if (digitalRead(modeSw) == true){
learnMode = false;  
lcd.clear();
lcd.home();
lcd.print("Connect To WiFi");
WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println('.');
  }
  Serial.println("connecting wifi");
  lcd.setCursor(0,1);
  lcd.print("Wifi Connected");
  delay(2000);
}else{
  learnMode = true;
}

Serial.print("laser status:");
Serial.println(digitalRead(laser));
}

void loop() {

  if (learnMode == false){
  //clear serial port buffer
  do{
  do{
    delay(10);
    serdata = rfid.read();
    }while(rfid.available()== true);
  
  lcd.clear();
  lcd.home();
  lcd.print("Swap Card");
  
  

  do{
    delay(500);
    Serial.println("Waiting For Card");
    if (digitalRead(laser) == false){
    laserBreak = true;
    }
    if (laserBreak == true){
      break;
    }
  }while(rfid.available() == false);

  if (laserBreak == false){
  lcd.clear();
  lcd.home();
  lcd.print("Card Swaped");
  
  readRfidCard();
  cardOk = false;
  cardOk = matchCard();
  
  
  for (int i = 0 ; i < 12; i++){
    Serial.print(cardId[i]);
  }
  Serial.println();

  lcd.setCursor(0,1);
  if (cardOk == false){
    lcd.print("Acess Denied");
    
    delay(2000);
    //send telegram messaage
    //write_to_server(String d, String alert,String otp, String user) {
    write_to_server("a","0","0","0");
    //
  }else{
    lcd.print("Acess Granted");
    //send telegram message
    write_to_server("A","0","0","0");
  }
  }else{
    //laser break found
    lcd.clear();
    lcd.home();
    lcd.print("Laser Break Found");
    myservo.write(90);
    
    while(1){
    write_to_server("D","0","0","0");
    delay(10000);  
    }
  }
  
  
  }while(cardOk == false);
  delay(2000);
  lcd.clear();
  lcd.home();
  lcd.print("Level-1 Passed");
  delay(2000);
  level2Ok = false;

//open first door here
  openDoor1();

  
  breakFinger = false;


  do{
  lcd.clear();
  lcd.home();
  lcd.print("Use Finger Sensor");
  attempt = 0;
  
  do{
    attempt++;
    userNo = getFingerprintIDez();
    delay(200);
    if (attempt > 50){
      userNo = 10;
      break;
    }
    if (digitalRead(laser) == false){
    laserBreak = true;
    }
    if (laserBreak == true){
      break;
    }
  }while(userNo <= 0);

//
  if (laserBreak == true){
    lcd.clear();
    lcd.home();
    lcd.print("Laser Break Found");
    myservo.write(90);
    closeDoor1();
    while(1){
    write_to_server("D","0","0","0");
    delay(10000);  
    }
  }

  Serial.print("User Id:");
  Serial.println(userNo);

  switch(userNo){
    case 1:
    lcd.setCursor(0,1);
    lcd.print("1st Fing Matched");
    delay(2000);
    attempt = 0;
    do{
      attempt++;
    userNo = getFingerprintIDez();
    delay(200);
    if (attempt > 10){
      breakFinger = true;
      break;
      
    }
    }while(userNo != 2);

    if (userNo == 2){
      lcd.setCursor(0,1);
      lcd.print("2nd Fing Matched");
      delay(2000);
      attempt = 0;
      do{
        attempt++;
        userNo = getFingerprintIDez();
        delay(200);
        if (attempt > 10){
          breakFinger = true;
          break;
        }
      }while(userNo != 3);

      if (userNo == 3 ){
        lcd.clear();
        lcd.home();
        lcd.print("All fng Matched");
        delay(2000);
        level2Ok = true;  
        //send msg to telegram
        //format B
        rnd = random(10000,99999);
        write_to_server("B","0",String(rnd),"1");
      }else{
      lcd.clear();
      lcd.home();
      lcd.print("Fing Order Wrong");
      delay(2000);
      //send msg to telegram
      //format B
      //write_to_server(String d, String alert,String otp, String user) {
      write_to_server("b","0","0","0");
      breakFinger = true;
      }
      
    }else{
      lcd.clear();
      lcd.home();
      lcd.print("Fing Order Wrong");
      delay(2000);
      //send msg to telegram
      //format B
      write_to_server("b","0","0","0");
      breakFinger = true;
    }
    
  
    break;

    case 4:
    lcd.setCursor(0,1);
    lcd.print("1st Fing Matched");
    delay(2000);
    attempt = 0;
    do{
      attempt++;
    userNo = getFingerprintIDez();
    delay(200);
    if (attempt > 10){
      breakFinger = true;
      break;
    }
    }while(userNo != 5);

    if (userNo == 5){
      lcd.setCursor(0,1);
      lcd.print("2nd Fing Matched");
      delay(2000);
      attempt = 0;
      do{
        attempt ++;
        userNo = getFingerprintIDez();
        delay(200);
        if (attempt > 10){
          breakFinger = true;
          break;
        }
      }while(userNo != 6);

      if (userNo == 6 ){
        lcd.clear();
        lcd.home();
        lcd.print("All fng Matched");
        delay(2000);
        level2Ok = true;  
        //send msg to telegram
        //format b
        rnd = random(10000,99999);
        write_to_server("B","0",String(rnd),"2");
      }else{
      lcd.clear();
      lcd.home();
      lcd.print("Fing Order Wrong");
      delay(2000);
      //send msg to telegram
      //format B
      write_to_server("b","0","0","0");
      breakFinger = true;
      
      }
      
    }else{
      lcd.clear();
      lcd.home();
      lcd.print("Fing Order Wrong");
      delay(2000);
      //send msg to telegram
      //format B
      write_to_server("b","0","0","0");
      breakFinger = true;
    }
    
  
    
    break;

    case 7:
    lcd.setCursor(0,1);
    lcd.print("1st Fing Matched");
    delay(2000);
    attempt = 0;
    do{
      attempt++;
    userNo = getFingerprintIDez();
    delay(200);
    if (attempt > 10){
      breakFinger = true;
      break;
    }
    }while(userNo != 8);

    if (userNo == 8){
      lcd.setCursor(0,1);
      lcd.print("2nd Fing Matched");
      delay(2000);
      attempt = 0;
      do{
        attempt++;
        userNo = getFingerprintIDez();
        delay(200);
        if (attempt > 10){
          breakFinger = true;
          break;
        }
      }while(userNo != 9);

      if (userNo == 9 ){
        lcd.clear();
        lcd.home();
        lcd.print("All fng Matched");
        delay(2000);
        level2Ok = true;  
        //send msg to telegram
        //format b
        rnd = random(10000,99999);
        Serial.print("otp:");
        Serial.println(rnd);
        write_to_server("B","0",String(rnd),"3");
      }else{
      lcd.clear();
      lcd.home();
      lcd.print("Fing Order Wrong");
      delay(2000);
      //send msg to telegram
      //format B
      write_to_server("b","0","0","0");
      breakFinger = true;
      }
      
    }else{
      lcd.clear();
      lcd.home();
      lcd.print("Fing Order Wrong");
      delay(2000);
      closeDoor1();
      //send msg to telegram
      //format B
      write_to_server("b","0","0","0");
      breakFinger = true;
    }
    
  
    
    break;

    case 10:
    breakFinger = true;
    closeDoor1();
    lcd.clear();
    lcd.print("Wrong Finger or");
    lcd.setCursor(0,1);
    lcd.print("No Finger Detect");
    delay(2000);
    break;
    

    
  }

  if (breakFinger){
    break;
  }
  }while(level2Ok == false);

  if (level2Ok == true){
  
  //rnd = 35789;
  
  
  level3Ok = false;
  lcd.clear();
  lcd.home();
  lcd.print("Enter OTP");
  inputPassword= "";
  cnt = 0;
  rowCnt = 0;
  lcd.setCursor(rowCnt,1);
  laserBreak = false;
  
  do{
    key = keypad.getKey();
    if (key){
      Serial.print("Button:");
      Serial.println(key);
      userP[cnt] = key;
      cnt++;
      lcd.print("*");
      rowCnt++;  
      
    }
    
    if (key == '*' ){
      inputPassword = "";
      Serial.println("password cleared");
      lcd.setCursor(0,1);
      lcd.print("                ");
      rowCnt = 0;
      lcd.setCursor(rowCnt,1);
      
    }
    
    if (key == '#'){
      bool passwordOk = matchPassword();
      if (passwordOk == true){
        level3Ok = true;
      } 
      break;
      }

//      if (laserBreak == true){
//        break;
 //     }
    
  delay(300);
  }while(level3Ok == false);
/*
  if (laserBreak == true){
    lcd.clear();
    lcd.home();
    lcd.print("Laser Break Found");
    myservo.write(90);
    closeDoor1();
    
    while(1){
    write_to_server("D","0","0","0");
    delay(10000);  
    }
  }
*/

  if (level3Ok){
  lcd.setCursor(0,1);
  lcd.print("Password Matched");
  write_to_server("C","0","0","0");
  lcd.setCursor(0,1);
  lcd.print("Level-3 Passed  ");
  delay(2000);
  
  }else{
   closeDoor1();
   lcd.setCursor(0,1);
   lcd.print("Wrong password  "); 
   write_to_server("c","0","0","0");
    
  }
  delay(2000);

  lcd.clear();
  lcd.home();
  lcd.print("Deactivate Laser");
  delay(2000);
  digitalWrite(laser,LOW);
  delay(2000);
  lcd.setCursor(0,1);
  lcd.print("Laser Deactivate");
  openDoor2();
  
  
  
  
  

  
  
    
  }//if level-2 ok then
delay(2500);

  }else{//learn finger here
    lcd.clear();
    lcd.home();
    lcd.print("Enroll Mode");
    delay(2000);
    lcd.clear();
    lcd.home();
    lcd.print("Put finger");
    lcd.setCursor(0,1);
    lcd.print("Select User No.");
    userNo = getUser();
    delay(100);
    lcd.clear();
    lcd.home();
    lcd.print("Selected user:");
    lcd.print(userNo);
    delay(2000);
    fingerStored = false;
    switch (userNo){
      case 1:
        do{
        lcd.setCursor(0,1);
        lcd.print("Put finger");
        while (!getFingerprintEnroll(userNo));
        delay(1000);
        lcd.clear();
        if (fingerStored){
        userNo++;  
        fingerStored = false;
        }
        Serial.print("User:");
        Serial.println(userNo);
        }while(userNo < 4);
        delay(2000);
        lcd.clear();
        lcd.home();
        lcd.print("User-1 finger's");
        lcd.print("     Stored     ");
        delay(2000);

      break;

      case 4:
        do{
        lcd.setCursor(0,1);
        lcd.print("Put finger");
        while (!getFingerprintEnroll(userNo));
        delay(1000);
        lcd.clear();
        if (fingerStored){
        userNo++;  
        fingerStored = false;
        }
        Serial.print("User:");
        Serial.println(userNo);
        }while(userNo < 7);
        delay(2000);
        lcd.clear();
        lcd.home();
        lcd.print("User-2 finger's");
        lcd.print("     Stored     ");
        delay(2000);

      break;

      case 7:
        do{
        lcd.setCursor(0,1);
        lcd.print("Put finger");
        while (!getFingerprintEnroll(userNo));
        delay(1000);
        lcd.clear();
        if (fingerStored){
        userNo++;  
        fingerStored = false;
        }
        Serial.print("User:");
        Serial.println(userNo);
        }while(userNo < 10);
        delay(2000);
        lcd.clear();
        lcd.home();
        lcd.print("User-3 finger's");
        lcd.print("     Stored     ");
        delay(2000);

      break;
      
    }
    
    
    
  }
 
}

/************************************************************************************
// data to server
 **********************************************************************************/
void write_to_server(String d, String alert,String otp, String user) {
   
   //Serial.println("Posting water data to server");
   // Send an HTTP POST request depending on timerDelay
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;

      String serverPath = host + d +"&alert=" + alert + "&otp=" + otp + "&user=" + user ;
      Serial.println(serverPath);
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
        if (payload == "status:OK"){
          
        }

      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }else{
      Serial.println("Wifi Disconnected");
    }
}