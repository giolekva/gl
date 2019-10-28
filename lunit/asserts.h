#ifndef GL_LUNIT_ASSERTS_H_
#define GL_LUNIT_ASSERTS_H_

#include <stdio.h>
#include <string.h>

// TODO(giolekva): line info
#define __ASSERT_EQ(cmp, msg, ...) {					\
    if (!(cmp)(__VA_ARGS__)) {						\
      test->success = false;			\
      (msg)(test->error, __VA_ARGS__);					\
      LOG_INFO(test->error);						\
      return;								\
    }									\
  }

#define ASSERT_INT_EQ(actual, expected) \
  __ASSERT_EQ(IntEq, IntEqMsg, actual, expected)

#define ASSERT_CHAR_EQ(actual, expected) \
  __ASSERT_EQ(CharEq, CharEqMsg, actual, expected)

#define ASSERT_STR_EQ(actual, expected) \
  __ASSERT_EQ(StrEq, StrEqMsg, actual, expected)

#define ASSERT_STR_NEQ(actual, expected) \
  __ASSERT_EQ(StrNEq, StrNEqMsg, actual, expected)

#define ASSERT_STR_STARTS_WITH(str, sub_str) \
  __ASSERT_EQ(StrStartsWith, StrStartsWithMsg, str, sub_str)

#define ASSERT_MEM_EQ(actual, expected, size)	\
  __ASSERT_EQ(MemEq, MemEqMsg, actual, expected, size)

#endif // GL_LUNIT_ASSERTS_H_
