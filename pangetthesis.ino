#include <LiquidCrystal.h>
#include <Servo.h>
Servo myservo1;
Servo myservo2;

int servo_count1 = 0;
int servo_ctr1 = 1;
int servo_seconds1 = 5;
int servo1 = 0;

int servo_count2 = 0;
int servo_ctr2 = 1;
int servo_seconds2 = 5;
int servo2 = 0;

int cnt1 = 0;
int cnt2 = 3;


int cntr1 = 0;
int cntr2 = 0;

unsigned long previousMillis1, previousMillis2;
const long interval3 = 1000;

class FlourMachine : public LiquidCrystal {
private:
  uint8_t _motor_pin;
  uint8_t minTens, minOnes, secTens, secOnes;
  String buffer;
  unsigned long previousMillis;
  const long interval1 = 1000;
  const long interval2 = 200;
  boolean _blink;
  uint8_t _cursor;
  
public:
  FlourMachine(uint8_t motor_pin, uint8_t rs, uint8_t ena, 
    uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3) 
    : LiquidCrystal(rs, ena, d0, d1, d2, d3)
  {
    _motor_pin = motor_pin;
    minTens = 0;
    minOnes = 0;
    secTens = 0;
    secOnes = 0;
    _cursor = 0;
  };
  void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS)
  {
    pinMode(_motor_pin, OUTPUT);
    LiquidCrystal::begin(cols, rows, charsize);
    buffer.reserve(cols*rows);
    previousMillis = millis();
    _blink = false;
  };
  void SetState(boolean Run)
  {
    unsigned long currentMillis = millis();
    if (Run)
    {
      if (currentMillis - previousMillis >= interval1) 
      {
        previousMillis = currentMillis;
        if(cntr1 == 1){
          send_time1();
        }
        digitalWrite(_motor_pin, HIGH);
        if (GetTotalSeconds()>0)
        {
          if (secOnes>0)
          {
            secOnes--;
          }
          else
          {
            secOnes = 9;
            if (secTens>0)
            {
              secTens--;  
            }
            else
            {
              secTens = 5;
              if (minOnes>0)
              {
                minOnes--;
              }
              else
              {
                minOnes = 9;
                minTens--;
              }
            }
          }
          Refresh(false);
        }
        else
        {
          digitalWrite(_motor_pin, LOW);
          LiquidCrystal::clear();
          _blink = !_blink;
          if (_blink)
          {
            LiquidCrystal::setCursor(0,1);
            LiquidCrystal::print("Done!");
          }
        }
      }
    }
    else
    {
      digitalWrite(_motor_pin, LOW);
      if (currentMillis - previousMillis >= interval2) 
      {
        previousMillis = currentMillis;
        _blink = !_blink;
        Refresh(true);
      }
    }
  };
  uint16_t GetTotalSeconds(void)
  {
     uint16_t TotalSeconds = minOnes + 10*minTens; 
     TotalSeconds *= 60;
     TotalSeconds += secOnes + 10*secTens; 
     return TotalSeconds;
  }
  void Refresh(boolean cursorON)
  {
    if (cursorON && _blink)
    {
      switch (_cursor) {
      default:
        _cursor = 0;
      case 0:
        buffer = String('_')+String(minOnes)+String(':')+String(secTens)+String(secOnes);
        break;
      case 1:
        buffer = String(minTens)+String('_')+String(':')+String(secTens)+String(secOnes);
        break;
      case 2:
        buffer = String(minTens)+String(minOnes)+String(':')+String('_')+String(secOnes);
        break;
      case 3:
        buffer = String(minTens)+String(minOnes)+String(':')+String(secTens)+String('_');
        break;
      }
    }
    else
    {
      buffer = String(minTens)+String(minOnes)+String(':')+String(secTens)+String(secOnes);
    }
    LiquidCrystal::clear();
    LiquidCrystal::print(buffer);
  };
  void send_time1(){
    String time1 = String(minTens) + String(minOnes) + ":" + String(secTens) + String(secOnes);
    Serial.print(time1);
  }
  void Reset(){
    minTens = 0;
    minOnes = 0;
    secTens = 0;
    secOnes = 0;
    _cursor = 0;
    Refresh(true);
  };
  void Left(void)
  {
    if (_cursor>0)
      _cursor--;
    else
      _cursor=3;
  };
  void Right(void)
  {
    if (_cursor<3)
      _cursor++;
    else
      _cursor=0;
  };
  void Up(void)
  {
    switch (_cursor) {
    case 0:
      if (minTens<9)
        minTens++;
      else
        minTens=0;
      break;
    case 1:
      if (minOnes<9)
        minOnes++;
      else
        minOnes=0;
      break;
    case 2:
      if (secTens<5)
        secTens++;
      else
        secTens=0;
      break;
    case 3:
      if (secOnes<9)
        secOnes++;
      else
        secOnes=0;
      break;
    }
  };
  void Down(void)
  {
    switch (_cursor) {
    case 0:
      if (minTens>0)
        minTens--;
      else
        minTens=9;
      break;
    case 1:
      if (minOnes>0)
        minOnes--;
      else
        minOnes=9;
      break;
    case 2:
      if (secTens>0)
        secTens--;
      else
        secTens=5;
      break;
    case 3:
      if (secOnes>0)
        secOnes--;
      else
        secOnes=9;
      break;
    }
  };
};

