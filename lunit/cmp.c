#include "gl/lunit/cmp.h"

#include <string.h>

bool IntCmp(int a, int b) {
  return a == b;
}

bool CharCmp(char a, char b) {
  return a == b;
}

bool StrCmp(char* a, char* b) {
  return strcmp(a, b) == 0;
}

bool StrCmpNot(char* a, char* b) {
  return strcmp(a, b) != 0;
}

bool StrStartsWith(char* str, char* sub_str) {
  if (strlen(str) < strlen(sub_str)) {
    return false;
  }
  return strncmp(str, sub_str, strlen(sub_str)) == 0;
}
