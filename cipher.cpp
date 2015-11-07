#include <stdio.h>
#include <string.h>
#include "Blocks.h"
#include <time.h>

#define SHIFT_SIZE 11
#define SHIFT_MASK 0xffe00000

const char s_table[] = {
    13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
    10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
     2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11,
    15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,
     1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
     7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
     4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
    12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11
};

char substitute(int i, int j) {
    return s_table[i*8 + j];
}

void iteration(char* b, int k, char i) {
    int* n1 = (int*)b;
    int* n2 = (int*)(b + BLOCK_SIZE / 2);
    
    int s = (*n1) + k;

    int temp = 0; 
    int mask = 0xf0000000;
    int j = (s & mask) >> (4 * 7);
    j &= 0x0000000f;
//    printf("s = %x; mask = %x; j = %x;\n", s, mask, j);
    mask = 0x0f000000;
    for (int i = 1; i < 8; i++) {
        j = (s & mask) >> (4 * (7 - i)); 
        //printf("s = %x; mask = %x; j = %x;\n", s, mask, j);
        
        temp |= substitute(i, j);
        mask >>= 4; mask &= 0x0fffffff;
    }

    temp = s & SHIFT_MASK;
    s = (s << SHIFT_SIZE) | (temp >> (32 - SHIFT_SIZE));
    s = s ^ (*n2);
    *n2 = *n1;
    *n1 = s;
}

Blocks cipher(char* m, int size, vector<int> key) {
    Blocks e = Blocks(m, size);
    for (auto *b : e.getBlocks()) {
        for (int k = 0; k < 3; k++) {
            for (int i = 0; i < 8; i++) {
                iteration(b, key[i], i);
            }
        }
        for (int i = 7; i >= 0; i--) {
                iteration(b, key[i], i);
        }
    } 
    return e;
}

vector<int> gen_key() {
    srand(time(NULL));
    vector<int> result;
    result.resize(8);
    for (int i = 0; i < 8; i++) {
        result[i] = 0xffeeddaa;
    }
    return result;
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

      Blocks e = cipher(m, strlen(m), gen_key()); 

      printf("Cipher:        \"");
      for (auto *b : e.getBlocks()) {
        print_hex(b, BLOCK_SIZE); printf(" ");
      }
      printf("\"\n");
    }

    return 0;
}
