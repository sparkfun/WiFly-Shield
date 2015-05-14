SparkFun WiFly Shield Arduino Library
==========================================

This is a library for the [SparkFun WiFly Shield](https://www.sparkfun.com/products/9954)

The goal with this library is to make it--as much as possible--a "drop in" replacement for the official [Arduino Ethernet library](http://www.arduino.cc/en/Reference/Ethernet). Once a wireless network is joined the library should respond in the same way as the Ethernet library. This means you should be able to take existing Ethernet examples and make them work wirelessly without too many changes.

The library also provides a high-level interface for the [SC16IS750 I2C/SPI-to-UART IC](http://www.sparkfun.com/products/9981) used in the WiFly shield but also available on a separate [breakout board](https://www.sparkfun.com/products/9981).


Repository Contents
-------------------

* **/examples** - Example sketches for the library (.ino). Run these from the Arduino IDE. 
* **/extras** - Additional documentation for the user. These files are ignored by the IDE. 
* **/src** - Source files for the library (.cpp, .h).
* **library.properties** - General library properties for the Arduino package manager. 

Documentation
--------------

* **[Installing an Arduino Library Guide](https://learn.sparkfun.com/tutorials/installing-an-arduino-library)** - Basic information on how to install an Arduino library.
* **[Product Repository](https://github.com/sparkfun/WiFly-Shield)** - Main repository (including hardware files) for the WiFly Shield.
* **[Hookup Guide](https://learn.sparkfun.com/tutorials/wifly-shield-hookup-guide)** - Basic hookup guide for the WiFly Shield. 


Installation
------------

A good resource to start is the [Arduino Hacking Libraries](http://www.arduino.cc/en/Hacking/Libraries) article which goes in depth about how to install libraries. There are two ways to install this library, it can either be directly downloaded and unzipped or it can be cloned using Git.

### Step 1: Check the libraries directory
For both methods, you'll need to check that you have a shared library
directory. It should be under the Arduino home (e.g. in OSX, it will probably be `~/Documents/Arduino/libraries`).
If the `libraries` directory doesn't already exist, you'll need to create it.

### Step 2a: Add the library via Git
Clone this project to your favorite project folder.

    git clone git@github.com:sparkfun/WiFly-Shield.git

If you've never used Git before, check out the [Git Community Book](http://book.git-scm.com/).

Now create a symbolic link From your Libraries folder to the newly created WiFly_Shield directory of the freshly cloned project.

    ln -s ~/path/to/project/Libraries/WiFly_Shield ~/Documents/Arduino/libraries/Wifly_Shield (Mac)

### Step 2b: Add the library via Archived File
Download and unzip the zip file from this GitHub project and put the
contents in the `libraries` directory.

At the end of Step 2, you should have a copy of this project at
`~/Documents/Arduino/libraries/WiFly`

### Step 3: Include the library in your project
Restart the Arduino IDE if you're using it. Under the `Sketch` menu, you
should see a `WiFly` item in the `Import Library ...` option. If you
don't, recheck that the library is in the right location and that you've
restarted the Arduino IDE. Right now it adds a lot more then needed.

You can also manually add the library by adding the following to the top
of your sketch.

```c
#include <SPI.h>
#include <WiFly.h>
```

Usage
-----

This is how you connect to a WPA wireless network with a passphrase
and use DHCP to obtain an IP address and DNS configuration:

```c
#include <SPI.h>
#include <WiFly.h>

void setup() {

  WiFly.begin();
  
  if (!WiFly.join("ssid", "passphrase")) {
     // Handle the failure
  }
  
  // Rejoice in your connection
}
```

If the network you want to connect to has no passphrase you can use this form:

```c
if (!WiFly.join("ssid")) {
  // Handle the failure
}
```

If the network you want to connect to is using WEP use this form:

```c
if (!WiFly.join("NETWORK", "00112233445566778899AABBCC", WEP_MODE)) {
  // Handle the failure
}
```

Note the description of the WEP key from the WiFly user guide:

 * Key must be EXACTLY 26 ASCII characters representing 13 bytes.
 * In HEX format, hex digits > 9 can be either upper or lower case.
 * "The Wifly GSX only supports “open” key mode, 128 bit keys for WEP."

Whatever connection method you use, once you have joined you can use
the Client and Server classes (re-implemented for the WiFly) mostly as
normal.

You can supply a domain name rather than an IP address for client
connections:

```c
Client client("google.com", 80);
```

You can also retrieve the current IP address with:

```c
Serial.println(WiFly.ip());
```

This release of the library comes with three examples:

  * WiFly_Autoconnect_Terminal: reimplementation from tutorial 
  * WiFly_WebClient: Ethernet WebClient demo with small WiFly changes
  * WiFly_WebServer: Ethernet WebServer demo with small WiFly changes

For each example you will need to modify the file "Credentials.h" to
supply your network's name (SSID) and passphrase.

There are also some troubleshooting tools:

  * SpiUartTerminal: enter command mode and send commands manually
  * HardwareFactoryReset: hardware factory reset a WiFly module

Configuration
-------------

Different revisions of the WiFly shield support different features. If
you are using an older revision of the shield you will need to modify
the value of 'SHIELD_REVISION' in the file 'Configuration.h' to
indicate which revision of the WiFly shield you are using. See the
documentation in the file for further detail.

The value defaults to the most recent revision sold at the time of
code release.

Arduino Mega support
--------------------

This library supports using the WiFly Shield with the Arduino Mega if
four jumper wires are added. The following connections are required:

 * Mega pin 53 to shield pin 10
 * Mega pin 51 to shield pin 11
 * Mega pin 50 to shield pin 12
 * Mega pin 52 to shield pin 13

In addition, code on the Mega must not use pins 10, 11, 12, or 13.

Known Issues
------------

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
