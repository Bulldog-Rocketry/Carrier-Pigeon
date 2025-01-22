#include <Wire.h>
#include <Adafruit_BMP3XX.h>
#include <SparkFun_ISM330DHCX.h>
#include <SPI.h>
#include <SD.h>

// Pins
#define SD_CS_PIN 2

// BMP388 Object
Adafruit_BMP3XX bmp;

// ISM330DHCX Object
SparkFun_ISM330DHCX imu;

sfe_ism_data_t accelData; 
sfe_ism_data_t gyroData; 

// SD Card File
File dataFile;

// Sea level pressure in hPa
#define SEA_LEVEL_PRESSURE 1013.25

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  // Initialize SD Card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed!");
    while (1);
  }
  Serial.println("SD card initialized.");

  // Initialize BMP388
  if (!bmp.begin_I2C()) {
    Serial.println("Could not find a valid BMP388 sensor!");
    while (1);
  }
  Serial.println("BMP388 initialized.");

  // Initialize ISM330DHCX
  if (!imu.begin()) {
    Serial.println("Could not initialize ISM330DHCX!");
    while (1);
  }
  imu.setDeviceConfig();
	imu.setBlockDataUpdate();
	
	// Set the output data rate and precision of the accelerometer
	imu.setAccelDataRate(ISM_XL_ODR_104Hz);
	imu.setAccelFullScale(ISM_4g); 

	// Set the output data rate and precision of the gyroscope
	imu.setGyroDataRate(ISM_GY_ODR_104Hz);
	imu.setGyroFullScale(ISM_500dps); 

	// Turn on the accelerometer's filter and apply settings. 
	imu.setAccelFilterLP2();
	imu.setAccelSlopeFilter(ISM_LP_ODR_DIV_100);

	// Turn on the gyroscope's filter and apply settings. 
	imu.setGyroFilterLP1();
	imu.setGyroLP1Bandwidth(ISM_MEDIUM);


  // Write header to SD card
  dataFile = SD.open("data.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("Time (ms),Pressure (hPa),Temperature (C),Altitude (m),AccelX (g),AccelY (g),AccelZ (g),GyroX (dps),GyroY (dps),GyroZ (dps)");
    dataFile.close();
    Serial.println("Header written to SD card.");
  } else {
    Serial.println("Could not open file on SD card!");
    while (1);
  }
}

void loop() {
  // Read BMP388 data
  float pressure, temperature, altitude;
  if (bmp.performReading()) {
    pressure = bmp.pressure / 100.0; // Convert to hPa
    temperature = bmp.temperature;
    altitude = bmp.readAltitude(SEA_LEVEL_PRESSURE); // Calculate altitude
  } else {
    Serial.println("Failed to read BMP388!");
    return;
  }

  // Read ISM330DHCX data
  float accelX, accelY, accelZ;
  float gyroX, gyroY, gyroZ;
  if( imu.checkStatus() ){
		imu.getAccel(&accelData);
		imu.getGyro(&gyroData);
    accelX = accelData.xData;
    accelY = accelData.yData;
    accelZ = accelData.zData;
    gyroX = gyroData.xData;
    gyroY = gyroData.yData;
    gyroZ = gyroData.zData;
  } else {
    gyroX = -1;
    gyroY = 0;
    gyroZ = 1;
  }
  

  // Write data to SD card
  #define dataFile = Serial //SD.open("data.csv", FILE_WRITE);
  if (true) {
    dataFile.print(millis());
    dataFile.print(",");
    dataFile.print(pressure, 2);
    dataFile.print(",");
    dataFile.print(temperature, 2);
    dataFile.print(",");
    dataFile.print(altitude, 2);
    dataFile.print(",");
    dataFile.print(accelX, 4);
    dataFile.print(",");
    dataFile.print(accelY, 4);
    dataFile.print(",");
    dataFile.print(accelZ, 4);
    dataFile.print(",");
    dataFile.print(gyroX, 2);
    dataFile.print(",");
    dataFile.print(gyroY, 2);
    dataFile.print(",");
    dataFile.println(gyroZ, 2);
    dataFile.close();
    Serial.println("Data written");
  } else {
    Serial.println("couldn't write");
  }

  // Delay to match sensor data rates
  delay(10);
}






