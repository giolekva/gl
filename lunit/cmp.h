#ifndef GL_LUNIT_CMP_H_
#define GL_LUNIT_CMP_H_

#include <stdbool.h>

bool IntCmp(int a, int b);

bool CharCmp(char a, char b);

bool StrCmp(char* a, char* b);

bool StrCmpNot(char* a, char* b);

bool StrStartsWith(char* str, char* sub_str);

#endif // GL_LUNIT_CMP_H_
