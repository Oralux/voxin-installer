#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include "debug.h"


#ifdef DEBUG
#define VOXIND_DBG "/tmp/test_voxind"
#endif

#define STR_MAX 128

// .../0.9.1/x86_64/sd_voxin
#define MODULE_MAX (PATH_MAX + STR_MAX)

typedef enum {
  SD_VOX_OK=0,
  SD_VOX_ARGS_ERROR,
  SD_VOX_SYS_ERROR,
  SD_VOX_UNEXPECTED_DATA,
  SD_VOX_INTERNAL_ERROR,  
} sd_vox_error;

static int get_speech_dispatcher_version(const char **version) {
  int err = SD_VOX_OK;
  char *s = NULL;
  FILE *fd = NULL;
  static char buf[STR_MAX];

  ENTER();
  
  if (!version) {
    err = SD_VOX_ARGS_ERROR;
    dbg("args error");
    goto exit0;
  }

  *version = NULL;

  fd = popen("/usr/bin/speech-dispatcher -v", "r");    
  if (fd == NULL) {
    err = SD_VOX_SYS_ERROR;
    dbg("sys error");
    goto exit0;
  }

  if (!fgets(buf, sizeof(buf), fd)) {
    err = SD_VOX_UNEXPECTED_DATA;
    dbg("unexpected version");
    goto exit0;
  }

  s = strrchr(buf, ' ');
  if (!s || (strlen(s) < 4)) {
    err = SD_VOX_UNEXPECTED_DATA;
    dbg("unexpected version");
    goto exit0;
  }
  s++;
  s[strlen(s)-1] = 0;
  *version = s;
  
 exit0:
  if (fd)
    pclose(fd);  	

  return err;
}

// obtain the absolute path of the voxin module compatible with the
// current speech-dispatcher version
static int get_voxin_module(const char *version, char **module) {
  int err = SD_VOX_OK;
  struct utsname buf;
  static char path[MODULE_MAX];
  size_t len;
  
  ENTER();

  if (!version || !module) {
    err = SD_VOX_ARGS_ERROR;
    dbg("args error");
    return err;
  }

  if (!strncmp(version, "0.7", 3))
    version = "0.7.1";
  else if (!strncmp(version, "0.8", 3) && version[3]) {
    version = "0.8.8";
  } else if (!strncmp(version, "0.10", 4)) {
    version = "0.10.1";
  }
  
  *path = 0;
  if (!realpath("/proc/self/exe", path)) {
    int res = errno;
    err = SD_VOX_SYS_ERROR;
    dbg("realpath error: %s", strerror(res));
    return err;
  }

  dbg("path=%s", path);

  {
    char *basename = strrchr(path, '/');
    if (!basename) {
      dbg("/ not found!");
      return SD_VOX_UNEXPECTED_DATA;
    }
    *basename = 0;
  }
  len = strlen(path);
  len += snprintf(path+len, sizeof(path)-len, "/%s/sd_voxin", version);
  if (len >= sizeof(path)) {
    dbg("error: path length=%u", (unsigned int)len);
    return SD_VOX_INTERNAL_ERROR;
  }
  
  *module = path;  
  dbg("module=%s", path);
  return err;
}

int main(int argc, char *argv[])
{
  ENTER();
  const char *version = NULL;
  char *module = NULL;
  int err = 0;

#ifdef DEBUG
  {
    struct stat buf;
    while (!stat(VOXIND_DBG, &buf)) {
      sleep(1);
    }
  }
#endif

  err = get_speech_dispatcher_version(&version);
  if (err) {
    // version identification failed.
    // try the last supported version
    version = "last";
    err = SD_VOX_OK;
  }
  
  err = get_voxin_module(version, &module);
  if (err)
    return err;
  
  { // pass all arguments to the voxin module
    void **arg = calloc(1, (argc+1)*sizeof(*argv));
    if (arg) {
      memcpy(arg, argv, argc*sizeof(*argv));
      dbg("execve %s", module);
      execv(module, (char* const *)arg);
    }
    free(arg);
  }
  
  return SD_VOX_UNEXPECTED_DATA;
}
