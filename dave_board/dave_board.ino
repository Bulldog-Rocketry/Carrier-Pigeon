// Aerial code to write sensor data to MicroSD card during rocket flight
// CONDITION TO BEGIN WRITING TO SD CARD: Y ACCELEROMETERS MUST MEASURE GREATER THAN +20
// CONDITION TO END WRITING TO SD CARD: 20 MINUTES HAVE ELAPSED, AS MEASURED BY THE VARIABLE "COUNT"

// necessary libraries
#include <Wire.h>
#include <Adafruit_ICM20649.h>
#include <Adafruit_MMC56x3.h>
#include <Adafruit_BMP3XX.h>
#include <Adafruit_Sensor.h>
#include <SD.h>
#include <SPI.h>
//Addressess for IMUs
Adafruit_ICM20649 icm1;
Adafruit_ICM20649 icm2;
Adafruit_ICM20649 icm3;
//Addresses for MMCs
Adafruit_MMC5603 mmc = Adafruit_MMC5603(0x63);
//Barometer Address
Adafruit_BMP3XX bmp;
#define TCAADDR 0x70  //Multiplexer
#define I2C_SDA 32    // Programmed different pins for I2C connection
#define I2C_SCL 14
unsigned long StartTime = millis();  // not used
// creating an SD card writer variable
File myFile;
int count = 0;
bool launched = false;

void tcaselect(uint8_t i) {      //Multiplexer switching function
  Wire.beginTransmission(0x70);  //Multiplexer address hex
  Wire.write(1 << i);
  Wire.endTransmission();
}

void setup(void) {
  Serial.begin(9600);
  while (!Serial)
    delay(10);
  Wire.begin(I2C_SDA, I2C_SCL);
  pinMode(2, OUTPUT);                                //CS Pin
  if(SD.begin(2)) {
    Serial.print("SD card sucess");
  }                                      //CS Pin
  myFile = SD.open("/Launch_File.txt", FILE_WRITE);  //Open File, or create it
  myFile.println("temp1 temp2 temp3 x1 x2 x3 z1 z2 z3 y1 y2 y3 gx1 gx2 gx3 gy1 gy2 gy3 gz1 gz2 gz3 mx my mz pressure alt");
  //Initializing IMU
  tcaselect(0);
  if (!icm1.begin_I2C())
    Serial.println("Failed ICM1");
  tcaselect(1);
  if (!icm2.begin_I2C())
    Serial.println("Failed ICM2");
  tcaselect(2);
  if (!icm3.begin_I2C())
    Serial.println("Failed ICM3");
  //Initializing MMC
  tcaselect(4);
  if (!mmc.begin())
    Serial.println("Failed MMC1");
  tcaselect(3);
  if (!bmp.begin_I2C())
    Serial.println("Failed BMP");
}
void loop() {
  launched = true;
  sensors_event_t accel1, accel2, accel3;
  sensors_event_t gyro1, gyro2, gyro3;
  sensors_event_t mag;
  sensors_event_t temp1, temp2, temp3;
  tcaselect(0);
  icm1.getEvent(&accel1, &gyro1, &temp1);
  tcaselect(1);
  icm2.getEvent(&accel2, &gyro2, &temp2);
  tcaselect(2);
  icm3.getEvent(&accel3, &gyro3, &temp3);

  // need to check y-axis accelerometer values to determine if
  // launch occurred. When rocket is sitting upright on launch pad,
  // they measure approx. 9.8 due to gravity. A launch would push
  // this value well above 20


  while (launched) {
    Serial.print("writing data");
    //selecting through the IMU's
    tcaselect(0);
    icm1.getEvent(&accel1, &gyro1, &temp1);
    tcaselect(1);
    icm2.getEvent(&accel2, &gyro2, &temp2);
    tcaselect(2);
    icm3.getEvent(&accel3, &gyro3, &temp3);
    tcaselect(4);
    //selecting through the MMC's
    mmc.getEvent(&mag);
    //bmp pressure check
    tcaselect(3);

    myFile.print(temp1.temperature);
    myFile.print(" ");
    myFile.print(temp2.temperature);
    myFile.print(" ");
    myFile.print(temp3.temperature);
    myFile.print(" ");

    myFile.print(accel1.acceleration.x);
    myFile.print(" ");
    myFile.print(accel2.acceleration.x);
    myFile.print(" ");
    myFile.print(accel3.acceleration.x);
    myFile.print(" ");

    myFile.print(accel1.acceleration.z);
    myFile.print(" ");
    myFile.print(accel2.acceleration.z);
    myFile.print(" ");
    myFile.print(accel3.acceleration.z);
    myFile.print(" ");

    myFile.print(-accel1.acceleration.y);
    myFile.print(" ");
    myFile.print(-accel2.acceleration.y);
    myFile.print(" ");
    myFile.print(-accel3.acceleration.y);
    myFile.print(" ");

    myFile.print(gyro1.gyro.x);
    myFile.print(" ");
    myFile.print(gyro2.gyro.x);
    myFile.print(" ");
    myFile.print(gyro3.gyro.x);
    myFile.print(" ");

    myFile.print(gyro1.gyro.y);
    myFile.print(" ");
    myFile.print(gyro2.gyro.y);
    myFile.print(" ");
    myFile.print(gyro3.gyro.y);
    myFile.print(" ");

    myFile.print(gyro1.gyro.z);
    myFile.print(" ");
    myFile.print(gyro2.gyro.z);
    myFile.print(" ");
    myFile.print(gyro3.gyro.z);
    myFile.print(" ");

    myFile.print(mag.magnetic.x);
    myFile.print(" ");
    myFile.print(mag.magnetic.y);
    myFile.print(" ");
    myFile.print(mag.magnetic.z);
    myFile.print(" ");

    myFile.print(bmp.pressure);
    myFile.print(" ");
    myFile.print(bmp.readAltitude(1013.25));  //sea level is 1013.25
    myFile.println();
    count++;         // incremented once each loop. with a delay of 0.1 second, it will take 12,000 loops to add up to 20 minutes
    myFile.flush();  // to force data to be written to SD card without needing to close file
    delay(1000);

    if (count == 12000) {  // a count of 12,000 means 20 minutes have elapsed and rocket has landed
      myFile.close();      // rocket has landed, close the file
      while (1) {          // infinite loop to wait for SD card retrieval
      }
    }
  }
}
