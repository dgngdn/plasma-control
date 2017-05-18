0.050 µF


ƒ = 1 / 2π*sqrt(LC)

LC = (0.0001/2π)**2 = 2.4674011002723393e-08

C = 0.5 µF = 0.0000005
therefore...
L = 0.04934802200544679 H = 49 mH

----
what are mkp4 capacitors?


via: http://4hv.org/e107_plugins/forum/forum_viewtopic.php?84628.0#post_84666

"MKT and MKP are completely different dielectric materials, with very different high frequency loss factors (dissipation factor = dielectric heating at high frequencies).

MKT represents a metallized polyester film dielectric. This material (one trade name is Mylar) is fine for use at 50-60 Hz, but it can quickly overheat and fail at 100-400 KHz, a range of frequencies typical of small-medium Tesla coils.

MKP represents a metallized polypropylene film dielectric, which has much lower loss factor than MKT and can be suitable for use at higher frequencies.

However, in addition to having the correct low-loss dielectric, a capacitor's ability to handle high pulse current is crucial to being able to survive in a TC tank circuit. Unfortunately, most of the commonly-encountered "X capacitors" use metallized film construction, which cannot handle really high pulse currents. What you really want are capacitors using polypropylene dielectric with foil electrodes, like the CDE 942 series, the WIMA FKP series, etc. The "F" in FKP represents "foil" electrodes, vs the "M" representing metallized film in MKP capacitors.

Capacitors using foil electrodes can handle much higher pulse currents than metallized film electrodes can. The MKT caps will overheat internally and die, and with both the MKT and MKP capacitors the end connections between the leads and the edges of the metallized film can burn away and the caps will die. 

Regards,
Herr Zapp"


"Also, there are of course different types of MKP capacitor. WIMA, for example, has the MKP4 and the MKP10 series, with the latter being more suitable for pulse applications (altough they are obviously not as strong as the FKP series).

I have succesfully used MKP4 caps in a small SSTC halfbridge and will be using MKP10 as coupling capacitors for my next full bridge. However, since I have no DRSSTC experience yet, I can't say how they will perform in an LC tank circuit. I'm guessing it all comes down to the power level you're planning on using..."
----

via: http://www.teslacoildesign.com/construction.html

"It's important to use the correct type of caps in the MMC. Most caps will quickly fail when used in a Tesla coil. Look for these qualities in a good MMC cap:
	
	Polypropylene type caps
	Metal "foil" type, especially foil electrodes
	High dV/dT rating (min of 1000 - 2000 V/uS)
	High RMS current capacity (min of 10 - 15 Amps)
	High peak current capacity (min of several hundred amps)
	Self healing ability"

----
Buying capacitors:
http://www.mouser.com/wima/

505-FKP4.47/1KV/10    1000V, 0.47µF   $7.94
505-FKP41.0/1KV/10    1000V, 1.00µF   $17.31
505-FKP4.22/1600/10   1600V, 0.22µF   $7.58
505-FKP4.33/1600/10   1600V, 0.33µF   $11.73
505-FKP4.47/1600/10   1600V, 0.47µF   $15.24

Digikey does NOT have WIMA, but they do have Kemet, Epcos

Kemet R76 Series:
  Kemet R76QR3220SE30K  1000V, 0.22µF   $2.03
  Automotive; High Frequency, Switching; High Pulse, DV/DT
  http://www.kemet.com/Lists/ProductCatalog/Attachments/348/KEM_F3034_R76.pdf
  "Typical applications include deflection circuits in televisions
(S-correction and flyback tuning) and monitors, switching spikes
suppression in switched mode power supplies (SMPS), lamp
capacitors for electronic ballasts and compact lamps, and
snubber and silicon-controlled rectifier (SCR) commutation
circuits as well as applications with high voltage and high current.
Not suitable for across-the-line application (see Suppressor
Capacitors)."

MKP metalized film polypropylene capacitors are really for interference suppression and "across the line" applications

 - 


