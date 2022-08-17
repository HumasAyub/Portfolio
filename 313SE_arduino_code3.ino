
#include <LiquidCrystal.h>
#include<Wire.h>
#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11 //add zip library to make this work 

static const int DHT_SENSOR_PIN = 4;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );
const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
//float roll, pitch, yaw;
float accAngleX, accAngleY, accAngleZ, gyroAngleX, gyroAngleY, gyroAngleZ;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
int c = 0;
float elapsedTime, currentTime, previousTime;
//float temperature;
//float humidity;
//int roll2, pitch2, yaw2;
//float Tmp;
//int Tmp2;
//int buttonState = 0;
//int temperature2;
//int humidity2;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//int buttonState2 = 0;
void setup() 
{
  


  lcd.begin(16, 2);
  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  pinMode(26, OUTPUT);//GREEN LED
  pinMode(24, OUTPUT);//YELLOW LED
  pinMode(22, OUTPUT);//RED LED
  pinMode(28, OUTPUT);//BLUE LED
  pinMode(3, INPUT);//SWITCH 1
//  pinMode(5, INPUT);//SWITCH 2
   
 
 // Configure Accelerometer Sensitivity - Full Scale Range (default +/- 2g)
 Wire.beginTransmission(MPU);
  Wire.write(0x1C);                  //Talk to the ACCEL_CONFIG register (1C hex)
  Wire.write(0x10);                  //Set the register bits as 00010000 (+/- 8g full scale range)
  Wire.endTransmission(true);
  // Configure Gyro Sensitivity - Full Scale Range (default +/- 250deg/s)
  Wire.beginTransmission(MPU);
  Wire.write(0x1B);                   // Talk to the GYRO_CONFIG register (1B hex)
  Wire.write(0x10);                   // Set the register bits as 00010000 (1000deg/s full scale)
  Wire.endTransmission(true);
  delay(20);
  
  // Call this function if you need to get the IMU error values for your module
   calculate_IMU_error();
  delay(20);
}

void loop() {
float Tmp;
int Tmp2;
float roll, pitch, yaw;
float temperature;
float humidity;
int temperature2;
int humidity2;
int buttonState = 0;
int roll2, pitch2, yaw2;


  // === Read acceleromter data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
  // Calculating Roll and Pitch from the accelerometer data
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L) // GYRO TEMP
 
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI); // AccErrorX ~(0.58) See the calculate_IMU_error()custom function for more details
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI); // AccErrorY ~(-1.58)
  // === Read gyroscope data === //
  previousTime = currentTime;        // Previous time is stored before the actual time read
  currentTime = millis();            // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
  GyroX = GyroX - GyroErrorX; // GyroErrorX ~(+0.01)
  GyroY = GyroY - GyroErrorY; // GyroErrorY ~(-2.18)
  GyroZ = GyroZ  - GyroErrorZ; // GyroErrorZ ~ (-1.53)
  accAngleX = accAngleX - AccErrorX;
  accAngleY = accAngleY - AccErrorY;
  
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw =  yaw + GyroZ * elapsedTime;
  // Complementary filter - combine acceleromter and gyro angle values
//  roll = 0.5 * gyroAngleX + 0.5 * accAngleX;
// pitch = 0.5 * gyroAngleY + 0.5 * accAngleY;

 roll = 0.005 * gyroAngleX + 0.995 * accAngleX;
  pitch = 0.005 * gyroAngleY + 0.995 * accAngleY;

  
 
  buttonState = digitalRead(3);
  //buttonState2 = digitalRead(5);
  
  if( measure_environment( &temperature, &humidity ) == true ){
    
  LCDDisplay(humidity, temperature, roll, pitch, yaw, Tmp, buttonState);
  
HotTempWarning(temperature);
ColdTempWarning(temperature);
PitchWarning(pitch);
RollWarning(roll);

  delay(100);
  }

Serial.begin(9600);
SerialCommunication(roll, pitch, yaw, Tmp, temperature, humidity);

 /* Serial.write('<'); // start marker
roll2 = (uint8_t)roll ;
  Serial.write(roll2);
//Serial.write(','); // comma separator
  pitch2 = (uint8_t)pitch;
 Serial.write(pitch2);
//Serial.write(','); // comma separator
 yaw2 = (uint8_t)yaw;
  Serial.write(yaw2);
//Serial.write(','); // comma separator
  Tmp2 = (uint8_t)Tmp;
 Serial.write(Tmp2);
//Serial.write(','); // comma separator
   temperature2 = (uint8_t)temperature;
Serial.write(temperature2);
//Serial.write(','); // comma separator
humidity2 = (uint8_t)humidity;
Serial.write(humidity2);
//Serial.write('>'); // end marker */

//while (Serial.available() > 0) {
  //  Serial.read();
