# ESP8266
ESP NodeMcu as alarm info detection and sharing on IFTTT. 
Also it expost the pin status on a web server. First 3 pins used.
D1 indicate Alarm Detection 
D2 indicate Alarm Armed
D3 Failure (like power off) detection

This prototype is for Lynce EURO5K but can be adapted to many other alarm systems

NOTE: the simple wifi client and server has been used instead of HTTP class because the 2nd not working (maybe for memory issue) on NodeMcu
