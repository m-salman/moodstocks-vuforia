#ifndef MSV_WHICHOS_H
#define MSV_WHICHOS_H

#if (defined(__APPLE__))
  #include <TargetConditionals.h>
  #if TARGET_OS_IPHONE
    #define __MSV_SYS_IOS__
  #endif
#endif

#endif
