#include <LiquidCrystal.h>
#include <Servo.h>

//Servo
Servo myservo1;
Servo myservo2;

int servo_count1 = 0;
int servo_ctr1 = 1;
int servo_seconds1 = 15;
int servo1 = 0;

int servo_count2 = 0;
int servo_ctr2 = 1;
int servo_seconds2 = 15;
int servo2 = 0;

// LED Array 
int cnt1 = 0;
int cnt2 = 3;

//Start Counter
int cntr1 = 0;
int cntr2 = 0;

//Intervals
unsigned long previousMillis_servo, previousMillis_time;
const long interval_servo = 1000;
const long interval_time = 900;

/*int onoff1 = 0;
int onoff2 = 0;*/

// Main Timer ------------------------------------------
class FlourMachine : public LiquidCrystal {
private:
  uint8_t _motor_pin;
  int onoff = 0;
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

        if(onoff == 0){           
          digitalWrite(_motor_pin, HIGH);
          onoff = 1;
        } else if(onoff == 1){
          digitalWrite(_motor_pin, LOW);
          onoff = 0;
        }

        /*if(cntr2 == 1){
          if(onoff2 == 0){           
            digitalWrite(_motor_pin, HIGH);
            onoff2 = 1;
          } else if(onoff2 == 1){
            digitalWrite(_motor_pin, LOW);
            onoff2 = 0;
          }
          Serial.println("shake2");
        }*/
        
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
  
  void send_time(){
    String _time = String(minTens) + String(minOnes) + ":" + String(secTens) + String(secOnes);
    Serial.print(_time);
  };

  // Reset Method ------------------------------------
  void Reset(){
    minTens = 0;
    minOnes = 0;
    secTens = 0;
    secOnes = 0;
    _cursor = 0;
    Refresh(true);
    
    if(cntr1 == 1){
      cntr1 = 0;
    } else if(cntr2 == 1){
      cntr2 = 0;
    }
    
  };

  // Control Buttons ----------------------------------
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
      if (minTens<2)
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
        minTens=2;
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

// Declaration of Variables -------------------------------

//multiplexers
const int latchPin = 8;
const int dataPin = 9;
const int clockPin = 7;
byte switchVar = 72; //01001000
byte switchVar2 = 159; //10011111

//Motors
const int motor1 = 4;
const int motor2 = 3;

//Bluetooth
String x;

//Shift Registers
int data1=12;
int clk1=11;
int latch1=10;

// LED 
int led_bit[] = {2,4,8,16,32,64};
int led_val1 = 0;
int led_val2 = 0;
int led_val_total = 0;

int mach_ctr = 0; // machine_control
int dis_mach = 0; // disabled_machine

// LCD
FlourMachine machine1(motor1, 2, A4, A3, A2, A1, A0);
FlourMachine machine2(motor2, 2, A5, A3, A2, A1, A0);

