/*
tao lenh dieu khien hoat dong 16 bit voi cac bit dieu khien mot chuc nang

0:1 bit dieu khien tien (0) va lui (1)
1:2 bit dieu khien truc x
3:1 bit dieu khien trai (0) va phai (1)
5:2 bit dieu khien truc y 

6:1 bit dieu khien cat
7:1 bit dieu khien gian cat nang
8:1 bit dieu khien gian cat ha

9:1 bit dieu khien phun
10:1 bit bat / tat quat phan
11:1 bit dong /  nha phan

12:1 bit quay trai step
13:1 bit quay phai step
14:1 bit chinh cam nang len
15:1 bit chinh cam huong xuong

0 12 3 45 6 7 8 9 10  11  12  13  14  15
0 00 0 00 0 0 0 0 0   0   0   0   0   0
 
nrf24			mega
 miso		| 50
 sck		| 52
 mosi		| 51
 ce		  | 48
 csn		| 49

driver1			mega    (trai)
 lpwm		| 22
 rpwm		| 23
 pwm		| 2

driver2			mega    (phai)
 lpwm		| 24
 rpwm		| 25
 pwm 		| 3

l298			mega
 pwma		| 4         (nang ha cat)
 in1		| 26
 in2		| 27
 in3 		| 28        (dong mo nha phan)
 in4		| 29
 pwmb		| not connect

relay cat 	| 30
relay phun	| 31
relay quat	| 32

L298	 	mega    (step motor)
connect like this: https://lastminuteengineers.com/stepper-motor-l298n-arduino-tutorial/#google_vignette

servo		| 5   

esp   mega
D1  | 20
D2  | 21
*/


#include <Wire.h>
#include <Servo.h>
#include <Stepper.h>

const int stepsPerRevolution = 200;

Servo sercam;
Stepper myStepper(stepsPerRevolution, 8,9,10,11); 
#define tocdo0 0
#define tocdo1 85
#define tocdo2 170
#define tocdo3 255
int angle = 90;
int control[16] = {};

//relay
#define quat_phan 32
#define phun 31
#define cat 30

//nang ha cat
#define pwma 4
#define in1 26
#define in2 27
//dong mo nha phan
#define in3 28
#define in4 29

//1 trai 2 phai
#define brake1 12
#define brake2 13
#define pwm1 2
#define rpwm1 23
#define lpwm1 22

// ket noi voi driver a4988
#define dirpin 33
#define steppin 34

//servo
#define servo 5

void setup() {
  //----------set up address and check for connection
  Wire.begin(8);                /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  //----------done

  //----------servo setup 
  sercam.attach(servo);
  sercam.write(angle);

  // set up for pin
  pinMode(brake1, OUTPUT);
  pinMode(brake2, OUTPUT);
  for(int i = 22; i <= 34; i++){
    pinMode(i, OUTPUT);
  }
  for(int j = 2; j <= 4; j++){
    pinMode(j , OUTPUT);
  }
  // --------
}

void receiveEvent(int howMany) {
  while (0 < Wire.available()) {
    for(int i = 0; i < 16; i++){
      control[i] = Wire.read();
    }
  }
}


void tien(int tocdo){
  analogWrite(pwm1, tocdo);
  digitalWrite(lpwm1, 1);
  digitalWrite(rpwm1, 0);

  digitalWrite(brake1, 0);
  digitalWrite(brake2, 0);
}


void lui(int tocdo){
  analogWrite(pwm1, tocdo);
  digitalWrite(lpwm1, 0);
  digitalWrite(rpwm1, 1);

  digitalWrite(brake1, 0);
  digitalWrite(brake2, 0);
}


void dung(int tocdo){
  analogWrite(pwm1, 0);
  digitalWrite(lpwm1, 1);
  digitalWrite(rpwm1, 1);

  digitalWrite(brake1, 1);
  digitalWrite(brake2, 1);
}


void trai(int tocdo){
  analogWrite(pwm1, tocdo);
  digitalWrite(lpwm1, 1);
  digitalWrite(rpwm1, 1);

  digitalWrite(brake1, 1);
  digitalWrite(brake2, 0);
}


void phai(int tocdo){
  analogWrite(pwm1, tocdo);
  digitalWrite(lpwm1, 1);
  digitalWrite(rpwm1, 0);

  digitalWrite(brake1, 0);
  digitalWrite(brake2, 1);
}


void nang_cat(int tocdo = 175){
  analogWrite(pwma, tocdo);
  digitalWrite(in1, 1);
  digitalWrite(in2, 0);
}


