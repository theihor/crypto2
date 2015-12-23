#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

#define BLOCK_SIZE 8 // in bytes!!!
#define KEY_SIZE 32

#define SHIFT_SIZE 11
#define SHIFT_MASK 0xffe00000


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

void print_(char* s, int n) {
    int i = 0;
    for (i = 0; i < n; i++) {
        printf("%c", s[i]);
    }
}

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

void generate(char* x) { /*
    int i = 1;
    while (i < 3) {
        char c = x[i];
        x[i] = x[BLOCK_SIZE - i];
        x[BLOCK_SIZE - i] = c;
        i += 1 + c % 2;
    }
    */
}

char* generate_init(vector<int> k, char* m) {

    char* x = new char[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_SIZE; i++) {
        x[i] = (substitute(i, 0) << 4) | substitute(i, 3);
    }
    x[4] = x[4] ^ 0x04;
    /*
    char* p1 = (char*)&(k[3]);

    for (int i = 0; i < 8; i++) {
        if (i % 2 == 0) {
            x[i] = p1[i];
        } else {
            x[i] = m[i];
        }
    }

    generate(x);
    */
    return x;
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

char* cipher(char* m, int size, vector<int> key) {
    char* e = new char[size];
    
    char* init = generate_init(key, m);
   // printf("Generated:     \"");
   // print_hex(init, BLOCK_SIZE);
   // printf("\"\n");
   
    int b = 0;
    while (b + BLOCK_SIZE <= size) {
        for (int k = 0; k < 3; k++) {
            for (int i = 0; i < 8; i++) {
                iteration(init, key[i], i);
            }
        }
        for (int i = 7; i >= 0; i--) {
            iteration(init, key[i], i);
        }
        
        for (int i = 0; i < BLOCK_SIZE; i++) {
            e[b + i] = init[i] ^ m[b + i];
            init[i] = e[b + i];
        }

        b += BLOCK_SIZE;
        generate(init); 
    } 

    if (size % BLOCK_SIZE != 0) {
       for (int k = 0; k < 3; k++) {
            for (int i = 0; i < 8; i++) {
                iteration(init, key[i], i);
            }
        }
        for (int i = 7; i >= 0; i--) {
            iteration(init, key[i], i);
        }
        
        for (int i = 0; i < size - (size / BLOCK_SIZE) * BLOCK_SIZE; i++) {
            e[b + i] = init[i] ^ m[b + i];
        }

    }

    delete[] init;

    return e;
}

char* decrypt(char* e, int size, vector<int> key) {
    char* m = new char[size];
    
    char* init = generate_init(key, m);
   // printf("Generated:     \"");
   // print_hex(init, BLOCK_SIZE);
   // printf("\"\n");
   
    int b = 0;
    while (b + BLOCK_SIZE <= size) {
        for (int k = 0; k < 3; k++) {
            for (int i = 0; i < 8; i++) {
                iteration(init, key[i], i);
            }
        }
        for (int i = 7; i >= 0; i--) {
            iteration(init, key[i], i);
        }
        
        for (int i = 0; i < BLOCK_SIZE; i++) {
            m[b + i] = init[i] ^ e[b + i];
            init[i] = e[b + i];
        }

        b += BLOCK_SIZE;
        generate(init); 
    } 

    if (size % BLOCK_SIZE != 0) {
       for (int k = 0; k < 3; k++) {
            for (int i = 0; i < 8; i++) {
                iteration(init, key[i], i);
            }
        }
        for (int i = 7; i >= 0; i--) {
            iteration(init, key[i], i);
        }
        
        for (int i = 0; i < size - (size / BLOCK_SIZE) * BLOCK_SIZE; i++) {
            m[b + i] = init[i] ^ e[b + i];
        }

    }

    delete[] init;

    return m;
}

vector<int> gen_key(char* keyword) {
    srand(time(NULL));
    vector<int> result;
    result.resize(8);
    int n = strlen(keyword);
    if (n < KEY_SIZE) {
        int k = 0;
        
        for (int i = 0; i < n / 4; i += 4) {
            int x = 0;
            for (int i = 0; i < 8; i++) {
                x |= substitute(k,i); 
                x <<= 4;
            }
            int *p = (int*)&keyword[i];
            result[k] = (*p) ^ x;
            k++;
        }
        while(k < 8) {
            int x = 0;
            for (int i = 0; i < 8; i++) {
                x |= substitute(k,i); 
                x <<= 4;
            }
            //printf("x = %d\n", x);
            result[k] = x;
            //printf("r[k] = %d\n", result[k]);
            k++;
        }
    } else {
        for (int i = 0; i < KEY_SIZE; i += 4) {
            int *p = (int*)&keyword[i];
            result[i/4] = *p;
        }
    }
    //for (int i = 0; i < 8; i++) {
    //    result[i] = 0x4321abcd;
    //}
    return result;
}



int main(int argc, char *argv[]) {
    char* input;
    char* keyword;

    if( argc == 4 ) {
        keyword = argv[1];
	
	    ifstream ifs(argv[2], ios::binary|ios::ate);
	    ifstream::pos_type pos = ifs.tellg();
            int n = pos;
	    char* input = new char[n];

	    ifs.seekg(0, ios::beg);
	    ifs.read(&input[0], n);
	    
            char* e = cipher(input, n, gen_key(keyword));
	    
	    ofstream ofs (argv[3], ios::binary | ios::trunc | ios::out);
	    for (int i = 0; i < n; i++) {
	        ofs << e[i];
	    }
	    ofs.close();
    }   
    
    if( argc == 5 ) {
        if (strcmp(argv[1], "-d") == 0) {
            keyword = argv[2];
            ifstream ifs(argv[3], ios::binary|ios::ate);
	        ifstream::pos_type pos = ifs.tellg();
	        int n = pos;
	        char* input = new char[n];

	        ifs.seekg(0, ios::beg);
	        ifs.read(&input[0], n);
	    
            char* m = decrypt(input, n, gen_key(keyword));
	    
	        ofstream ofs (argv[4], ios::binary | ios::trunc | ios::out);
	        for (int i = 0; i < n; i++) {
		        ofs << m[i];
	        }
	        ofs.close();
	    
            //print_(message, n);
        }
    }




    if( argc == 3 ) {
      char* m = argv[2];
      vector<int> key = gen_key(argv[1]);
      int size = strlen(m);
    
      printf("Generated key: \"");
      for (int i : key) {
          printf("%x ", i);
      }
      printf("\"\n");
    
      printf("Message text:  \"%s\"\n", m);
      
      printf("Message bytes: \"");
      print_hex(m, size);
      printf("\"\n");

      char* e = cipher(m, size, key); 

      printf("Cipher:        \"");
      print_hex(e, size); //printf(" ");
      printf("\"\n");

      printf("Decrypted hex: \"");
      char* decryption = decrypt(e, size, key);
      print_hex(decryption, size); //printf(" ");
      printf("\"\n");
      printf("Decrypted:     \"");
      print_(decryption, size); //printf(" ");
      printf("\"\n");


    }

    return 0;
}
