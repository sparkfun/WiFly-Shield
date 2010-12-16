== SparkFun WiFly Shield Library : alpha 1 release ==

This is a library for the Arduino-compatible WiFly Shield available from
SparkFun Electronics:

  <http://sparkfun.com/products/9954> 

The goal with this library is to make it--as much as possible--a "drop
in" replacement for the official Arduino Ethernet library
<http://www.arduino.cc/en/Reference/Ethernet>. Once a wireless network
is joined the library should respond in the same way as the Ethernet
library. This means you should be able to take existing Ethernet
examples and make them work wirelessly without too many changes.

The library also provides a high-level interface for the "SC16IS750
I2C/SPI-to-UART IC" used in the WiFly shield but also available on a
separate breakout board:

   <http://www.sparkfun.com/products/9981>


= Usage =

This is how you connect to a WPA wireless network with a passphrase
and use DHCP to obtain an IP address and DNS configuration:

----
#include "WiFly.h"

void setup() {

  WiFly.begin();
  
  if (!WiFly.join("ssid", "passphrase")) {
     // Handle the failure
  }
  
  // Rejoice in your connection
}
---

If the network you want to connect to has no passphrase you can use this form:

---
  if (!WiFly.join("ssid")) {
     // Handle the failure
  }
---

If the network you want to connect to is using WEP use this form:

---
  if (!WiFly.join("NETWORK", "00112233445566778899AABBCC", WEP_MODE)) {
     // Handle the failure
  }
---

Note the description of the WEP key from the WiFly user guide:

 * Key must be EXACTLY 26 ASCII characters representing 13 bytes.

 * In HEX format, hex digits > 9 can be either upper or lower case.
 
 * "The Wifly GSX only supports “open” key mode, 128 bit keys for WEP."

Whatever connection method you use, once you have joined you can use
the Client and Server classes (re-implemented for the WiFly) mostly as
normal.

You can supply a domain name rather than an IP address for client
connections:

  Client client("google.com", 80);

You can also retrieve the current IP address with:

  Serial.println(WiFly.ip());

This release of the library comes with three examples:

  * WiFly_Autoconnect_Terminal: reimplementation from tutorial 
  * WiFly_WebClient: Ethernet WebClient demo with small WiFly changes
  * WiFly_WebServer: Ethernet WebServer demo with small WiFly changes

For each example you will need to modify the file "Credentials.h" to
supply your network's name (SSID) and passphrase.

There are also some troubleshooting tools:

  * SpiUartTerminal: enter command mode and send commands manually
  * HardwareFactoryReset: hardware factory reset a WiFly module


= Configuration =

Different revisions of the WiFly shield support different features. If
you are using an older revision of the shield you will need to modify
the value of 'SHIELD_REVISION' in the file 'Configuration.h' to
indicate which revision of the WiFly shield you are using. See the
documentation in the file for further detail.

The value defaults to the most recent revision sold at the time of
code release.


= Known Issues =

This is an alpha release--this means it's non-feature complete and may
not be entirely reliable. It has been tested with the shipped examples and
works in most cases.

There are some known issues:

 * Connections to WEP networks have not really been tested--please try
   it out and provide feedback. At the moment adhoc networks of any
   type are not supported--the module supports them, the library just
   hasn't been modified to recognise the different way the module
   responds when connecting.

 * Incomplete documentation.

 * Only tested with WiFly firmware version 2.18--earlier or later
   versions may or may not have issues. 2.20 has also been tested.

 * Only DHCP is supported--you can't specify an IP address and DNS
   configuration directly.

 * There are some situations (exact cause unknown but often it seems
   to be after initial programming) where the WiFly will fail to
   respond to requests. You may need to power-cycle the Arduino or try
   refreshing the page in your browser if it's acting as a server.

 * There's a limit to how quickly you can refresh a page when acting
   as a server--this is because the library doesn't handle dropped
   connections well at present. You can generally tell from the lights
   on the unit if it's busy. (This is particularly obvious when a
   using a web browser (rather than something like 'wget') because
   after the page is loaded the browser makes an immediate request for
   the favicon. Once every five seconds or so should be fine depending
   on how big the page is.

 * None of the non-ethernet capabilities of the WiFly are yet exposed
   e.g. network scans, signal strength information etc.

 * The code isn't very robust for error states--in general it will
   hang rather than return useful information.

 * We only have a 9600 baud connection between the Arduino and WiFly
   it should in theory be possible to be much faster.

 * Passphrases or SSIDs that contain spaces or dollar signs ($) will
   probably not work.


= License & Authors =

See LICENSE.TXT


= Feedback =

Please email <spark@sparkfun.com> or leave a comment on the SparkFun forums:

  <http://forum.sparkfun.com/>


= Changelog =

+ alpha 2 -- 17 December 2010 -- "Azalea Galaxy"

  * NOTE: New configuration location! You now need to modify the value
    	  of 'SHIELD_REVISION' in the file 'Configuration.h' to
    	  indicate what revision of the WiFly shield you are
    	  using. See the documentation in the file for further
    	  detail. The value defaults to the most recent revision sold
    	  at the time of the code release.

  * Added support for joining Open networks (i.e. without a passphrase or key).

  * Added (untested) WEP network support.

  * Modified software reboot to hopefully work more reliably.

  * Added support for hardware reset which should be more reliable than
    software reboot on board revisions that support it (currently only the
    most recent revision).

  * Modified command mode entry method to hopefully work more
    reliably.  Includes use of guard time as originally inspired by
    World Mood Light.

  * Added 'SpiUartTerminal' troubleshooting tool.

  * Added 'HardwareFactoryReset' tool to help with troubleshooting.

  * Added support for further board revision feature support configuration.

  * Added some debugging support.


+ alpha 1 -- 31 August 2010 -- "August Gratitude"

  * Change default crystal speed to match new 14MHz crystal used.
    NOTE: People using the older 12MHz crystal will need to change
          "USE_14_MHZ_CRYSTAL" in SpiUart.cpp to 'false'.

  * Added ability to set baud rate at runtime in SpiUart class.
    (Note: This ability to change the baudrate has not yet been propagated
    to the WiFly class.)

  * Renamed Spi.h to _Spi.h in order to avoid clashes with the other
    library by the same name as the IDE might (and has) mistakenly
    included ours instead of the the other one which results in
    confusing error messages.

  * Changed the case of the 'examples' directory so it gets displayed
    in the IDE correctly.


+ alpha 0 -- 19 May 2010 -- "Awfully Gorgeous"
  *  Initial release
