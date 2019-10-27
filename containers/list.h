#ifndef GL_CONTAINERS_LIST_H_
#define GL_CONTAINERS_LIST_H_

#include <stddef.h>

typedef void(*FreeFn)(void*);

typedef struct {
  void* elems;
  size_t elem_size;
  FreeFn free_fn;
  int size;
  int alloc_len;
} List;

void ListInit(List* list, size_t elem_size, FreeFn free_fn);
void ListDispose(List* list);
void ListAdd(List* list, const void* elem);
void* ListGet(List* list, int index);

#endif // GL_CONTAINERS_LIST_H_
