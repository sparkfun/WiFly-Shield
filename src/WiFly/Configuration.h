
#define SHIELD_REVISION 3

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