 // Initializations ----------------------------------------
 
void setup() {
  
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

  //initialization of Shift Register
  digitalWrite(latch1,LOW);
  shiftOut(data1,clk1,MSBFIRST,128);
  digitalWrite(latch1,HIGH);
}

// Main Function -------------------------------------------------

void loop() {

  //Servo Controls
  unsigned long currentMillis_servo = millis();
  unsigned long currentMillis_time = millis();

  if (currentMillis_servo - previousMillis_servo >= interval_servo){
    if(servo1 == 1){
      if(servo_count1 < servo_seconds1){
        if(servo_count1 == 1){
          myservo1.write(140);
        }
        servo_count1++;
      }
    } else if(servo2 == 1){
      if(servo_count2 < servo_seconds2){
        if(servo_count2 == 1){
          myservo1.write(140);
        }
        servo_count2++;
      }
    }
    previousMillis_servo = currentMillis_servo;
  }
  
  if(currentMillis_time - previousMillis_time >= interval_time){
    if(cntr1 == 1 && mach_ctr == 1 && dis_mach == 1){
      machine1.send_time();
    } else if(cntr2 == 1 && mach_ctr == 2 && dis_mach == 2){
      machine2.send_time();
    }
    previousMillis_time = currentMillis_time;
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

  //Bluetooth --------------------------------------
  if(Serial.available()){
    x = Serial.readString();
    //Serial.println(x);
  }

  if(x.equalsIgnoreCase("m1")){
    mach_ctr = 1;
    dis_mach = 1;
  } else if(x.equalsIgnoreCase("m2")){
    mach_ctr = 2;
    dis_mach = 2;
  }

  if(x.equalsIgnoreCase("r_m1")){
    mach_ctr = 0;
    dis_mach = 0;
  } else if(x.equalsIgnoreCase("r_m2")){
    mach_ctr = 0;
    dis_mach = 0;
  }

  //First Machine (x, y, a, b, c, d) --------------
  //start button for ready timer M1
  if(mach_ctr == 1){
    if(x.equalsIgnoreCase("valve1")){
      led_control(4);
    }
  
    if(x.equalsIgnoreCase("cancel1")){
      machine1.Reset();
    }
  
    if(x.equalsIgnoreCase("r_led1")){
      led_reset(1);    
    }
  
    if(x.equalsIgnoreCase("start1"))
    {
      cntr1+=1;
    }
    
    if(x.equalsIgnoreCase("stop1")){
      cntr1 = 0;
    }
    if(cntr1 == 1){
      if(cnt1 != 0){
        servo1 = 1;
        servo_control1();
      }
      machine1.SetState(true);
      x = "";
    } else if (cntr1 != 1){
      cntr1 = 0;
      servo1 = 0;
      machine1.SetState(false);
      servo_count1 = 0;
      myservo1.write(0);
      
      if (x.equalsIgnoreCase("left1"))
      {
        machine1.Left(); 
      }
      if (x.equalsIgnoreCase("right1"))
      {
        machine1.Right(); 
      }
      if (x.equalsIgnoreCase("up1"))
      {
        machine1.Up(); 
      }
      if (x.equalsIgnoreCase("down1"))
      {
        machine1.Down();  
      }
    }
  }
  
  if(dis_mach != 1){
    if(old_switchVar2&4){
      led_control(4);
    }
  
    if(old_switchVar2&16){
      machine2.Reset();
    }
  
    if(old_switchVar2&1){
      cntr1+=1;
    }
    
    if(cntr1 == 1){
      if(cnt1 != 0){
        servo1 = 1;
        servo_control1();
      }
      machine1.SetState(true);
      x = "";
    } else if (cntr1 != 1){
      cntr1 = 0;
      servo1 = 0;
      machine1.SetState(false);
      servo_count1 = 0;
      myservo1.write(0);
      
      if (old_switchVar&1){
        machine1.Left(); 
      }
      if (old_switchVar&2){
        machine1.Right(); 
      }
      if (old_switchVar&4){
        machine1.Up(); 
      }
      if (old_switchVar&8){
        machine1.Down();  
      }
    }
  }
  
  //Second Machine (z, w, e, f, g, h)---------------
  //start button for ready timer M2
  if(mach_ctr == 2){
    if(x.equalsIgnoreCase("valve2")){
      led_control(8);
    }
  
    if(x.equalsIgnoreCase("cancel2")){
      machine2.Reset();
    }
  
    if(x.equalsIgnoreCase("start2"))
    {
      cntr2+=1;
    }

    if(x.equalsIgnoreCase("r_led2")){
      led_reset(2);    
    }
    
    if(x.equalsIgnoreCase("stop2")){
      cntr2 = 0;
    }
    if(cntr2 == 1){
      if(cnt2 != 0){
        servo2 = 1;
        servo_control2();
      }
      machine2.SetState(true);
      x = "";
    } else if (cntr2 != 1){
      cntr2 = 0;
      servo2 = 0;
      machine2.SetState(false);
      servo_count2 = 0;
      myservo2.write(0);
      
      if (x.equalsIgnoreCase("left2")){
        machine2.Left(); 
      }
      if (x.equalsIgnoreCase("right2")){
        machine2.Right(); 
      }
      if (x.equalsIgnoreCase("up2")){
        machine2.Up(); 
      }
      if (x.equalsIgnoreCase("down2")){
        machine2.Down();
      }
    }
  }
  
  if(dis_mach != 2){
    if(old_switchVar2&8){
      led_control(8);
    }
  
    if(old_switchVar2&32){
      machine1.Reset();
    }
  
    if(old_switchVar2&32){
      machine2.Reset();
    }
    
    if(old_switchVar2&2){
      cntr2+=1;
    }
  
    if(cntr2 == 1) {
      if(cnt2 != 3){
        servo2 = 1;
        servo_control2();
      }
      machine2.SetState(true);
      x = "";
    } else if(cntr2 !=1){
      cntr2 = 0;
      servo2 = 0;
      machine2.SetState(false);
      servo_count2 = 0;
      myservo2.write(0);
      
      if (old_switchVar&0x10){
        machine2.Left();  
      }
      if (old_switchVar&0x20){
        machine2.Right();  
      }
      if (old_switchVar&0x40){
        machine2.Up();  
      }
      if (old_switchVar&0x80){
        machine2.Down();  
      }
    }
  }
  x = "";
}

//LED Controls ------------------------------------
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
  led_val_total = led_val1 + led_val2 + 128;
  
  digitalWrite(latch1,LOW);
  shiftOut(data1,clk1,MSBFIRST,led_val_total);
  digitalWrite(latch1,HIGH);
}

void led_reset(int m){
  if(m == 1){
    cnt1 = 0;
    led_val1 = 0;
    led_val_total = led_val1 + led_val2 + 128;
  } else if(m == 2){
    cnt2 = 3;
    led_val2 = 0;
    led_val_total = led_val1 + led_val2 + 128;
  }
  digitalWrite(latch1,LOW);
  shiftOut(data1,clk1,MSBFIRST,led_val_total);
  digitalWrite(latch1,HIGH);
}

//Servo Motor Method -----------------------------
void servo_control1(){
  if(servo_count1 == servo_seconds1){
    if(servo_ctr1 == 1){
      myservo1.write(0);
      servo_ctr1 = 0;
      servo_count1 = 0;
    } else if(servo_ctr1 == 0){
      servo_ctr1 = 1;
      servo_count1 = 0;
      myservo1.write(140);
      cnt1--;
    
      led_val1 = 0;
      led_val_total = led_val1 + led_val2 + 128;
  
      digitalWrite(latch1,LOW);
      shiftOut(data1,clk1,MSBFIRST,led_val_total);
      digitalWrite(latch1,HIGH);
    }
  }
}

void servo_control2(){
  if(servo_count2 == servo_seconds2){
    if(servo_ctr2 == 1){
      myservo2.write(0);
      servo_ctr2 = 0;
      servo_count2 = 0;
    } else if(servo_ctr2 == 0){
      servo_ctr2 = 1;
      servo_count2 = 0;
      myservo2.write(140);
      cnt2--;
      
      led_val2 = 0;
      led_val_total = led_val1 + led_val2 + 128;
      
      digitalWrite(latch1,LOW);
      shiftOut(data1,clk1,MSBFIRST,led_val_total);
      digitalWrite(latch1,HIGH);
    }
  }
}

//to know what button you click ----------------------------
byte shiftIn(int myDataPin, int myClockPin) { 
  int i;
  int temp = 0;
  int pinState;
  byte myDataIn = 0;

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, INPUT);

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
      pinState = 0;
    }

    digitalWrite(myClockPin, 1);
  }
  
  return myDataIn;
}
