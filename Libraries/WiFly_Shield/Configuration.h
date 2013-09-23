/*

  Configuration.h -- configuration of different board features

  Different revisions of the WiFly shield support different
  features. If you are using an older revision of the shield you will
  need to modify the value of 'SHIELD_REVISION' below to indicate
  which revision of the WiFly shield you are using.

  The value defaults to the most recent revision sold at the time of
  code release.


  How to identify which shield revision you have
  ----------------------------------------------

  * Revision 3 (Define SHIELD_REVISION as the value 3)

    + <http://www.sparkfun.com/products/9954>

    + Has a date code "6/15/10" on the underside of the PCB near the
      mounting hole nearest the WiFly module.

    + Has the ~14MHz crystal and hardware reset support.


  * Revision 2 (Define SHIELD_REVISION as the value 2)

    + <http://www.sparkfun.com/products/9367> (Same page as revision 1.)

    + Is identified by having the ~14MHz crystal (rectangular metal
      device near pins 6 & 7 with "14." engraved on it) and *not* having
      a date code on the underside of the PCB.

    + Has the ~14MHz crystal but no hardware reset support.

   
  * Revision 1 (Define SHIELD_REVISION as the value 1)

    + <http://www.sparkfun.com/products/9367> (Same page as revision 2.)

    + Is identified by having the *~12MHz* crystal (rectangular metal
      device near pins 6 & 7 with "12." engraved on it) and *not* having
      a date code on the underside of the PCB.

    + Has the ~12MHz crystal and no hardware reset support.
  
 */

#define SHIELD_REVISION 3 // Change this value to match your shield revision


/* -- Do not change anything below this line -- */

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#if SHIELD_REVISION == 1

#define USE_14_MHZ_CRYSTAL false
#define USE_HARDWARE_RESET false

#else
#if SHIELD_REVISION == 2

#define USE_14_MHZ_CRYSTAL true
#define USE_HARDWARE_RESET false

#else
// Default to Revision 3 (lol)

#define USE_14_MHZ_CRYSTAL true
#define USE_HARDWARE_RESET true

#endif // Revision 2 
#endif // Revision 1

#endif
