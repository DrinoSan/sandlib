# sandlib

A small C utility library providing common data structures, string handling, checked memory management, and an exception system. Built with `-Wall -Wextra -std=c11`.

## Modules

### Sand_string

Growable dynamic string with capacity management.

```c
#include "Sand_string.h"

Sand_string_t s;
sand_string_create(&s);
sand_string_append(&s, "hello ");
sand_string_append(&s, "world");
sand_string_trim(&s, 0);          // trim whitespace from both sides
sand_string_destroy(&s);
```

**Functions:**

| Function | Description |
|----------|-------------|
| `sand_string_create(s)` | Initialize empty string |
| `sand_string_destroy(s)` | Free memory |
| `sand_string_append(s, str)` | Append null-terminated string |
| `sand_string_append_n(s, str, len)` | Append `len` bytes |
| `sand_string_left_trim(s, ch)` | Trim `ch` from left (0 = whitespace) |
| `sand_string_right_trim(s, ch)` | Trim `ch` from right (0 = whitespace) |
| `sand_string_trim(s, ch)` | Trim both sides |
| `sand_string_left_trim_cstr(str, ch)` | Left trim on raw `char*` |
| `sand_string_right_trim_cstr(str, ch)` | Right trim on raw `char*` |
| `sand_string_trim_cstr(str, ch)` | Trim both sides on raw `char*` |

### Sand_string_view

Lightweight non-owning string view for slicing without copying.

```c
#include "Sand_string_view.h"

char buf[] = "   hello   ";
sand_string_view_t sv = { .data = buf, .size = strlen(buf) };
sand_string_view_trim(&sv, 0);
// sv.data points to 'h', sv.size is 5
```

**Functions:**

| Function | Description |
|----------|-------------|
| `sand_string_view_left_trim(sv, ch)` | Advance `data`, shrink `size` |
| `sand_string_view_right_trim(sv, ch)` | Shrink `size` from end |
| `sand_string_view_trim(sv, ch)` | Trim both sides |

### Sand_vector

Type-generic dynamic array using `void*` and element size.

```c
#include "Sand_vector.h"

sand_list* list = sand_list_create(sizeof(int), NULL);
int v = 42;
sand_list_push_back(list, &v);

int out;
sand_list_get(list, 0, &out);    // out == 42

int* p = sand_list_at(list, 0);  // direct pointer access
*p = 99;                          // in-place modification

sand_list_free(list);
free(list);
```

**Functions:**

| Function | Description |
|----------|-------------|
| `sand_list_create(type_size, freeElem)` | Create empty list |
| `sand_list_push_back(l, e)` | Append element |
| `sand_list_pop_back(l, out)` | Remove and optionally return last element |
| `sand_list_get(l, index, out)` | Copy element at index to `out` |
| `sand_list_at(l, index)` | Get pointer to element at index |
| `sand_list_clear(l)` | Reset size to 0 (keeps capacity) |
| `sand_list_shrink_to_fit(l)` | Reallocate to exact size |
| `sand_list_free(l)` | Free element data (call `free(l)` after) |

For lists of heap-allocated strings, pass `string_free` as the `freeElem` callback.

### Atom

String interning via hash table. Identical strings share the same pointer. Based on the Atom module from David Hanson's *C Interfaces and Implementations*.

```c
#include "atom.h"

const char* a = sand_atom_string("hello");
const char* b = sand_atom_string("hello");
// a == b (same pointer)

const char* c = sand_atom_int(42);
// c points to interned string "42"

int len = sand_atom_length(a);
// len == 5
```

**Functions:**

| Function | Description |
|----------|-------------|
| `sand_atom_string(str)` | Intern a null-terminated string |
| `sand_atom_new(str, len)` | Intern `len` bytes from `str` |
| `sand_atom_int(n)` | Intern the decimal representation of `n` |
| `sand_atom_length(atom)` | Get length of an interned atom |

### Except

Exception handling via `setjmp`/`longjmp`. Hanson's *C Interfaces and Implementations*.

```c
#include "except.h"

sand_except_t My_Error = { "something broke" };

TRY
    RAISE(My_Error);
EXCEPT(My_Error)
    printf("caught it\n");
FINALLY
    printf("cleanup\n");
END_TRY;
```

**Macros:**

| Macro | Description |
|-------|-------------|
| `TRY` | Begin protected block |
| `EXCEPT(e)` | Catch specific exception (pointer identity) |
| `ELSE` | Catch any exception |
| `FINALLY` | Always runs (with or without exception) |
| `END_TRY` | End protected block |
| `RAISE(e)` | Raise an exception |
| `RERAISE` | Re-raise current exception |
| `RETURN` | Return from within a TRY block (pops the stack) |

Exceptions are identified by **pointer**, not by value. Two `sand_except_t` variables with the same reason string are different exceptions.

### Mem

Checked memory allocation. Two implementations behind the same `mem.h` interface:

- **`mem.c`** -- Production. Thin wrappers around malloc/calloc/realloc/free. Raises `sand_mem_failed` on allocation failure.
- **`memchk.c`** -- Debug. Custom pool allocator that tracks every allocation in a hash table. Detects double-free, wild-free, and misaligned-free. Link one or the other, never both.

```c
#include "except.h"
#include "mem.h"

void* p = sand_mem_alloc(100, __FILE__, __LINE__);
// or use the macro:
void* q = ALLOC(100);

sand_mem_free(p, __FILE__, __LINE__);
FREE(q);
```

**Functions:**

| Function | Description |
|----------|-------------|
| `sand_mem_alloc(nbytes, file, line)` | Allocate (never returns NULL) |
| `sand_mem_calloc(count, nbytes, file, line)` | Allocate zeroed |
| `sand_mem_resize(ptr, nbytes, file, line)` | Resize allocation |
| `sand_mem_free(ptr, file, line)` | Free allocation |

**Macros:** `ALLOC(n)`, `CALLOC(count, n)`, `NEW(p)`, `FREE(ptr)`, `RESIZE(ptr, n)`

Swap between production and debug at **link time** -- no code changes needed:

```sh
# debug build (tracks allocations, catches double-free)
gcc -o myapp main.o memchk.o except.o

# release build (minimal overhead)
gcc -o myapp main.o mem.o except.o
```

## Building

```sh
make          # build libsand.a
make test     # build and run all unit tests
make clean    # remove build artifacts
```

## Testing

Tests use the [Unity](https://github.com/ThrowTheSwitch/Unity) framework (cloned into `tests/unity/`).

```
tests/
  test_sand_string.c       27 tests
  test_sand_string_view.c  15 tests
  test_sand_vector.c       20 tests
  test_atom.c              23 tests
  test_except.c            17 tests
  test_mem.c                9 tests
  test_memchk.c            13 tests
```

Run with `make test`. Output is color-coded (green = pass, red = fail).

## References

- David R. Hanson, *C Interfaces and Implementations: Techniques for Creating Reusable Software* (Addison-Wesley, 1996)