//}
}
void calculate_IMU_error() {
//  float AccX, AccY, AccZ;
//float GyroX, GyroY, GyroZ;
//float accAngleX, accAngleY, accAngleZ, gyroAngleX, gyroAngleY, gyroAngleZ;
//float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
//int c;
  // We can call this funtion in the setup section to calculate the accelerometer and gyro data error. From here we will get the error values used in the above equations printed on the Serial Monitor.
  // Note that we should place the IMU flat in order to get the proper values, so that we then can the correct values
  // Read accelerometer values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0 ;
    // Sum all readings
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  //Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;
  // Read gyro values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }
  //Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  // Print the error values on the Serial Monitor
  //Serial.print("AccErrorX: ");
  //Serial.println(AccErrorX);
  //Serial.print("AccErrorY: ");
  //Serial.println(AccErrorY);
  //Serial.print("GyroErrorX: ");
  //Serial.println(GyroErrorX);
  //Serial.print("GyroErrorY: ");
  //Serial.println(GyroErrorY);
  //Serial.print("GyroErrorZ: ");
  //Serial.println(GyroErrorZ);
}

static bool measure_environment( float *temperature, float *humidity ){
//float temperature;
//float humidity;
  static unsigned long measurement_timestamp = millis( );

  /* Measure once every four seconds. */
  if( millis( ) - measurement_timestamp > 3000ul )
  {
    if( dht_sensor.measure( temperature, humidity ) == true )
    {
      measurement_timestamp = millis( );
      return( true );
    }
  }

  return( false );
}


void SerialCommunication(float roll, float pitch, float yaw, float Tmp, float temperature, float humidity) {
Serial.begin(9600);
//float roll, pitch, yaw;
//float Tmp;
//float temperature;
//float humidity;
int roll2;
int pitch2;
int yaw2;
int Tmp2;
int temperature2;
int humidity2;

      boolean newData = true;
  roll2 = (uint8_t)roll ;
 pitch2 = (uint8_t)pitch;
  yaw2 = (uint8_t)yaw;
   Tmp2 = (uint8_t)Tmp;
    temperature2 = (uint8_t)temperature;
    humidity2 = (uint8_t)humidity;
    if (newData == true) {
Serial.read();
Serial.write(roll2);
delayMicroseconds(20);
Serial.read();
Serial.write(pitch2);
delayMicroseconds(20);
Serial.read();
Serial.write(yaw2);
delayMicroseconds(20);
Serial.read();
Serial.write(Tmp2);
delayMicroseconds(20);
Serial.read();
Serial.write(temperature2);
delayMicroseconds(20);
Serial.read();
Serial.write(humidity2);
delayMicroseconds(20);
GPSWaypoints();
delayMicroseconds(20);
Serial.read();
Serial.write(-128);
delayMicroseconds(20);
       // newData = false;
    }
    delay(20);
}

  
void GPSWaypoints(){
  int lat1, long1;
  lat1 = 60;
long1 = 65;
Serial.read();
Serial.write(lat1);
Serial.read();
Serial.write(long1);

int lat2, long2;
lat2 = 65;
long2 = 65;
Serial.read();
Serial.write(lat2);
Serial.read();
Serial.write(long2);

int lat3, long3;
lat3 = 67;
long3 = 68;
Serial.read();
Serial.write(lat3);
Serial.read();
Serial.write(long3);

int lat4, long4;
lat4 = 69;
long4 = 62;
Serial.read();
Serial.write(lat4);
Serial.read();
Serial.write(long4);

int lat5, long5;
lat5 = 69;
long5 = 62;
Serial.read();
Serial.write(lat5);
Serial.read();
Serial.write(long5);
}

void LCDDisplay(float humidity,float temperature, float roll, float pitch, float yaw, float Tmp, int buttonState){
//float roll, pitch, yaw;
//float Tmp;
//float temperature;
//float humidity;
      if (buttonState == HIGH){
    //
 // {
  lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("H:");
    lcd.setCursor(2,0);
   lcd.print(humidity);
    lcd.setCursor(0,1);
    lcd.print("Ta:");
    lcd.setCursor(3,1);
    lcd.print(temperature);
    
  //}
    
  } 
  else {
    
    // turn LED off:
  lcd.setCursor(0, 0);
  lcd.print("R:");
  lcd.setCursor(2, 0);
 lcd.print(roll);
 lcd.setCursor(0,1);
  lcd.print("P:");
  lcd.setCursor(2,1);
  lcd.print(pitch);
  lcd.setCursor(8,0);
  lcd.print("Y:");
  lcd.setCursor(10,0);
  lcd.print(yaw);
  lcd.setCursor(8,1);
  lcd.print("T:");
  lcd.setCursor(10,1);
  lcd.print(Tmp);
  }
}


  


void HotTempWarning(float temperature){
//float temperature;
if (temperature > 31) {
  digitalWrite(22, HIGH);} 
    else{
  digitalWrite(22, LOW); }
  }

  void ColdTempWarning(float temperature){
//  float temperature;
      if (temperature < 8) {
  digitalWrite(28, HIGH);} 
  else{
  digitalWrite(28, LOW);}
  
  }
  
  void PitchWarning(float pitch) {
   // float pitch
    if (pitch > 45 || pitch < -45){
  digitalWrite(24, HIGH);}
  else{
  digitalWrite(24, LOW); }
  }

  

  void RollWarning(float roll) {
    //float roll
      if (roll >45 || roll < -45){
  digitalWrite(26, HIGH);}
    else{
    digitalWrite(26, LOW);}
 
  }
  
