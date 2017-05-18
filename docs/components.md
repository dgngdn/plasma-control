# OP AMPS

## Parameters

### input bias current

Ideally, no current flows into the input terminals of an op amp. In practice, there are always two
input bias currents, IB+ and IB-. These currents may vary in magnitude and polarity!  This current will flow through external impedances and generate additional voltages that mess up your inputs.

[ad-opamp-bias]: http://www.analog.com/media/en/training-seminars/tutorials/MT-038.pdf

### input offset voltage

Ideally, if both inputs of an op amp are at exactly the same voltage, then the output should be at
zero volts. In practice, a small differential voltage must be applied to the inputs to force the
output to zero. This is known as the input offset voltage, VOS.

The best bipolar devices can have 10x lower values than the best JFET devices.

ref: [Analog Devices tutorial on op-amp input offset voltage and mitigation techniques][ad-offsetvolt]

[ad-offset-volt]: http://www.analog.com/media/en/training-seminars/tutorials/MT-037.pdf


### input voltage range

The inputs to the op amp are expressed in two parts: a "common-mode voltage" and a "differential voltage"

Vc = (V1 + V2) / 2
Vd = (V1 - V2) / 2

If the single-supply common-mode input voltage range includes the lower rail, that means it's okay to pull both inputs to ground.  If the input voltage range does NOT include the lower rail, the op amp is better suited for dual-supply operation.

ref: [Op Amp Input and Output Common-Mode and Differential Voltage Range][and-opamp-range]

[ad-opamp-range]: http://www.analog.com/media/en/training-seminars/tutorials/MT-041.pdf

### output voltage range

An ideal op-amp can swing the output 'rail-to-rail', from Vee (low supply rail) to Vcc (high supply rail).  The output of a real op amp saturates at a voltage that falls short of the supply rails.  In practice, the output range will be a function of the supply voltages, the load impedance, and the input signal frequency.

### input impedance

The input impedance is expressed in two parts: a "common-mode impedance" and a "differential impedance"

The common-mode impedance describes the impedance of a single input to ground.

The differential impedance describes the impedance between the two inputs.

"In many applications, the input capacitance of an op amp is not a problem. However where the
source impedance is high, such as in a photodiode preamp, the diode capacitance adds to the op
amp input capacitance and may require the addition of a feedback capacitor to stabilize the op
amp. For high impedance high frequency sources, the input capacitance of the op amp should be
significantly less than the source capacitance."

[analog-opamp-imp]: http://www.analog.com/media/en/training-seminars/tutorials/MT-040.pdf

## stability

"Internally compensated op amps can be made unstable in several ways: by driving capacitive loads, by adding capacitance to the inverting input lead, and by adding in phase feedback with external components."

"Input capacitance is easily compensated by adding a feedback capacitor into the circuit. The value of the feedback capacitor should be just large enough to achieve the desired overshoot response, because larger values cause a loss of high-frequency performance."

ref: [TI op amp stability][ti-opamp-stability].

[ti-opamp-stability]: http://www.ti.com/lit/an/slyt087/slyt087.pdf

### total harmonic distortion

Describes the linearity of the input-output relationship.


### frequency response

The output impedance increases with increasing frequency; likewise, the peak-to-peak swing of the output voltage falls with increasing frequency.

### short circuit protection

Many op amps have "infinite" short-circuit protection and can run continuously with the output shorted.


## Example Devices

### MC34072, MC33072

supply  single
        3-44V
GBP     4.5 MHz
slew    13 V/µs
type    Bipolar
drive   0-10,000 pF
current 30 mA
Cost    $0.89

datasheet: https://www.jameco.com/webapp/wcs/stores/servlet/Product_10001_10001_251061_-1

### INA126

datasheet: http://www.ti.com/lit/ds/symlink/ina126.pdf

precision instrumentation amplifier
+ single or double-supply
+ set gain from 5-10,000x with one resistor
+ set the reference voltage with a low-impedance input

"The INAx126 can be used on single power supplies of 2.7 V to 36 V. Use the output REF pin to level shift the internal output voltage into a linear operating condition. Ideally, connecting the REF pin to a potential that is midsupply avoids saturating the output of the amplifiers."

### TL071

datasheet: http://www.ti.com/lit/ds/symlink/tl072.pdf
applications: inverters, audio, oscilloscopes

supply  single / dual
        36 V / ±18 V
type    JFET
slew    13 V/µs
GBP     3 MHz
in_bias 65 pA
in_off  2 nA
in_off  3 mV
price   $0.69

https://www.jameco.com/z/TL072CP-Major-Brands-OP-Amp-Dual-General-Purpose-plusmn-18-Volt-8-Pin-Plastic-Dip-Tube_33195.html

### LM741

datasheet: http://www.ti.com/lit/ds/symlink/lm741.pdf
applications: general-purpose op-amp

supply  dual
        +/- 22V
GBP     1.5 MHz
slew    0.5 V/µs
type    Bipolar
bias    1500 nA
offset  60 nA
in_off  1 mV
resist  2 Mohm
current 25 mA
Cost    $0.49

https://www.jameco.com/z/LM741CN-R-Major-Brands-OP-Amp-Single-General-Purpose-plusmn-18-Volt-8-Pin-Plastic-Dip-Rail_24540.html

### LM358



### LM339 - dedicated comparator

supply    single/dual
          36 V, +/-18 V
response  0.3 µs

datasheet: http://www.ti.com/lit/ds/symlink/lm2901v.pdf

Uses an open collector ("open drain") output that is either connected to ground for disconnected.: https://en.wikipedia.org/wiki/Open_collector

"In practice, using an operational amplifier as a comparator presents several disadvantages as compared to using a dedicated comparator:

+ Op-amps are designed to operate in the linear mode with negative feedback. Hence, an op-amp typically has a lengthy recovery time from saturation. Almost all op-amps have an internal compensation capacitor which imposes slew rate limitations for high frequency signals. Consequently, an op-amp makes a sloppy comparator with propagation delays that can be as long as tens of microseconds.
+ Since op-amps do not have any internal hysteresis, an external hysteresis network is always necessary for slow moving input signals.
+ The quiescent current specification of an op-amp is valid only when the feedback is active. Some op-amps show an increased quiescent current when the inputs are not equal.
+ A comparator is designed to produce well limited output voltages that easily interface with digital logic. Compatibility with digital logic must be verified while using an op-amp as a comparator.
+ Some multiple-section op-amps may exhibit extreme channel-channel interaction when used as comparators.
+ Many op-amps have back to back diodes between their inputs. Op-amp inputs usually follow each other so this is fine. But comparator inputs are not usually the same. The diodes can cause unexpected current through inputs.

via: https://en.wikipedia.org/wiki/Comparator#Op-amp_voltage_comparator

+ http://www.st.com/content/ccc/resource/technical/document/application_note/group0/88/5b/0a/e2/7d/39/4e/9e/DM00050759/files/DM00050759.pdf/jcr:content/translations/en.DM00050759.pdf
+ https://www.maximintegrated.com/en/app-notes/index.mvp/id/886


## Use Cases

### JFET vs Bipolar Inputs

JFET Properties
+ lower input impedances
+ higher input offset voltages
+ easily damaged by static