void ha_cat(int tocdo = 175){
  analogWrite(pwma, tocdo);
  digitalWrite(in1, 0);
  digitalWrite(in2, 1);
}


void dung_ha(int tocdo = 0){
  analogWrite(pwma, 0);
  digitalWrite(in1, 1);
  digitalWrite(in2, 1);
}


void cat_co(){
  digitalWrite(cat, 1);
}

void dung_cat_co(){
  digitalWrite(cat, 0);
}


void phun_thuoc(){
  digitalWrite(phun, 1);
}

void dung_phun_thuoc(){
  digitalWrite(phun, 0);
}


void mo_phan(){
  digitalWrite(in3, 1);
  digitalWrite(in4, 0);
}

void giu_trang_thai_nha(){
  digitalWrite(in3, 1);
  digitalWrite(in4, 1);
}

void bat_quat(){
  digitalWrite(quat_phan, 1);
}

void tat_quat(){
  digitalWrite(quat_phan, 0);
}


//dieu khien stepmotor
void xoay_sang_trai(int tocdo = 1000){
  myStepper.setSpeed(tocdo);
  myStepper.step(-250);
  delay(500);
}

void xoay_sang_phai(int tocdo = 1000){
  myStepper.setSpeed(tocdo);
  myStepper.step(250);
  delay(500);
}

void dung_xoay(){
  myStepper.setSpeed(0);
  myStepper.step(0);
}

void cam_up(int step = 1){
  if(angle < 180) {
    angle += step;
  }
  sercam.write(angle);
}

void cam_down(int step = 1){
  if(angle > 0) {
    angle -= step;
  }
  sercam.write(angle);
}


void loop() {
  // doc du lieu dieu khien truc x
  if(control[0] == 0){
    if(control[1] == 1 && control[2] == 1){
      tien(tocdo3);
    }else if(control[1] == 1 && control[2] == 0){
      tien(tocdo2);
    }else if(control[1] == 0 && control[2] == 1){
      tien(tocdo1);
    }else if(control[1] == 0 && control[2] == 0){
      tien(tocdo0);
    }
  } else if(control[0] == 1){
    if(control[1] == 1 && control[2] == 1){
      lui(tocdo3);
    }else if(control[1] == 1 && control[2] == 0){
      lui(tocdo2);
    }else if(control[1] == 0 && control[2] == 1){
      lui(tocdo1);
    }else if(control[1] == 0 && control[2] == 0){
      lui(tocdo0);
    }
  } else {
    dung(tocdo0);
  }

  // doc du lieu dieu khien truc y
  if(control[3] == 0){
    if(control[4] == 1 && control[5] == 1){
      trai(tocdo3);
    }else if(control[4] == 1 && control[5] == 0){
      trai(tocdo2);
    }else if(control[4] == 0 && control[5] == 1){
      trai(tocdo1);
    }else if(control[4] == 0 && control[5] == 0){
      trai(tocdo0);
    }
  } else if(control[3] == 1){
    if(control[4] == 1 && control[5] == 1){
      phai(tocdo3);
    }else if(control[4] == 1 && control[5] == 0){
      phai(tocdo2);
    }else if(control[4] == 0 && control[5] == 1){
      phai(tocdo1);
    }else if(control[4] == 0 && control[5] == 0){
      phai(tocdo0);
    }
  } else {
    dung(tocdo0);
  }


  // cat co
  if(control[6] == 1){
    cat_co();
  }else {
    dung_cat_co();
  }


  //nang ha cat
  if(control[7] == 1 && control[8] == 0){
    nang_cat();
  } else if(control[7] == 0 && control[8] == 1){
    ha_cat();
  } else {
    dung_ha();
  }


  //phun thuoc
  if(control[9] == 1){
    phun_thuoc();
  }else {
    dung_phun_thuoc();
  }


  //quat phan
  if(control[10] == 1){
    bat_quat();
  } else {
    tat_quat();
  }


  // nha phan
  if(control[11] == 1){
    mo_phan();
  }else {
    giu_trang_thai_nha();
  }


  // quay camera
  if(control[12] == 1 && control[13] == 0){
    xoay_sang_trai();
  } else if(control[12] == 0 && control[13] == 1){
    xoay_sang_phai();
  }else{
    dung_xoay();
  }
  
  if(control[14] == 1 && control[15] == 0){
    cam_up();
  }else if(control[14] == 0 && control[15] == 1){
    cam_down();
  }else {
    sercam.write(angle);
  }  
}