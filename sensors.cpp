
// https://github.com/jameszah/ESP32-CAM-RocketCam -- Aug 28, 2022

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);
#include <Adafruit_ADXL345_U.h>
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
bool bmp180 = false;
bool adxl345 = false;

void setup_sensors() {
  Wire.begin(3, 13 );
  Serial.println("\nI2C Scanner");

  byte error, address;
  int nDevices;

  Serial.println("Scanning...");

  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
      nDevices++;
    }
    else if (error == 4)
    {
      Serial.print("Unknown error at address 0x");
      if (address < 16)
        Serial.print("0");
      Serial.println(address, HEX);
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");

  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  } else {
    bmp180 = true;
    sensor_t sensor;
    bmp.getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.print  ("Sensor:       "); Serial.println(sensor.name);
    Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
    Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
    Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" hPa");
    Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" hPa");
    Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" hPa");
    Serial.println("------------------------------------");
    sensors_event_t event;
    bmp.getEvent(&event);
    if (event.pressure) {
      Serial.print("Pressure:    ");
      Serial.print(event.pressure);
      Serial.println(" hPa");
      float temperature;
      bmp.getTemperature(&temperature);
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.println(" C");
      float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
      Serial.print("Altitude:    ");
      Serial.print(bmp.pressureToAltitude(seaLevelPressure,
                                          event.pressure));
      Serial.println(" m");
      Serial.println("");

    }
  }

  if (!accel.begin()) {
    /* There was a problem detecting the ADXL345 ... check your connections */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
  } else {
    adxl345 = true;
    sensor_t asensor;
    accel.getSensor(&asensor);
    Serial.println("------------------------------------");
    Serial.print  ("Sensor:       "); Serial.println(asensor.name);
    Serial.print  ("Driver Ver:   "); Serial.println(asensor.version);
    Serial.print  ("Unique ID:    "); Serial.println(asensor.sensor_id);
    Serial.print  ("Max Value:    "); Serial.print(asensor.max_value); Serial.println(" m/s^2");
    Serial.print  ("Min Value:    "); Serial.print(asensor.min_value); Serial.println(" m/s^2");
    Serial.print  ("Resolution:   "); Serial.print(asensor.resolution); Serial.println(" m/s^2");
    Serial.println("------------------------------------");
    Serial.println("");
    accel.setRange(ADXL345_RANGE_16_G);
    sensors_event_t aevent;
    accel.getEvent(&aevent);

    /* Display the results (acceleration is measured in m/s^2) */
    Serial.print("X: "); Serial.print(aevent.acceleration.x); Serial.print("  ");
    Serial.print("Y: "); Serial.print(aevent.acceleration.y); Serial.print("  ");
    Serial.print("Z: "); Serial.print(aevent.acceleration.z); Serial.print("  "); Serial.println("m/s^2 ");
    delay(500);
  }
}

void get_adxl(float &altitude, float &temperature) {
  // you can get a more precise measurement of altitude
  // if you know the current sea level pressure which will
  // vary with weather and such. If it is 1015 millibars
  // that is equal to 101500 Pascals.

  sensors_event_t event;
  bmp.getEvent(&event);
  if (event.pressure) {

    float temp;
    bmp.getTemperature(&temp);
    float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
    temperature = temp;
    altitude = bmp.pressureToAltitude(seaLevelPressure, event.pressure);

  } else {
    altitude = -1;
    temperature = -1;
  }
}


void get_bmp180(float &x, float &y, float &z) {
  sensors_event_t aevent;
  accel.getEvent(&aevent);
  if (aevent.acceleration.x) {
    x = aevent.acceleration.x;
    y = aevent.acceleration.y;
    z = aevent.acceleration.z;
  } else {
    x = -1;
    y = -1;
    z = -1;
  }
}
