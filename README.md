# LinBPQ Authserver


One Time Passcode server to enable RF only local URL calls. Output from the fetch URL is displayed to the user.


make with 
  ```make```

(Note: you may need to add  libcurl)

  ```sudo apt-get install libcurl4-gnutls-dev```
 


edit 'creds' file with callsign (uppercase) and password
edit 'menu' file with description and the url to be called.

#Menu format

Menu Display Option Text, action url or command, type

type:
	c = CURL
	b = BASH (well, SH)



create a CMDPORT application [BPQ CMDPORT](https://www.cantab.net/users/john.wiseman/Documents/LinBPQ%20Applications%20Interface.html ) and a systemd service/socket pair (files included)

The server takes the file supplied password and adds the 'code' passed to the user.
From the commandline, run the CmdLineAuth program (supplied with LinBPQ (make CmdLineAuth)) and generate a code
with your password and the OTP code appended

e.g. 
	```./CmdLineAuth mypassword12345```


# Usage:

``` BEX:GB7BEX-7} Ok
authserver
*** Connected to AUTHSERVER      
Your code=789383
Enter AuthCode: 400533
Code verified
1. Relays On
2. Relays Off
3. Restart pi
Enter your choice (0 to exit): 2

<a href="https://nextnavigation.com/lease3">Moved Permanently</a>.

1. Relays On
2. Relays Off
3. Restart pi
Enter your choice (0 to exit): 0
Returned to Node BEX:GB7BEX-7
```

G7TAJ@GB7BEX.#38.GBR.EURO
