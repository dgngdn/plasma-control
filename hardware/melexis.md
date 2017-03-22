

## DIY-THERMOCAM PAPER

via: https://github.com/maxritter/DIY-Thermocam/blob/master/Documents/Scientific%20Paper.pdf

An additional spot sensor, the MLX90614 single-point IR sensor from Melexis is used to convert
the raw values from the FLIR Lepton3 to absolute temperatures. The relationship between
raw values and the temperatures can be linearized around a specific operating point. The user
can trigger a calibration over the main menu and then point the device to different hot and cold
objects in the surrounding area. One-hundred raw values from the Lepton and temperatures
from the spot sensor are sampled. A least-square fit algorithm is applied to create a calibration
formula. The result is a calibration slope and offset stored in the EEPROM for future usage.
The offset is compensated constantly with the spot sensor and the ambient temperature.

115: CMD_GET_SPOTTEMP
 Description: Get the spot temperature from the MLX90614 sensor, in Fahrenheit
or Celcius
 Returns (4 byte): Float split into four bytes, LSB first, then MSB 

Lepton raw values - 9600 byte (Lepton2) or 38400 byte (Lepton3)
o 4800 / 19200 raw values for the Lepton2 / Lepton3, split to two bytes, MSB
first, then LSB
o Raw value is 14-bit (0 … 16383) for each pixel, equivalent to infrared intensity
o Difference can be made by the total file size (10005 byte Lepton2 vs. 38805
Lepton3) 

Calibration offset - 4 byte
o Offset for the raw-to-abs temperature conversion
o Float, split to four bytes, LSB first. Reconstruction analog to the spot sensor
temp
 Calibration slope - 4 byte
o Slope for the raw-to-abs temperature conversion
o Float, split to four bytes, LSB first. Reconstruction analog to the spot sensor
temp
The absolute temperature can be calculated out of each raw value by using the following
formula:
absTemp = rawValue * calibrationSlope + calibrationOffset

Calibration: Improves the accuracy of the color to absolute temperature conversion and
should be performed after each start. During the calibration process, point the device to different
hot and cold objects to ensure a good calibration. Repeat the calibration for different surrounding
areas.

----

Similar to the visual camera module, the configuration of the Lepton is done over I2C, whereas
the data is transferred over the SPI interface with a clock speed of up to 20 MHz. The shutter
attached to the lens opens and closes automatically, when required. It provides a uniform surface,
that can be used to recalibrate the active pixel array and remove noise from the image. It
can be triggered manually by the user or automatically after a specific time period. The lens
focuses the radiation towards the focal-plane array (FPA) and a system-on-chip (SOC) solution
from provides the measured IR radiation of the single thermal pixels as 14-bit intensity value
on the output. Figure 10 gives an overview about the FLIR Lepton3 system architecture. The
FLIR Lepton2 and Lepton3 sensor have the same pinout and form factor, so only the software
part is different, which needs to catch four times the amount of pixels in four different segments.
That allows to upgrade previous versions of the DIY-Thermocam with the new sensor, too.

----

An additional spot sensor, the MLX90614 single-point IR sensor from Melexis is used to convert
the raw values from the FLIR Lepton3 to absolute temperatures. The relationship between
raw values and the temperatures can be linearized around a specific operating point. The user
can trigger a calibration over the main menu and then point the device to different hot and cold
objects in the surrounding area. One-hundred raw values from the Lepton and temperatures
from the spot sensor are sampled. A least-square fit algorithm is applied to create a calibration
formula. The result is a calibration slope and offset stored in the EEPROM for future usage.
The offset is compensated constantly with the spot sensor and the ambient temperature. 


----

https://github.com/maxritter/DIY-Thermocam/blob/master/Firmware/Source/Thermal/Calibration.h

/* Calculates the average of the 196 (14x14) pixels in the middle */
uint16_t calcAverage()

	//Refresh MLX90614 ambient temp
	mlx90614_getAmb();
	///Refresh object temperature
	mlx90614_getTemp();


----

https://cdn-shop.adafruit.com/datasheets/MLX90614.pdf

A – single zone
B – dual zone
C – gradient compensated*

a/b/c/d - 5V/3V/_/3V medial
a/b/c - single/dual/gradient comp
a/c/f/h/i - _/35/10/12/5° FOV

