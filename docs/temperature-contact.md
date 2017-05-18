
## Dallas Semiconductor DS18B20

https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf

interface: 1-wire
resolution: 9-12 bits (programmable; 0.5-0.0625°C)

Supports asynchronous operation, introduced by Rob Tillaart in v3.7:

  sensors.setWaitForConversion(false); // make non-blocking
  sensors.requestTemperatures(); // this requests a new temperature, but it'll take time to get it!
  sensors.getTempCByIndex(0); // in the meantime, this will give you the most recent measurement
  
From the datasheet, https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf:

"To initiate a temperature measurement and A-to-D conversion, the master must issue a Convert T [44h] command. Following the conversion, the resulting thermal data is stored in the 2-byte temperature register in the scratchpad memory and the DS18B20 returns to its idle state."

setWaitForConversion()
// sets the value of the waitForConversion flag
// TRUE : function requestTemperature() etc returns when conversion is ready
          it does this by calling blockTillConversionComplete()
// FALSE: function requestTemperature() etc returns immediately (USE WITH CARE!!)

setCheckForConversion()
// sets the value of the checkForConversion flag
// TRUE : function requestTemperature() etc will 'listen' to an IC to determine whether a conversion is complete
          blockTillConversionComplete will poll the sensor and return as soon as it indicates it's ready
// FALSE: function requestTemperature() etc will wait a set time (worst case scenario) for a conversion to complete
          blockTillConversionComplete will just delay for the duration specified in the datasheet

There is also a way for the microcontroller to poll the sensor to determine when the newly-requested measurement is ready.  This is somehow implemented with setCheckForConversion() and isConversionComplete().

Arduino library: https://github.com/milesburton/Arduino-Temperature-Control-Library

Timing Experiments:

  file = ds18b20-timings.ino
  
  1) based on these experiments, it looks like a single read-only-loop takes about 28 milliseconds!
  2) in the example sketches, the microcontroller blindly waits a fixed time for the temperature measurement and conversion instead of polling the sensor.  The listed measure-and-convert times are VERY slow, so this takes forever!
    
    resolution    max time    observed time
    9 bits        93.75 ms    60 ms, 58 ms, 58 ms
    10 bits       187.5 ms    146, 145
    11 bits       375 ms      289, 290, 290
    12 bits       750 ms      676 ms, 576 ms, 575 ms
    
### Library Problems

The library that calls itself "version 3.7.6" appears to contain some code that's over two years old.  This demo also prints a LIBRARYVERSION variable that's set to 3.7.3.  What's going on here?

This is how libraries are added to the Arduino IDD Library Manager: 

Here is the GitHub Issue filed with Arduino that added this library: https://github.com/arduino/Arduino/issues/3517

Someone's already noticed this!: "Latest release is 3.7.5, but Library Manager install version 3.7.2. Bug?"

SOLUTION: 3.7.6 contained lots of inconsistencies and errors.  All of this has since been corrected on GitHub.  Installing the newest build from the Master branch on GitHub made it work great.

Most notably: the isConversionComplete() function works correctly now!  You can use this function to see if the new measurement you've queued up is ready.  This is useful because this check takes microseconds, while getTemp() is ~28 milliseconds!  (this is still very slow... can we speed it up?)

Can we speed things up if an address isn't needed?  Datasheet says:

Search ROM [F0h]:

"If there is only one slave on the bus, the simpler Read ROM [33h] command can be used in place of the Search ROM process."

Read ROM [33h]:

"This command can only be used when there is one slave on the bus. It allows the bus master to read the slave’s 64-bit ROM code without using the Search ROM procedure. If this command is used when there is more than one slave present on the bus, a data collision will occur when all the slaves attempt to respond at the same time."

Skip Rom [CCh]:

The master can use this command to address all devices on the bus simultaneously without sending out any ROM code information. Note that the Read Scratchpad [BEh] command can follow the Skip ROM command only if there is a single slave device on the bus. In this case, time is saved by allowing the master to read from the slave without sending the device’s 64-bit ROM code. A Skip ROM command followed by a Read Scratchpad command will cause a data collision on the bus if there is more than one slave since multiple devices will attempt to transmit data simultaneously.

Match ROM [55h] does the device-matching. Not needed for single-drop!

However, these are the only OneWire commands implemented in the library:

  // OneWire commands
  #define STARTCONVO      0x44  // Tells device to take a temperature reading and put it on the scratchpad
  #define COPYSCRATCH     0x48  // Copy EEPROM
  #define READSCRATCH     0xBE  // Read EEPROM
  #define WRITESCRATCH    0x4E  // Write to EEPROM
  #define RECALLSCRATCH   0xB8  // Reload from last known
  #define READPOWERSUPPLY 0xB4  // Determine if device needs parasite power
  #define ALARMSEARCH     0xEC  // Query bus for devices with an alarm condition

As described in the datasheet:

  Convert T [44h]         // initiates a single temperature conversion. 
  Copy Scratchpad [48h]   // This command copies the contents of the scratchpad TH, TL and configuration registers (bytes 2, 3 and 4) to EEPROM.
  Read Scratchpad [BEh]   // allows the master to read the contents of the scratchpad.
  Write Scratchpad [4Eh]  // allows the master to write 3 bytes of data to the DS18B20’s scratchpad.
  Recall E2 [B8h]         // recalls the alarm trigger values (TH and TL) and configuration data from EEPROM and places the data in bytes 2, 3, and 4, respectively, in the scratchpad memory
  Read Power Supply [B4h] // master device issues this command followed by a read time slot to determine if any DS18B20s on the bus are using parasite power
  Alarm Search [ECh]      // The operation of this command is identical to the operation of the Search ROM command except that only slaves with a set alarm flag will respond.

