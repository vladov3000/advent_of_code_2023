#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define false ((bool) 0)
#define true  ((bool) 1)

#define array_length(array) (sizeof(array) / sizeof((array)[0]))
#define slice_from_string(string) (Slice) { .data = (string), .size = sizeof(string) - 1 }

typedef unsigned long long u64;
typedef _Bool              bool;

typedef struct {
  char* data;
  u64   size;
} Slice;

static bool is_digit(char c) {
  return '0' <= c && c <= '9';
}

static char print_buffer[4096];
static int  print_buffered;

static void flush() {
  write(STDOUT_FILENO, print_buffer, print_buffered);
  print_buffered = 0;
}

static void print(char* message, u64 size) {
  if (size > sizeof print_buffer - print_buffered) {
    write(STDOUT_FILENO, print_buffer, print_buffered);
    write(STDOUT_FILENO, message, size);
    print_buffered = 0;
  } else {
    memcpy(&print_buffer[print_buffered], message, size);
    print_buffered += size;
  }
}

static void print_char(char c) {
  print(&c, 1);
}

static void print_string(char* string) {
  print(string, strlen(string));
}

static void print_int(int n) {
  char storage[11];
  int  index    = sizeof storage;
  bool negative = n < 0;

  if (negative)
    n = -n;

  do
    storage[--index] = n % 10 + '0';
  while (n /= 10);

  if (negative)
    storage[--index]  = '-';

  print(&storage[index], sizeof storage - index);
}

static void print_file_error(char* operation, char* path, char* error) {
  print_string("Error while ");
  print_string(operation);
  print_string("ing \"");
  print_string(path);
  print_string("\": ");
  print_string(error);
  print_string(".\n");
}

static void quit(int exit_code) {
  flush();
  _Exit(exit_code);
}
