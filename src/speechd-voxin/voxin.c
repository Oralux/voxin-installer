#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "debug.h"


#ifdef DEBUG
#define VOXIND_DBG "/tmp/test_voxind"
#endif

// .../sd_ibmtts.0.7.1-voxin1
#define MODULE_MAX PATH_MAX+30 
static char path[MODULE_MAX];  


static int get_version(char *buf, int max_buf, char **version) {
  int err = 0;
  char *ver = NULL;
  FILE *fd = NULL;

  if (!buf || !version) {
    err = 10;
    goto exit0;
  }

  *version = NULL;

  fd = popen("/usr/bin/speech-dispatcher -v", "r");    
  if (fd == NULL) {
    err = 11;
    goto exit0;
  }

  if (!fgets(buf, max_buf, fd)) {
    err = 12;
    goto exit0;
  }

  ver = strrchr(buf, ' ');
  if (!ver || (strlen(ver) < 4)) {
    err = 13;
    goto exit0;
  }
  ver++;
  ver[strlen(ver)-1] = 0;

  *version = ver;
  
 exit0:
  if (fd)
    pclose(fd);  	

  if (err) {
    dbg("Error %d", err);    
  }

  return err;
}

static int get_module(const char *version, char **module) {
  int err = 0;
  char *m = NULL;
  
  if (!module || !version) {
    err = 20;
    goto exit0;
  }

  m = *module;
  
  if (!strncmp(version, "0.9", 3)) {
    m = "sd_ibmtts." SD9_0;
  } else if (!strncmp(version, "0.8", 3)) {
    m = (!version[3]) ? "sd_ibmtts." SD8 : "sd_ibmtts." SD8_8;
  } else if (!strncmp(version, "0.7", 3)) {
    m = "sd_ibmtts." SD7_1;
  } else {
    err = 21;
    dbg("unexpected version %s", version);
    goto exit0;
  }

  *module = m;
  
 exit0:
  if (err) {
    dbg("Error %d", err);    
  }
  return err;
}

int main(int argc, char *argv[])
{
  ENTER();
#define MAX_BUF 30
  char buf[MAX_BUF];
  char *version = NULL;
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

  err = get_version(buf, MAX_BUF, &version);
  if (err)
    return err;
  
  err = get_module(version, &module);
  if (err)
    return err;
  
  *path = 0;
  if (!realpath("/proc/self/exe", path)) {
    err = 1;
    goto exit0;
  }

  dbg("path=%s", path);
  char *basename = strrchr(path, '/');
  if (!basename) {
    err = 2;
    dbg("path=%s", path);
    goto exit0;
  }
  
  strncpy(basename+1, module, MODULE_MAX - strlen(path));
  dbg("execve %s", path);
  execl(path, path, NULL);  

 exit0:
  if (err) {
    dbg("Error %d", err);    
  }
  return err;
}