mlx90614aci - 5V 	5°FOV
mlx90614bci - 3.3V 	5°FOV
mlx90614dci - 3.3V  medical 5°FOV
mlx90614acf - 5V 	10°FOV
mlx90614bcf - 3.3V 	10°FOV




This effect is especially relevant for thermometers with a small FOV like the xxC and xxF as the energy
received by the sensor from the object is reduced. Therefore, Melexis has introduced the xCx version of the
MLX90614. In these MLX90614xCx, the thermal gradients are measured internally and the measured
temperature is compensated for them. In this way, the xCx version of the MLX90614 is much less sensitive to
thermal gradients, but the effect is not totally eliminated. It is therefore important to avoid the causes of thermal
gradients as much as possible or to shield the sensor from them.


MLX90614xBA really does measure two zones:

peak, zone 1	+25°
width, zone 1	70°
peak, zone 2	-25°
width, zone 2	70°

FOV of MLX90614xCF is VERY tight
FOV of MLX90614xCH is VERY tight (medical grade)
FOV of MLX90614xCI is EVEN SMALLER FOV

When I measure aluminum and plastic parts settled at the same conditions I get significant
errors on aluminum. Why?
Different materials have different emissivity. A typical value for aluminum (roughly polished) is 0.18 and for
plastics values of 0.84…0.95 are typical. IR thermometers use the radiation flux between the sensitive element
in the sensor and the object of interest, given by the equation

...

When a body with low emissivity (such as aluminum) is involved in this heat transfer, the portion of the
radiation incident to the sensor element that really comes from the object of interest decreases – and the
reflected environmental IR emissions take place. (This is all for bodies with zero transparency in the IR band.)
The IR thermometer is calibrated to stay within specified accuracy – but it has no way to separate the incoming
IR radiation into real object and reflected environmental part. Therefore, measuring objects with low emissivity
is a very sophisticated issue and infra-red measurements of such materials are a specialized field.
What can be done to solve that problem? Look at paintings – for example, oil paints are likely to have
emissivity of 0.85…0.95 – but keep in mind that the stability of the paint emissivity has inevitable impact on
measurements.
It is also a good point to keep in mind that not everything that looks black is “black” also for IR. For
example, even heavily oxidized aluminum has still emissivity as low as 0.30.
How high is enough? Not an easy question – but, in all cases the closer you need to get to the real
object temperature the higher the needed emissivity will be, of course.
With the real life emissivity values the environmental IR comes into play via the reflectivity of the object
(the sum of Emissivity, Reflectivity and Absorptivity gives 1.00 for any material). The larger the difference
between environmental and object temperature is at given reflectivity (with an opaque for IR material reflectivity
equals 1.00 minus emissivity) the bigger errors it produces.

Special care is taken to cut off the visible light spectra as well as the NIR (near IR) before it reaches the
sensitive sensor die. Even more, the glass (in most cases) is not transparent to the IR radiation used by the
MLX90614. Glass has temperature and really high emissivity in most cases – it is “black” for IR of interest.
Overall, Sun behind a window is most likely to introduce relatively small errors. Why is it not completely
eliminated after all? Even visible light partially absorbed in the filter of the sensor has some heating potential
and there is no way that the sensor die will be “blind” for that heating right in front of it.


----
MLX90614ESF-BCI-000-SP
http://www.mouser.com/ProductDetail/Melexis/MLX90614ESF-BCI-000-SP/?qs=%2fha2pyFaduhIZ08g7grvytRGY7Rlf%252bxSPTI1qUS0sM9cXY8yenAz8JM3bBXMHzP%252b
$36.83

https://www.digikey.com/product-detail/en/melexis-technologies-nv/MLX90614ESF-BCI-000-TU/MLX90614ESF-BCI-000-TU-ND/2666249
36.84000

MLX90614ESF-DCI-000-SP
http://www.mouser.com/ProductDetail/Melexis/MLX90614ESF-DCI-000-SP/?qs=%2fha2pyFaduhIZ08g7grvyi6KZ19QHzFlrcEDWqTWkbPVewFdm0XmCdY8fpO0lXRb
$50.93

https://www.digikey.com/product-detail/en/melexis-technologies-nv/MLX90614ESF-DCI-000-TU/MLX90614ESF-DCI-000-TU-ND/3131345
$--

