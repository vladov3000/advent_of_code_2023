#include "basic.h"

typedef enum {
  FIRST,
  LAST,
} State;

static char buffer[4096];

static char ring[5];

static Slice digits[] = {
  slice_from_string("zero"),
  slice_from_string("one"),
  slice_from_string("two"),
  slice_from_string("three"),
  slice_from_string("four"),
  slice_from_string("five"),
  slice_from_string("six"),
  slice_from_string("seven"),
  slice_from_string("eight"),
  slice_from_string("nine"),
};

static void ring_add(char ring[5], char c) {
  for (int i = 0; i < 4; i++)
    ring[i] = ring[i + 1];
  ring[4] = c;
}

static bool ring_ends_with(char ring[5], Slice suffix) {
  for (int i = 0; i < suffix.size; i++)
    if (ring[5 - suffix.size + i] != suffix.data[i])
      return false;
  return true;
}

static int ring_matches_digit(char ring[5]) {
  for (int i = 0; i < array_length(digits); i++)
    if (ring_ends_with(ring, digits[i]))
      return i;
  return -1;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    print_string("Expected 1 argument.\n");
    quit(EXIT_FAILURE);
  }

  char* path = argv[1];
  int   fd   = open(path, O_RDONLY);
  if (fd == -1) {
    print_file_error("open", path, strerror(errno));
    quit(EXIT_FAILURE);
  }

  int     results[2] = {};
  State   states[2]  = { FIRST, FIRST };
  int     lasts[2]   = {};
  ssize_t buffer_filled;
  
  do {
    buffer_filled = read(fd, buffer, sizeof buffer);
    if (buffer_filled == -1) {
      print_file_error("read", path, strerror(errno));
      quit(EXIT_FAILURE);    
    }

    for (ssize_t i = 0; i < buffer_filled; i++) {
      char c = buffer[i];
      switch (states[0]) {
	
      case FIRST:
	if (is_digit(c)) {
	  lasts[0]    = c - '0';
	  results[0] += lasts[0] * 10;
	  states[0]   = LAST;
	}
	break;

      case LAST:
	if (is_digit(c)) {
	  lasts[0] = c - '0';
	} else if (c == '\n') {
	  results[0] += lasts[0];
	  states[0]   = FIRST;
	}
      }

      ring_add(ring, c);
      int digit = ring_matches_digit(ring);
      
      switch (states[1]) {
	
      case FIRST:
	if (is_digit(c) || digit != -1) {
	  lasts[1]    = digit == -1 ? c - '0' : digit;
	  results[1] += lasts[1] * 10;
	  states[1]   = LAST;
	}
	break;

      case LAST:
	if (is_digit(c) || digit != -1) {
	  lasts[1] = digit == -1 ? c - '0' : digit;
	} else if (c == '\n') {
	  results[1] += lasts[1];
	  states[1]   = FIRST;
	}
      }
    }
  } while (buffer_filled > 0);
  
  print_string("Part 1 answer is ");
  print_int(results[0]);
  print_string(".\n");

  print_string("Part 2 answer is ");
  print_int(results[1]);
  print_string(".\n");
  
  quit(EXIT_SUCCESS);
}
