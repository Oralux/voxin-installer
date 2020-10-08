#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>

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

#define MAX_CHAR 256

typedef struct {
  int major;
  int minor;
  int patch;
  char str[MAX_CHAR];
} version_t;

static int get_version(struct dirent *d, version_t *version) {
  char *start, *end;
  int i;
  
  ENTER();

  if (!d || !version) {
    int err = EINVAL;
    dbg("%s", strerror(err));
    return err;
  }
  
  memset(version, 0, sizeof(*version));
  memcpy(version->str, d->d_name, MAX_CHAR);
  version->str[MAX_CHAR-1] = 0;

  // numerical chars expected optionally separated by '.'
  for (i=0; version->str[i]; i++) {
    if (!isdigit(version->str[i]) && (version->str[i] != '.')) {
      dbg("unexpected version: %s", version->str);
      return EINVAL;
    }
  }  
  
  start = version->str;
  end = strchr(start, '.');
  if (!end) {
    version->major = atoi(start);
    goto exit0;
  }
  
  *end = 0;
  version->major = atoi(start);
  *end = '.';
  start = end+1;
  
  end = strchr(start, '.');
  if (!end) {
    version->minor = atoi(start);
    goto exit0;
  }

  *end = 0;
  version->minor = atoi(start);
  *end = '.';
  start = end+1;

  version->patch = atoi(start);
  
 exit0:
  dbg("version=(%d,%d,%d), str=%s", version->major, version->minor, version->patch, version->str);
  return 0;
}

static int get_last_version(const char *dir_path, version_t *last) {
  int err = EINVAL;
  const int max_loop = 100;
  int loop = 0;
  DIR *dir;

  if (!dir_path || !last) {
    err = EINVAL;
    goto exit0;
  }

  dbg("ENTER: %s", dir_path);

  memset(last, 0, sizeof(*last));
  dir = opendir(dir_path);
  if (!dir) {
    err = errno;
    goto exit0;
  }

  do {
    version_t v;
    struct dirent *d = readdir(dir);
    if (!d)
      break;
    
    if (!get_version(d, &v)) {
      if ((v.major > last->major)
	  || ((v.major == last->major)
	      && ((v.minor > last->minor)
		  || ((v.minor == last->minor)
		      && (v.patch > last->patch))))) {
	memcpy(last, &v, sizeof(*last));
	dbg("new last=(%d,%d,%d), str=%s", last->major, last->minor, last->patch, last->str);
	err = 0;
      }
    }
    
    loop++;
  } while(loop < max_loop);

 exit0:
  if (err) {
    dbg("%s", strerror(err));
  }
  return err;
}

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

  *module = NULL;
  
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

  dbg("version=%s, path=%s", version, path);

  {
    char *basename = strrchr(path, '/');
    if (!basename) {
      dbg("/ not found!");
      return SD_VOX_UNEXPECTED_DATA;
    }
    *basename = 0;
  }

  if (!strcmp(version, "last")) {
    version_t last;
    err = get_last_version(path, &last);
    if (err)
      return err;
    
    version = last.str;
  }
  
  len = strlen(path);
  len += snprintf(path+len, sizeof(path)-len, "/%s/sd_voxin", version);
  if (len >= sizeof(path)) {
    dbg("error: path length=%u", (unsigned int)len);
    return SD_VOX_INTERNAL_ERROR;
  }

  {
    struct stat statbuf;
    if (stat(path, &statbuf)) {
      int err = errno;
      dbg("%s", strerror(err));
      return SD_VOX_SYS_ERROR;
    }

    if ((statbuf.st_mode & S_IFMT) != S_IFREG) {
      dbg("type error: %s", path);
      return SD_VOX_UNEXPECTED_DATA;
    }
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
  bool last = false;

#ifdef DEBUG
  {
    struct stat buf;
    while (!stat(VOXIND_DBG, &buf)) {
      sleep(1);
    }
  }
#endif

  err = get_speech_dispatcher_version(&version);
  if (err)
    {
    // version identification failed.
    // try the last supported version
    version = "last";
    last = true;
    err = SD_VOX_OK;
  }
  
  err = get_voxin_module(version, &module);
  if (err && !last) {
    version = "last";
    last = true;
    err = get_voxin_module(version, &module);
  }
  
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
