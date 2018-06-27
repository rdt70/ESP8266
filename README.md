# ESP8266
ESP NodeMcu as alarm info detection and sharing on IFTTT. (require IFTTT account and a WebHook + Alarm item)
Also it expose the pin status on a web server. First 3 pins used.
 - D1 used for Alarm Detection 
 - D2 used for Alarm Armed
 - D3 used for Failure (like power off) detection

This prototype is for Lince EURO5K but can be adapted to many other alarm systems

NOTE: the simple wifi client and server has been used instead of HTTP class because the 2nd not working (maybe for memory issue) on NodeMcu

![Test Image](https://github.com/KnisterPeter/vscode-github/raw/master/images/create-pull-request.png)

## Test Title
