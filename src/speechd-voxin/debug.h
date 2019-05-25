#ifndef __DEBUG_H_
#define __DEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

  // log enabled if this file exits
#define ENABLE_LOG "/tmp/libvoxin.ok"

  // log level; first byte equals to a digit in DebugLevel (default 
#define LIBVOXINLOG "/tmp/libvoxin0.log.%d"

  enum DebugLevel {LV_ERROR_LEVEL=0, LV_INFO_LEVEL=1, LV_DEBUG_LEVEL=2, LV_LOG_DEFAULT=LV_ERROR_LEVEL};

#define log(level,fmt,...) if (DebugEnabled(level)) {DebugDisplayTime(); fprintf (myDebugFile, "%s: " fmt "\n", __func__, ##__VA_ARGS__);}
#define err(fmt,...) log(LV_ERROR_LEVEL, fmt, ##__VA_ARGS__)
#define msg(fmt,...) log(LV_INFO_LEVEL, fmt, ##__VA_ARGS__)
#define dbg(fmt,...) log(LV_DEBUG_LEVEL, fmt, ##__VA_ARGS__)

#define ENTER() dbg("ENTER")
#define LEAVE() dbg("LEAVE")

  // compilation error if condition is not fullfilled (inspired from
  // BUILD_BUG_ON, linux kernel).
#define BUILD_ASSERT(condition) ((void)sizeof(char[(condition)?1:-1]))

  extern int DebugEnabled(enum DebugLevel level);
  extern void DebugFileInit();
  extern void DebugFileFinish();
  extern void DebugDisplayTime();
  extern void DebugDump(const char *label, uint8_t *buf, size_t size);

  extern FILE *myDebugFile;
  


#ifdef __cplusplus
}
#endif

#endif