const int latchPin = 8;
const int dataPin = 9;
const int clockPin = 7;
const int motor1 = 4;
const int motor2 = 3;
String x;

//shift registers
int data1=12;
int clk1=11;
int latch1=10;

int led_bit[] = {1,2,4,8,16,32};
int led_val1 = 0;
int led_val2 = 0;
int led_val_total = 0;

//multiplexers
byte switchVar = 72; //01001000
byte switchVar2 = 159; //10011111

FlourMachine machine1(motor1, 2, A5, A3, A2, A1, A0);
FlourMachine machine2(motor2, 2, A4, A3, A2, A1, A0);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  machine1.begin(16, 2);
  machine2.begin(16, 2);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, INPUT);

  pinMode(data1, OUTPUT);
  pinMode(clk1, OUTPUT);
  pinMode(latch1, OUTPUT);

  myservo1.attach(5);
  myservo2.attach(6);
  
  //initialization of servo motor
  myservo1.write(0);
  myservo2.write(0);
  delay(2000);
  
  machine1.Refresh(false); //Update the display of machine1
  machine2.Refresh(false); //Update the display of machine2

  digitalWrite(latch1,LOW);
  shiftOut(data1,clk1,MSBFIRST,64);
  digitalWrite(latch1,HIGH);
}

void loop() {

  //Servo Control
  unsigned long currentMillis1 = millis();
  unsigned long currentMillis2 = millis();
  
  //1st Servo Motor  
  if (currentMillis1 - previousMillis2 >= interval3){
    if(servo1 == 1){
      if(servo_count1 < servo_seconds1){
        servo_count1++; 
        //Serial.println("servo_count1: " + String(servo_count1));
      }
    previousMillis1 = currentMillis1;
    }
  }
  
  //2nd Servo Motor
  if (currentMillis2 - previousMillis2 >= interval3){
    if(servo2 == 1){
      if(servo_count2 < servo_seconds2){
        servo_count2++; 
        //Serial.println("servo_count2: " + String(servo_count2));
      }
     }
    previousMillis2 = currentMillis2;
  }

  //Multiplexer Codes -------------------------------
  
  digitalWrite(latchPin,1);
  //set it to 1 to collect parallel data, wait
  delayMicroseconds(20);
  //set it to 0 to transmit data serially  
  digitalWrite(latchPin,0);
  
  byte old_switchVar = ~switchVar;
  byte old_switchVar2 = ~switchVar2;
  
  switchVar = shiftIn(dataPin, clockPin);
  old_switchVar &= switchVar; // detect the rising edges of the buttons.
    
  switchVar2 = shiftIn(dataPin, clockPin);
  old_switchVar2 &= switchVar2; // detect the rising edges of the buttons.

  //Bluetooth
  if(Serial.available()){
    x = Serial.readString();
  }

  if(old_switchVar2&4  || x.equalsIgnoreCase("valve1")){
    led_control(4);
  }

  if(old_switchVar2&8  || x.equalsIgnoreCase("valve2")){
    led_control(8);
  }

  if(old_switchVar2&16 || x.equalsIgnoreCase("cancel1")){
    machine1.Reset();
  }

  if(old_switchVar2&32 || x.equalsIgnoreCase("cancel2")){
    machine2.Reset();
  }
  //First Machine (x, y, a, b, c, d)
  //start button for ready timer M1
  if(old_switchVar2&1 || x.equalsIgnoreCase("start1"))
  {
    cntr1+=1;
  }
  
  if(x.equalsIgnoreCase("stop1")){
    cntr1 = 0;
  }
  
      if(cntr1 == 1)
      {
        if(cnt1 != 0){
          servo1 = 1;
          servo_control1();
          //Serial.println("servo pour flour1!");
        }
        //Serial.println("Servo_ctr1: " + String(servo_ctr1));
        //Serial.println("Servo_count1: " + String(servo_count1));
        machine1.SetState(true);
        x = "";
      }
      else if (cntr1 != 1)
      {
        cntr1 = 0;
        servo1 = 0;
        machine1.SetState(false);
        servo_count1 = 0;
        myservo1.write(0);
        
        if (old_switchVar&1 || x.equalsIgnoreCase("left1"))
        {
          machine1.Left(); 
        }
        if (old_switchVar&2 || x.equalsIgnoreCase("right1"))
        {
          machine1.Right(); 
        }
        if (old_switchVar&4 || x.equalsIgnoreCase("up1"))
        {
          machine1.Up(); 
        }
        if (old_switchVar&8 || x.equalsIgnoreCase("down1"))
        {
          machine1.Down();  
        }
      }

  //Second Machine (z, w, e, f, g, h)
  //start button for ready timer M2

  if(old_switchVar2&2)
  {
    cntr2+=1;
    delay(1000);
  }
    if(cntr2 == 1 || x.equalsIgnoreCase("start2"))
    {
      if(cnt2 != 3){
        servo2 = 1;
        servo_control2();
        //Serial.println("servo pour flour2!");
      }
      //Serial.println("Servo_ctr2: " + String(servo_ctr2));
      //Serial.println("Servo_count2: " + String(servo_count2));
      machine2.SetState(true);
      x = "";
    }
    else if(cntr2 !=1 || x.equalsIgnoreCase("stop2"))
    {
      cntr2 = 0;
      servo2 = 0;
      machine2.SetState(false);
      servo_count2 = 0;
      myservo2.write(0);
      
      if (old_switchVar&0x10 || x.equalsIgnoreCase("left2"))
      {
        machine2.Left();  
      }
      if (old_switchVar&0x20 || x.equalsIgnoreCase("right2"))
      {
        machine2.Right();  
      }
      if (old_switchVar&0x40 || x.equalsIgnoreCase("up2"))
      {
        machine2.Up();  
      }
      if (old_switchVar&0x80 || x.equalsIgnoreCase("down2"))
      {
        machine2.Down();  
      }
    }
    x = "";
}

