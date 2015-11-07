#include <stdio.h>
#include <string.h>
#include "Blocks.h"

Blocks cipher(char* m, int size) {
    Blocks e = Blocks(m, size);

    return e;
}

void print_hex(char* s, int n) {
    int i = 0;
    for (i = 0; i < n; i++) {
        if (s[i] >= 16) {
            printf("%x", s[i] & 0xff);
        } else {
            printf("0%x", s[i] & 0xf);
        }
    }
}

int main(int argc, char *argv[]) {
    if( argc == 2 ) {
      char* m = argv[1];
      printf("Message text:  \"%s\"\n", argv[1]);
      
      printf("Message bytes: \"");
      print_hex(m, strlen(m));
      printf("\"\n");

      Blocks e = cipher(m, strlen(m)); 

      printf("Cipher:        \"");
      for (auto *b : e.getBlocks()) {
        print_hex(b, BLOCK_SIZE);
      }
      printf("\"\n");
    }

    return 0;
}
