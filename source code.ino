#include <Wire.h>
#include <MPU6050.h>

const int trigPin = 5;     
const int echoPin = 4;     

MPU6050 mpu;                      

int VM1 = A1;  

unsigned long previousMillis = 0;   
const long interval = 1000;         

const int buzzerPin = 9; 

int melody[] = {
  1000, 500, 
  1500, 500, 
  1000, 500, 
  1500, 500, 
  1000, 500, 
  1500, 500  
};

int noteDuration = 10000; 
int pauseDuration = 10;    

void setup() {
  pinMode(VM1, OUTPUT);
  Serial.begin(9600);      
  Wire.begin();            

  mpu.initialize();        

  if(mpu.testConnection()) {
    Serial.println("MPU6050 connected successfully");
  } else {
    Serial.println("MPU6050 connection failed");
    while(1); 
  }

  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_2000); 

  pinMode(trigPin, OUTPUT);    
  pinMode(echoPin, INPUT);     
  
  pinMode(buzzerPin, OUTPUT);  

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

  int16_t ax, ay, az;
  int16_t gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  long acceleration = sqrt(sq(ax) + sq(ay) + sq(az));
  long gyro = sqrt(sq(gx) + sq(gy) + sq(gz));

  
  float pitch = atan2(ay, az) * 180 / PI;
  float roll = atan2(ax, az) * 180 / PI;

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