//LED Controls
void led_control(byte led_switch){
  if(led_switch == 4){
    delay(500);
    led_val1 += led_bit[cnt1];
    cnt1++;
      if(cnt1 == 4){
        cnt1 = 0;
        led_val1 = 0;
      }
  }
  if(led_switch == 8){
    delay(500);
    led_val2 += led_bit[cnt2];
    cnt2++;
      if(cnt2 == 7){
        cnt2 = 3;
        led_val2 = 0;
      }
  }
  //Serial.println("led val1: " + String(led_val1));
  //Serial.println("led val2: " + String(led_val2));
  led_val_total = led_val1 + led_val2 + 64;
  
  digitalWrite(latch1,LOW);
  shiftOut(data1,clk1,MSBFIRST,led_val_total);
  digitalWrite(latch1,HIGH);
}

//Servo Motor
void servo_control1(){
  if(servo_count1 == 5){
    if(servo_ctr1 == 1){
      //Serial.println("position: 180");
      //delay(1000);
      myservo1.write(140);
      servo_ctr1 = 0;
      servo_count1 = 0;
    } else if(servo_ctr1 == 0){
      servo_ctr1 = 1;
      servo_count1 = 0;
      //Serial.println("position: 0");
      //delay(1000);
      myservo1.write(0);
      cnt1--;
    
      led_val1 = 0;
      led_val_total = led_val1 + led_val2 + 64;
  
      digitalWrite(latch1,LOW);
      shiftOut(data1,clk1,MSBFIRST,led_val_total);
      digitalWrite(latch1,HIGH);
    }
  }
}

void servo_control2(){
  if(servo_count2 == 5){
    if(servo_ctr2 == 1){
      //Serial.println("position: 180");
      //delay(1000);
      myservo2.write(140);
      servo_ctr2 = 0;
      servo_count2 = 0;
    } else if(servo_ctr2 == 0){
      servo_ctr2 = 1;
      servo_count2 = 0;
      //Serial.println("position: 0");
      //delay(1000);
      myservo2.write(0);
      cnt2--;
      
      led_val2 = 0;
      led_val_total = led_val1 + led_val2 + 64;
      
      digitalWrite(latch1,LOW);
      shiftOut(data1,clk1,MSBFIRST,led_val_total);
      digitalWrite(latch1,HIGH);
    }
  }
}


//to know what button you click.
byte shiftIn(int myDataPin, int myClockPin) { 
  int i;
  int temp = 0;
  int pinState;
  byte myDataIn = 0;

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, INPUT);

//we will be holding the clock pin high 8 times (0,..,7) at the
//end of each time through the for loop

//at the begining of each loop when we set the clock low, it will
//be doing the necessary low to high drop to cause the shift
//register's DataPin to change state based on the value
//of the next bit in its serial information flow.
//The register transmits the information about the pins from pin 7 to pin 0
//so that is why our function counts down

  for (i=7; i>=0; i--)
  {
    digitalWrite(myClockPin, 0);
    delayMicroseconds(2);
    temp = digitalRead(myDataPin);
    if (temp) {
      pinState = 1;
      //set the bit to 0 no matter what
      myDataIn = myDataIn | (1 << i);
    }
    else {
      //turn it off -- only necessary for debuging
     //print statement since myDataIn starts as 0
      pinState = 0;
    }

    //Debuging print statements
    //Serial.print(pinState);
    //Serial.print("     ");
    //Serial.println (dataIn, BIN);

    digitalWrite(myClockPin, 1);

  }
  //debuging print statements whitespace
  //Serial.println();
  //Serial.println(myDataIn, BIN);
  return myDataIn;
}
