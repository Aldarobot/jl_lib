// Version Definitions
#define JL_VERSION "4.0.0e4"
/*
 * Version System:
 * 	major version "." minor version "." debug version "."
 *
 *	A new major version is made every time the current API changes.
 *	A new minor version is made every time new API is added.
 *	A new debug version is made for every release without API changes.
 *	A new "e#" version is made every commit that's not released.
 */

//Platform Declarations
#define JL_PLAT_COMPUTER 0 //PC/MAC
#define JL_PLAT_PHONE 1 //ANDROID/IPHONE
#if defined(__IPHONEOS__) || defined(__ANDROID__)
        #define JL_PLAT JL_PLAT_PHONE
#else
        #define JL_PLAT JL_PLAT_COMPUTER
#endif
