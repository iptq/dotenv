#include <errno.h>
#include <fstream>
#include <iostream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
using namespace std;

extern char **env;

int set_env(map<string, string> *env, string line) {
  size_t eq;
  if ((eq = line.find('=')) == string::npos) {
    return -1;
  }
  (*env)[line.substr(0, eq)] = line.substr(eq + 1);
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <envfile> <cmd>\n", argv[0]);
    return 1;
  }

  ifstream in;
  in.open(argv[1]);
  if (in.fail()) {
    fprintf(stderr, "Could not open file: %s\n", argv[1]);
    return 1;
  }

  string line;
  map<string, string> envdict;
  for (int lc = 0; getline(in, line);) {
    ++lc;
    if (line.size() == 0)
      continue;
    if (set_env(&envdict, line)) {
      fprintf(stderr, "Could not find '=' on non-empty line (ln %d)\n", lc);
    }
  }
  in.close();

  for (char **current = environ; *current; current++) {
    set_env(&envdict, string(*current));
  }

  int nenv = envdict.size();
  char *env[nenv + 1];
  int i, len;
  map<string, string>::iterator it;
  for (i = 0, it = envdict.begin(); it != envdict.end(); ++it, ++i) {
    len = it->first.size() + 2 + it->second.size();
    env[i] = (char *)malloc(len);
    sprintf(env[i], "%s=%s", it->first.c_str(), it->second.c_str());
  }
  env[nenv] = NULL;

  if (execve(argv[2], &argv[2], env) == -1) {
    fprintf(stderr, "Failed to exec '%s'\n", argv[2]);
    fprintf(stderr, "Error: %s\n", strerror(errno));
    return 1;
  }
}