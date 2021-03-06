#include "gl/lunit/cmp.h"

#include <stdio.h>
#include <string.h>

bool BoolEq(bool actual, bool expected) {
  return actual == expected;
}

void BoolEqMsg(char* msg, bool actual, bool expected) {
  sprintf(msg, "Expected %s got %s instead.",
	  expected ? "true" : "false",
	  actual ? "true" : "false");
}

bool IntEq(int a, int b) {
  return a == b;
}

void IntEqMsg(char* msg, int actual, int expected) {
  sprintf(msg, "Expected %d got %d instead.", expected, actual);
}

bool CharEq(char a, char b) {
  return a == b;
}

void CharEqMsg(char* msg, char actual, char expected) {
  sprintf(msg, "Expected %c got %c instead.", expected, actual);
}

bool StrEq(char* a, char* b) {
  return strcmp(a, b) == 0;
}

void StrEqMsg(char* msg, char* actual, char* expected) {
  sprintf(msg, "Expected %s got %s instead.", expected, actual);
}

bool StrNEq(char* a, char* b) {
  return strcmp(a, b) != 0;
}

void StrNEqMsg(char* msg, char* actual, char* expected) {
  sprintf(msg, "Did not expect %s.", expected);
}

bool StrStartsWith(char* str, char* sub_str) {
  if (strlen(str) < strlen(sub_str)) {
    return false;
  }
  return strncmp(str, sub_str, strlen(sub_str)) == 0;
}

void StrStartsWithMsg(char* msg, char* str, char* sub_str) {
  sprintf(msg, "%s does not start with %s.", str, sub_str);
}

bool MemEq(void* a, void* b, size_t size) {
  return memcmp(a, b, size) == 0;
}

void MemEqMsg(char* msg, void* a, void* b, size_t size) {
  sprintf(msg, "MEM NOT EQUAL");
}
