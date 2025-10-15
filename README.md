# Simple Mark-and-Sweep Garbage Collector in ![C]([https://search.brave.com/images?q=c+anguage+logo&context=W3sic3JjIjoiaHR0cHM6Ly93d3cubmljZXBuZy5jb20vcG5nL2RldGFpbC85ODEtOTgxMzE3NV9jLWxvZ28tYy1wcm9ncmFtbWluZy1sYW5ndWFnZS1sb2dvLnBuZyIsInRleHQiOiJDIExvZ28gLSBDKysgUHJvZ3JhbW1pbmcgTGFuZ3VhZ2UgTG9nbywgdHJhbnNwYXJlbnQgcG5nIGRvd25sb2FkIiwicGFnZV91cmwiOiJodHRwczovL3d3dy5uaWNlcG5nLmNvbS9vdXJwaWMvdTJ5M3c3YTlpMW8wcjVyNV9jLWxvZ28tYy1wcm9ncmFtbWluZy1sYW5ndWFnZS1sb2dvLyJ9XQ%3D%3D&sig=0966ad36a3b27d51a8e72b00abbb7182f1f999db81dd3ee0ca00d4220d961af5&nonce=75ddf258b3c10364273e63c8c4f22557&source=imageCluster](https://images.seeklogo.com/logo-png/45/1/c-language-logo-png_seeklogo-458623.png))

## Overview
This project implements a simple mark-and-sweep garbage collector in C, designed to manage memory for a virtual machine (VM) that handles integers and pairs of objects. The garbage collector automatically reclaims memory for objects that are no longer reachable from the VM's stack, using the classic mark-and-sweep algorithm.

## Mark-and-Sweep Algorithm
The mark-and-sweep garbage collector operates in two phases:
1. **Mark Phase**: Starting from the VM's stack (the "roots"), it traverses all reachable objects and marks them as "live." This ensures that any object still in use is identified.
2. **Sweep Phase**: It scans the heap and frees any objects that were not marked as live, as these are considered unreachable and safe to reclaim.

The collector supports two types of objects:
- **Integers**: Simple values stored as objects.
- **Pairs**: Objects that reference two other objects (e.g., `(left, right)`), allowing for nested data structures.

## Code Structure
The main components of the implementation are:
- **VM Structure**: Represents the virtual machine, maintaining a stack of objects and a linked list of all allocated objects on the heap.
- **Object Structure**: Represents either an integer or a pair, with a `marked` flag to track reachability during garbage collection.
- **Garbage Collection Functions**:
  - `markAll(VM* vm)`: Marks all objects reachable from the stack by recursively traversing pairs.
  - `sweep(VM* vm)`: Frees unmarked objects and updates the heap.
  - `gc(VM* vm)`: Combines mark and sweep, triggered either manually or when the heap size exceeds a threshold (`gcSize`).

## Usage
The provided `main.c` demonstrates the garbage collector's functionality through a series of tests:
- **Test 1**: Allocates five integers and runs the garbage collector. Since all objects are on the stack, none are collected.
- **Test 2**: Pops two integers from the stack, making them unreachable, and runs the garbage collector to reclaim them.
- **Test 3**: Creates nested pairs (e.g., `((1, 2), 3)`) and runs the garbage collector. All objects remain reachable, so none are collected.
- **Test 4**: Pops a top-level pair, making it and its nested objects unreachable, then runs the garbage collector to reclaim them.
- **Test 5**: Allocates many integers to exceed the `gcSize` limit, triggering automatic garbage collection.

### Example Code
```c
int main() {
  VM *vm = newVM();

  // Test 1: Simple int allocation
  for (int i = 0; i < 5; i++) {
    pushInt(vm, i);
  }
  gc(vm); // None collected — all are on stack

  // Test 2: Pop some values (unreachable)
  pop(vm);
  pop(vm);
  gc(vm); // Should collect 2

  // Test 3: Create pairs
  pushInt(vm, 1);
  pushInt(vm, 2);
  pushPair(vm); // Creates (1, 2)
  pushInt(vm, 3);
  pushPair(vm); // Creates ((1, 2), 3)
  gc(vm); // Nothing collected yet

  // Test 4: Make unreachable pairs
  pop(vm); // Pop top-level ((1, 2), 3)
  gc(vm); // Should collect the popped pair and its sub-pairs

  // Test 5: Force GC by exceeding gcSize limit
  for (int i = 0; i < 20; i++) {
    pushInt(vm, i);
  }

  freeVM(vm);
  return 0;
}
```

## How to Run
1. Compile the program using a C compiler (e.g., `gcc main.c -o gc`).
2. Run the executable (e.g., `./gc`).
3. The program will output the results of each test, showing the stack contents and garbage collection activity.

## Limitations
- The garbage collector assumes all objects are either integers or pairs. Extending it to support other types would require additional logic.
- The heap size limit (`gcSize`) is hardcoded, but it could be made configurable.
- The implementation is simplified for educational purposes and may not be optimized for production use.
