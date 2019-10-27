#include "gl/lunit/cmp.h"

#include <string.h>

int IntCmp(int a, int b) {
  return a - b;
}

int CharCmp(char a, char b) {
  return a - b;
}

int StrCmp(char* a, char* b) {
  return strcmp(a, b);
}

int StrCmpNot(char* a, char* b) {
  return !strcmp(a, b);
}
