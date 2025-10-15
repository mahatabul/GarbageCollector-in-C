#include <stdio.h>
#include <stdlib.h>
#define STACK_MAX_SIZE 100
#define gcSize 10

typedef enum { OBJ_INT, OBJ_PAIR } ObjectType;

void assert(int condition, const char *message) {
  if (!condition) {
    printf("%s\n", message);
    exit(1);
  }
}
typedef struct sObject {
  ObjectType type;
  struct sObject *next;
  unsigned char markbit;
  union {
    /* OBJ_INT */
    int value;

    /* OBJ_PAIR */
    struct {
      struct sObject *head;
      struct sObject *tail;
    };
  };
} Object;

typedef struct {
  Object *firstObject;
  int numObjects;
  int maxObjects;

  Object *stack[STACK_MAX_SIZE];
  int stack_size;
} VM;

VM *newVM() {
  VM *vm = malloc(sizeof(VM));
  vm->stack_size = 0;
  vm->firstObject = NULL;
  vm->numObjects = 0;
  vm->maxObjects = gcSize;
  return vm;
}
void push(VM *vm, Object *value) {
  assert(vm->stack_size < STACK_MAX_SIZE, "Stack Overflow!!");
  vm->stack[vm->stack_size++] = value;
}
Object *pop(VM *vm) {
  assert(vm->stack_size > 0, "Stack Underflow!");
  return vm->stack[--vm->stack_size];
}
void mark(Object *object) {
  if (object->markbit)
    return;

  object->markbit = 1;

  if (object->type == OBJ_PAIR) {
    mark(object->head);
    mark(object->tail);
  }
}

void markAll(VM *vm) {
  for (int i = 0; i < vm->stack_size; i++) {
    mark(vm->stack[i]);
  }
}

void sweep(VM *vm) {
  Object **obj = &(vm->firstObject);
  while (*obj) {
    if (!(*obj)->markbit) {

      Object *unreached = *obj;
      *obj = unreached->next;
      free(unreached);
      vm->numObjects--;
    } else {

      (*obj)->markbit = 0;
      obj = &(*obj)->next;
    }
  }
}
void gc(VM *vm) {
  int numObjects = vm->numObjects;
  markAll(vm);
  sweep(vm);
  vm->maxObjects = vm->numObjects == 0 ? gcSize : vm->numObjects * 2;
  printf("Collected Objects: %d, remaining Objects: %d \n",
         numObjects - vm->numObjects, vm->numObjects);
}
Object *newObject(VM *vm, ObjectType type) {
  if (vm->numObjects >= vm->maxObjects) {
    gc(vm);
  }
  Object *obj = malloc(sizeof(Object));
  obj->type = type;
  obj->next = vm->firstObject;
  vm->firstObject = obj;
  vm->numObjects++;
  return obj;
}

void pushInt(VM *vm, int intValue) {
  Object *object = newObject(vm, OBJ_INT);
  object->value = intValue;
  push(vm, object);
}

Object *pushPair(VM *vm) {
  Object *object = newObject(vm, OBJ_PAIR);
  object->tail = pop(vm);
  object->head = pop(vm);

  push(vm, object);
  return object;
}

void objPrint(Object *obj) {
  switch (obj->type) {
  case OBJ_INT:
    printf("%d", obj->value);
    break;

  case OBJ_PAIR:
    printf("(");
    objPrint(obj->head);
    printf(", ");
    objPrint(obj->tail);
    printf(")");
    break;
  }
}

void freeVM(VM *vm) {
  vm->stack_size = 0;
  gc(vm);
  free(vm);
}


int main() {
  VM *vm = newVM();

  printf("Test 1: Simple int allocation\n");
  for (int i = 0; i < 5; i++) {
    pushInt(vm, i);
  }
  gc(vm); // none collected — all are on stack
  printf("\n");

  printf("Test 2: Pop some values (unreachable)\n");
  pop(vm);
  pop(vm);
  gc(vm); // should collect 2
  printf("\n");

  printf("Test 3: Create pairs\n");
  pushInt(vm, 1);
  pushInt(vm, 2);
  pushPair(vm); // creates (1, 2)
  pushInt(vm, 3);
  pushPair(vm); // creates ((1, 2), 3)
  gc(vm); // nothing collected yet
  printf("\n");

  printf("Stack contents:\n");
  for (int i = 0; i < vm->stack_size; i++) {
    objPrint(vm->stack[i]);
    printf("\n");
  }
  printf("\n");

  printf("Test 4: Make unreachable pairs\n");
  pop(vm); // pop top-level ((1, 2), 3)
  gc(vm); // should collect the popped pair (and its sub-pairs if unreferenced)
  printf("\n");

  printf("Test 5: Force GC by exceeding gcSize limit\n");
  for (int i = 0; i < 30; i++) {
    pushInt(vm, i);
  }
  printf("\n");

  freeVM(vm);
  return 0;
}

