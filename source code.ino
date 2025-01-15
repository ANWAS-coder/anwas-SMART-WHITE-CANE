#include <Wire.h>
#include <MPU6050.h>

const int trigPin = 5;     
const int echoPin = 4;     

MPU6050 mpu;  //gyro                   

int VM1 = A1;  //ตัวสั่น

unsigned long previousMillis = 0;  //เก็บค่าเลยวินาทีจาก ultrasonic แล้วเปลี่ยนเป็นเมตร
const long interval = 1000;  //เว้นเพื่อให้ข้อมูลไม่รัวเกินไป

const int buzzerPin = 9;  //ตัวร้อง

int melody[] = {
  1000, 500, 
  1500, 500, 
  1000, 500, 
  1500, 500, 
  1000, 500, 
  1500, 500  
};

int noteDuration = 10000;  //ร้องไป 10 วิ
int pauseDuration = 10;  //หยุดไป 0.01 วิ

void setup() {
  pinMode(VM1, OUTPUT);
  Serial.begin(9600);      
  Wire.begin();  

  mpu.initialize();  //เปิดเครื่องไจโร

  if(mpu.testConnection()) {
    Serial.println("MPU6050 connected successfully");
  } else {
    Serial.println("MPU6050 connection failed");
    while(1); 
  }

  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);  //ตั้งว่า mpu คือแค่ gyro ไม่ต้องทำอย่างอื่น

  pinMode(trigPin, OUTPUT);    
  pinMode(echoPin, INPUT);     
  pinMode(buzzerPin, OUTPUT);  
  //ตั้ง ultrasonic
  vibrateMotorsTwice();  
}

void loop() {
  long duration, cm;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  cm = microsecondsToCentimeters(duration);

  int16_t ax, ay, az;  //อ่านค่าองศาจากไจโร
  int16_t gx, gy, gz;  //วัดเวลาหมุนเร็วแค่ไหน
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  //รับค่าเอาไปใช้

  long acceleration = sqrt(sq(ax) + sq(ay) + sq(az));  //คำนวณอัตราเร็ว
  long gyro = sqrt(sq(gx) + sq(gy) + sq(gz));  //คำนวณองศา

  
  float pitch = atan2(ay, az) * 180 / PI;  //คำนวณจาก radien เป็นองศา หน้าหลัง
  float roll = atan2(ax, az) * 180 / PI;  //คำนวณจาก radien เป็นองศา ซ้ายขวา

  Serial.print("ระยะทางจาก HC-SR04: ");
    Serial.print(cm);
    Serial.println(" เซนติเมตร");

  Serial.print("Pitch: "); Serial.print(pitch); Serial.print(" °\t");
  Serial.print("Roll: "); Serial.println(roll);

  if (roll < 5) {
    Serial.println("Detecting a fall");
    playMelody();  
  }

  if (cm <= 20) {
    Serial.println("high motors");
    digitalWrite(VM1, HIGH);
  } else if (cm <= 50) {
    Serial.println("medium motor");
    digitalWrite(VM1, HIGH);
    digitalWrite(VM1, LOW);
  } else {
    Serial.println("close 2 motors");
    digitalWrite(VM1, LOW);
  }

  delay(1000);
}

void vibrateMotorsTwice() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(VM1, HIGH);
    delay(500); 
    digitalWrite(VM1, LOW);
    delay(500); 
  }
}
//สั่น 2 ครั้ง ตอนเปิดเครื่อง
long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}

void playMelody() {
  unsigned long startMillis = millis();
  while (millis() - startMillis < 10000) {  
    for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i += 2) {
      int frequency = melody[i];
      int duration = melody[i + 1];

      tone(buzzerPin, frequency, duration);
      delay(duration + pauseDuration);  
    }
  }
}
