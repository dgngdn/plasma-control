

## Acquiring a VL6180X

Adafruit VL6180X Time of Flight Distance Ranging Sensor (VL6180)
https://www.adafruit.com/product/3316
$14
SOLD OUT

SparkFun ToF Range Finder Breakout - VL6180
https://www.sparkfun.com/products/12784
$15
No level-shifters or power supply

SparkFun ToF Range Finder Sensor - VL6180
https://www.sparkfun.com/products/12785
$25
With level-shifters and power supply

VL6180X Time-of-Flight Distance Sensor Carrier with Voltage Regulator, 60cm max
https://www.pololu.com/product/2489
https://github.com/pololu/vl6180x-arduino
https://www.pololu.com/file/download/VL6180X.pdf?file_id=0J961
$11.95

## Documentation

+ http://www.st.com/content/st_com/en/products/embedded-software/proximity-sensors-software/stsw-img005.html
+ https://github.com/pololu/vl53l0x-arduino
+ https://github.com/pololu/vl6180x-arduino

----

VL53L0X_StartMeasurement() function must be called to start a measurement. The
device will start a measurement using the chosen mode (single or continuous)

VL53L0X_PerformSingleMeasurement() function starts a measurement and waits for
data ready, by polling on the ranging status or on the interrupt status.
The 2 following API functions are called internally:
+ VL53L0X_StartMeasurement()
+ VL53L0X_GetMeasurementDataReady()

VL53L0X_PerformSingleRangingMeasurement() function starts a measurement,
waits for data ready (by polling on the ranging status or on the interrupt status) and reports
the data. This function also clears the interrupt after the measurement.
The 3 following API functions are called internally:
+ VL53L0X_PerformSingleMeasurement()
+ VL53L0X_GetRangingMeasurementData()
+ VL53L0X_ClearInterruptMask()

VL53L0X_GetRangingMeasurementData() function returns the ranging data.

----

From Pololu Library:


// based on VL53L0X_StartMeasurement()
void VL53L0X::startContinuous(uint32_t period_ms)

// Returns a range reading in millimeters when continuous mode is active
// (readRangeSingleMillimeters() also calls this function after starting a
// single-shot range measurement)
uint16_t VL53L0X::readRangeContinuousMillimeters(void)

// Performs a single-shot range measurement and returns the reading in
// millimeters
// based on VL53L0X_PerformSingleRangingMeasurement()
uint16_t VL53L0X::readRangeSingleMillimeters(void)


So what does 'start a single-shot range measurement' look like,
compared to 'start continuous measurements'?

  writeReg(0x80, 0x01);
  writeReg(0xFF, 0x01);
  writeReg(0x00, 0x00);
  writeReg(0x91, stop_variable);
  writeReg(0x00, 0x01);
  writeReg(0xFF, 0x00);
  writeReg(0x80, 0x00);
  writeReg(SYSRANGE_START, 0x01); // VL53L0X_REG_SYSRANGE_MODE_SINGLESHOT

versus...

  writeReg(0x80, 0x01);
  writeReg(0xFF, 0x01);
  writeReg(0x00, 0x00);
  writeReg(0x91, stop_variable);
  writeReg(0x00, 0x01);
  writeReg(0xFF, 0x00);
  writeReg(0x80, 0x00);
  writeReg(SYSRANGE_START, 0x04); // VL53L0X_REG_SYSRANGE_MODE_TIMED
    OR
  writeReg(SYSRANGE_START, 0x02); // VL53L0X_REG_SYSRANGE_MODE_BACKTOBACK


'single-shot' then does GetMeasurementDataReady:

  // "Wait until start bit has been cleared"
  startTimeout();
  while (readReg(SYSRANGE_START) & 0x01)
  {
    if (checkTimeoutExpired())
    {
      did_timeout = true;
      return 65535;
    }
  }


...then GetRangingMeasurementData and ClearInterruptMask:

  return readRangeContinuousMillimeters();
     ...which contains writeReg(SYSTEM_INTERRUPT_CLEAR, 0x01); 

what is this 'system_interupt' anyway?
the ST API says:

  After reading a measurement, host has to clear the interrupt by
  using the following function.

----

In the 6180X library:

// Performs a single-shot ranging measurement
uint8_t VL6180X::readRangeSingle()
{
  writeReg(SYSRANGE__START, 0x01);
  return readRangeContinuous();
}

and...

/ Returns a range reading when continuous mode is activated
// (readRangeSingle() also calls this function after starting a single-shot
// range measurement)
uint8_t VL6180X::readRangeContinuous()
{
  uint16_t millis_start = millis();
  while ((readReg(RESULT__INTERRUPT_STATUS_GPIO) & 0x04) == 0)
  {
    if (io_timeout > 0 && ((uint16_t)millis() - millis_start) > io_timeout)
    {
      did_timeout = true;
      return 255;
    }
  }

  uint8_t range = readReg(RESULT__RANGE_VAL);
  writeReg(SYSTEM__INTERRUPT_CLEAR, 0x01);

  return range;

----

what is VL53L0X doing extra?

it turns on 0x80, then turns it off: POWER_MANAGEMENT_GO1_POWER_FORCE
it turns on 0xFF, then it turns it off: (not mentioned in .h)
it turns off 0x00, then it turns it on: SYSRANGE_START
it writes 0x91 with 'stop_variable'

----

So what do I ACTUALLY need to do?

  // Performs a single-shot ranging measurement
  uint8_t VL6180X::readRangeSingle()
  {
    writeReg(SYSRANGE__START, 0x01);
    return readRangeContinuous();
  }

just write a 

Rewrite this as:

  // Performs a single-shot ranging measurement
  uint8_t VL6180X::startRangeMeasurement()
  {
    writeReg(SYSRANGE__START, 0x01);
  }

Then, when I actually want to grab the data, just call:

readRangecontinuous()
