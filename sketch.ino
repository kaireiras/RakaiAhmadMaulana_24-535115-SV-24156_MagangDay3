/*
  Nama: Rakai Ahmad Maulana
  NIM: 24/535115/SV/24156
  Programmer 

 */

#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>

// objek sensor
Adafruit_MPU6050 mpu;
Servo axisMotor1, axisMotor2, axisMotor3, axisMotor4, axisMotor5;

// definisi pin
int motionSensorPin = 25; // pirPin
#define MOTOR1_PIN 18 // Roll Axis
#define MOTOR2_PIN 17 // Roll Axis
#define MOTOR3_PIN 16 // Pitch Axis
#define MOTOR4_PIN 15 // Pitch Axis
#define MOTOR5_PIN 14 // Yaw Axis

// state variabel
bool isMovementDetected;
float lastGyroZ = 0;

// fungsi: set semua servo ke posisi yang spesifik
void setAllAxisMotors(int position) { 
  axisMotor1.write(position);
  axisMotor2.write(position);
  axisMotor3.write(position);
  axisMotor4.write(position);
  axisMotor5.write(position);
}

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22); 
  
  pinMode(motionSensorPin, INPUT);

  // attach motor ke pin
  axisMotor1.attach(MOTOR1_PIN);
  axisMotor2.attach(MOTOR2_PIN);
  axisMotor3.attach(MOTOR3_PIN);
  axisMotor4.attach(MOTOR4_PIN);
  axisMotor5.attach(MOTOR5_PIN);

  // set posisi awal
  setAllAxisMotors(90);

  // inisialisasi mpu
  if (!mpu.begin()) {
    Serial.println("MPU6050 tidak terdeteksi!");
    while (1);
  }
  Serial.println("MPU6050 sudah siap");
}


void loop() {
  isMovementDetected = digitalRead(motionSensorPin);
  
  if (isMovementDetected) {
    // aksi Serentak
    Serial.println("PIR: pergerakan external terdeteksi, mensingkronisasi aksi.");
    
    // gerak ke posisi bebas
    axisMotor1.write(45);
    axisMotor2.write(135);
    axisMotor3.write(60);
    axisMotor4.write(120);
    axisMotor5.write(150);
    
    delay(500); // blokir semua sensor selama 0.5 detik
    
    // kembali ke posisi awal
    setAllAxisMotors(90);
  } 
  else {
  // membaca mpu
    sensors_event_t accel, gyro, temp;
    mpu.getEvent(&accel, &gyro, &temp);

    // kalkulasi roll dan pitch
    float rollAngle = atan2(accel.acceleration.y, accel.acceleration.z) * 180.0 / PI;
    float pitchAngle = atan2(-accel.acceleration.x, sqrt(accel.acceleration.y * accel.acceleration.y + accel.acceleration.z * accel.acceleration.z)) * 180.0 / PI;
    
    // yaw
    float currentGyroZ = gyro.gyro.z;

    Serial.print("Roll: "); Serial.print(rollAngle);
    Serial.print(" | Pitch: "); Serial.print(pitchAngle);
    Serial.print(" | Yaw Rate: "); Serial.println(currentGyroZ);

    //axis control: roll 
    
    // servo 1 & 2: counter-axis motion (melawan arah)
    if (rollAngle > 5) { 
      axisMotor1.write(60); // roll positif -> S1/S2 bergerak negatif (60/120)
      axisMotor2.write(120);
    } else if (rollAngle < -5) { 
      axisMotor1.write(120); // roll negatif -> S1/S2 bergerak positif (120/60)
      axisMotor2.write(60);
    } else {
      axisMotor1.write(90);
      axisMotor2.write(90);
    }
    
    // axis control: pitch

    // servo 3 & 4: co-axis motion (searah)
    if (pitchAngle > 5) { 
      axisMotor3.write(120); // pitch positif -> S3/S4 bergerak positif (120)
      axisMotor4.write(120);
    } else if (pitchAngle < -5) { 
      axisMotor3.write(60); // pitch Negatif -> S3/S4 bergerak negatif (60)
      axisMotor4.write(60);
    } else {
      axisMotor3.write(90);
      axisMotor4.write(90);
    }


    // axis control: yaw

    
    float gyroZChange = currentGyroZ - lastGyroZ;
    

    if (fabs(gyroZChange) > 2) {
      if (currentGyroZ > lastGyroZ) {// yaw positif
        axisMotor5.write(120);
      } else { // yaw negatif
        axisMotor5.write(60);
      }
      
      delay(1000); // blok semua sensor selama sedetik
      
      axisMotor5.write(90); // kembali ke posisi awal
    }
    
    lastGyroZ = currentGyroZ; // update buat next look
  }
  
  delay(100); // delay utama loop
}
