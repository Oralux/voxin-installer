#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINE 30
extern char **environ;

int main(int argc, char *argv[])
{
  char line[MAX_LINE];
  char *version = NULL;
  char *module = NULL;
  static char path[PATH_MAX+10];  
  FILE *fd = popen("/usr/bin/speech-dispatcher -v", "r");
  if (fd == NULL) {
    perror("sd_voxin");
    return 1;
  }
  if (!fgets(line, MAX_LINE, fd)) {
    return 2;
  }

  pclose(fd);  	

  if (!realpath(*argv, path)) {
	return 4;
  }

  char *basename = strrchr(path, '/');
  *basename = 0;
  chdir(path);

  version = strrchr(line, ' ');
  if (!version || (strlen(version) < 4))
	return 3;
  version++;
  version[strlen(version)-1] = 0;
  if (!strncmp(version, "0.9", 3)) {
	module = "sd_ibmtts." SD9_0;
  } else if (!strncmp(version, "0.8", 3)) {
    module = (!version[3]) ? "sd_ibmtts." SD8 : "sd_ibmtts." SD8_8;
  } else if (!strncmp(version, "0.7", 3)) {
	module = "sd_ibmtts." SD7_1;
  }

  execve(module, argv, environ);
  
  return 0;
}