Actually, I think the underlying Wire library implements Skip ROM [0xCC]:

  // sends command for all devices on the bus to perform a temperature conversion
  void DallasTemperature::requestTemperatures(){
      _wire->reset();
      _wire->skip();
      _wire->write(STARTCONVO, parasite);
      // ASYNC mode?
      if (!waitForConversion) return;
      blockTillConversionComplete(bitResolution);
  }

However, this is the ONLY use of skip() in the Dallas Temperature library.

Right now, getTempCByIndex() requires a deviceIndex so it can look up that device's address. Lots of wasted time!

  // Fetch temperature for device index
  float DallasTemperature::getTempCByIndex(uint8_t deviceIndex){
      DeviceAddress deviceAddress;
      if (!getAddress(deviceAddress, deviceIndex)){
          return DEVICE_DISCONNECTED_C;
      }
      return getTempC((uint8_t*)deviceAddress);
  }

Likewise, the underlying getTempC() also requires an address:

  // returns temperature in degrees C or DEVICE_DISCONNECTED_C if the
  // device's scratch pad cannot be read successfully.
  // the numeric value of DEVICE_DISCONNECTED_C is defined in
  // DallasTemperature.h. It is a large negative number outside the
  // operating range of the device
  float DallasTemperature::getTempC(const uint8_t* deviceAddress){
      return rawToCelsius(getTemp(deviceAddress));
  }

Solution: implement a getTempCSingle(void) and getTempC(void) and getTemp(void) for use by single-drop systems!
  getTemp(void) calls calculateTemperature, which also requires an address... also need a non-address version of this!
  getTemp() also calls isConnected(address, scratchpad), which calls readScratchpad and updates the scratchpad, all of which require addresses...
  so we actually need new versions of:
    [x] readScratchpad(scratchpad) -> singleReadScratchPad(scratchpad)
    [x] isConnected(scratchpad)    -> must be renamed! -> singleConnected(scratchpad)
    [x] calculateTemperature       -> singleCalculateTemperature(scratchpad)
    [x] getTemp                    -> singleGetTemp(void)
    [x] getTempC                   -> singleGetTempC(void)

Implemented!
  https://github.com/graveslab/Arduino-Temperature-Control-Library/commit/5f9af016927ed708041bc59f03e3b24c0c4eb0af
  The read operation (as measured by ds18b20) is now ~8.5 milliseconds instead of ~26.5ms!

Can we replace these functions with default arguments to the existing functions?

    bool singleReadScratchPad(uint8_t*);
    bool singleIsConnected(uint8_t*);
    int16_t singleCalculateTemperature(uint8_t*, bool);
    int16_t singleGetTemp(bool);
    float singleGetTempC(bool);
    float singleGetTempF(bool);

I combined this stuff and got it all working, so getTempC() can be called without an address and it's faster, 8ms per read call:

  900
  908
   Temperature: 24.5
    Resolution: 9
  Time elapsed: 83

  1120
  1128
   Temperature: 24.75
    Resolution: 10
  Time elapsed: 157

  1487
  1496
   Temperature: 24.875
    Resolution: 11
  Time elapsed: 305

  2150
  2157
   Temperature: 24.8125
    Resolution: 12
  Time elapsed: 599

Providing the address to getTempC() slows things down a little to about 12ms per read call:

  905
  917
   Temperature: 25.0
    Resolution: 9
  Time elapsed: 87

  1129
  1141
   Temperature: 25.00
    Resolution: 10
  Time elapsed: 160

  1501
  1513
   Temperature: 25.000
    Resolution: 11
  Time elapsed: 309

  2167
  2181
   Temperature: 24.8750
    Resolution: 12
  Time elapsed: 603

getTempCByIndex() takes FOREVER, 26-28 ms per call:

  919
  946
   Temperature: 24.5
    Resolution: 9
  Time elapsed: 102

  1158
  1185
   Temperature: 24.75
    Resolution: 10
  Time elapsed: 176

  1545
  1571
   Temperature: 24.875
    Resolution: 11
  Time elapsed: 322

  2226
  2252
   Temperature: 24.8750
    Resolution: 12
  Time elapsed: 618




### Speeding Things Up

Wikipedia says that 1-wire was specifically designed for low-speed acquisition, where the primary goals are (1) minimizing cabling and (2) maximizing potential cable length.  It is similar to i2c, but i2c is 10-100x faster.

https://en.wikipedia.org/wiki/1-Wire
https://en.wikipedia.org/wiki/I%C2%B2C

The TMP100 series are very similar to the DS18B20 but use i2c instead of 1-wire.

http://www.ti.com/lit/ds/symlink/tmp101.pdf
2.7-5.5V, 9-12 bit resolution
delta-sigma ADC converter
typical conversion times from 40ms (9-bit) to 320ms (12-bit), but transfer should be much faster than 1-wire.


### Supporting Other Chips

The DS18S20 was designed to be a drop-in replacement for the DS1820, but is otherwise entirely inferior to the DS18B20.

https://www.maximintegrated.com/en/app-notes/index.mvp/id/4377

I purchased a DS18S20 so that I can test my changes and verify that the library still works well with that sensor!


