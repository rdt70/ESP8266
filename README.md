## ESP8266 NodeMcu Alarm info
ESP NodeMcu as alarm info detection and sharing on IFTTT. (require IFTTT account and a WebHook + Alarm item)
Also it expose the pin status on a web server. First 3 pins used.
 - D1 used for Alarm Detection 
 - D2 used for Alarm Armed
 - D3 used for Failure (like power off) detection

This prototype is for Lince EURO5K but can be adapted to many other alarm systems

NOTE: the simple wifi client and server has been used instead of HTTP class because the 2nd not working (maybe for memory issue) on NodeMcu

![Web Server](https://github.com/rdt70/ESP8266/blob/master/Images/EURO5K.png)

## Cabling
Power
	20 (AUX) / 21 (GND) -------------- [step-down 12v-3,3v]------------------- Pin 3,3v/GND

Failure
	Ok = not connected / Failure = GND
	22 (OUT) ----------------------------------------------------------------- D4 Pin (INPUT_PULLUP)

Armed
	Nothing = +12v /  Armed -> Not connected [=> require voltage divider]
	32[+OFF] --------- [1KOhm]------+------- [330Ohm] ----- GND [21]
			   12v					|				3v
									+----------------------------------------- D2 Pin (INPUT) [ voltage divider or step-down]
Alarm 
	Nothing = not connected / On Alarm = (NA connected to C) 
	27 (C) 		----------- GND [21]
	26 (NA) 	------------------------------------------------------------ D3 Pin (INPUT_PULLUP)


