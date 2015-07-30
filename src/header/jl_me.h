// Version Definitions
#define JL_VERSION "4.0.0-e1"

//Platform Declarations
#define JL_PLAT_COMPUTER 0 //PC/MAC
#define JL_PLAT_PHONE 1 //ANDROID/IPHONE
#if defined(__IPHONEOS__) || defined(__ANDROID__)
        #define JL_PLAT JL_PLAT_PHONE
#else
        #define JL_PLAT JL_PLAT_COMPUTER
#endif
