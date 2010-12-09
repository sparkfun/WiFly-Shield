== SparkFun WiFly Shield Library : alpha 1 release ==

This is a library for the Arduino-compatible WiFly Shield available from
SparkFun Electronics:

  <http://www.sparkfun.com/commerce/product_info.php?products_id=9367> 

The goal with this library is to make it--as much as possible--a "drop
in" replacement for the official Arduino Ethernet library
<http://www.arduino.cc/en/Reference/Ethernet>. Once a wireless network
is joined the library should respond in the same way as the Ethernet
library. This means you should be able to take existing Ethernet
examples and make them work wirelessly without too many changes.

The library also provides a high-level interface for the "SC16IS750
I2C/SPI-to-UART IC" used in the WiFly shield but also available on a
separate breakout board:

   <http://www.sparkfun.com/commerce/product_info.php?products_id=9745> .


= Usage =

This is how you connect to a wireless network and use DHCP to obtain
an IP address and DNS configuration:

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

From then on you can use the Client and Server classes (re-implemented
for the WiFly) mostly as normal.

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


= Known Issues =

This is an alpha release--this means it's non-feature complete and may
not be entirely reliable. It has been tested with the shipped examples and
works in most cases.

There are some known issues:

 * Only supports WPA networks with passwords. If you have a network
   without a passphrase then it should work if you supply a dummy
   passphrase. If you have a WEP network you should probably change it
   to use WPA instead. :) If that's not an option then it should be
   possible to change the library code to set the WEP key rather than
   a WPA passphrase.

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
