#ifndef GL_LUNIT_ASSERTS_H_
#define GL_LUNIT_ASSERTS_H_

#include <stdio.h>
#include <string.h>

#define __ASSERT_EQ(actual, expected, cmp, type, format) {		\
    type exp = (expected);						\
    type act = (actual);						\
    char* form = (format);						\
    if (!(cmp)(exp, act)) {						\
      test->success = false;						\
      char fmt[100];							\
      sprintf(fmt, "Expected %s Got %s (%%s) instead.", form, form);	\
      LOG_ERROR(fmt, exp, act, #actual);				\
      sprintf(fmt, "%%s:%%d: Expected %s Got %s (%%s) instead.", form, form); \
      sprintf(test->error, fmt,  __FILE__, __LINE__, exp, act, #actual); \
      return;								\
    }									\
  }

#define ASSERT_INT_EQ(actual, expected) \
  __ASSERT_EQ(actual, expected, IntCmp, int, "%d")

#define ASSERT_CHAR_EQ(actual, expected) \
  __ASSERT_EQ(actual, expected, CharCmp, char, "'%c'")

#define ASSERT_STR_EQ(actual, expected) \
  __ASSERT_EQ(actual, expected, StrCmp, char*, "\"%s\"")

#define ASSERT_STR_NEQ(actual, expected) \
  __ASSERT_EQ(actual, expected, StrCmpNot, char*, "\"%s\"")

#define ASSERT_STR_STARTS_WITH(actual, expected) \
  __ASSERT_EQ(actual, expected, StrStartsWith, char*, "\"%s\"")

#endif // GL_LUNIT_ASSERTS_H_
