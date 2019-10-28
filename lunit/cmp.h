#ifndef GL_LUNIT_CMP_H_
#define GL_LUNIT_CMP_H_

#include <stdbool.h>
#include <stddef.h>

bool IntCmp(int a, int b);
void IntEqMsg(char* msg, int a, int b);

bool CharCmp(char a, char b);
void CharEqMsg(char* msg, char a, char b);

bool StrCmp(char* a, char* b);
bool StrCmpNot(char* a, char* b);
bool StrStartsWith(char* str, char* sub_str);
void StrEqMsg(char* msg, char* a, char* b);
void StrNEqMsg(char* msg, char* a, char* b);
void StrStartsWithMsg(char* msg, char* a, char* b);

bool MemCmp(void* a, void* b, size_t size);
void MemEqMsg(char* msg, void* a, void* b, size_t size);

#endif // GL_LUNIT_CMP_H_
