#include "Blocks.h"

Blocks::Blocks(char* m, int size) {
    this->padded = false;
    this->content = split_message(m, size);
}

Blocks::~Blocks() {
    if (this->padded) {
        int n = this->content.size();
        free(this->content[n-1]);
    }
}

vector<char*> Blocks::getBlocks() {
    return this->content;
}

char* Blocks::pad_message(char* m, int size) {
    char* bytes = new char[BLOCK_SIZE];
    strcpy(bytes, m);
    if (size < BLOCK_SIZE) {
        this->padded = true;
        for (int i = size; i < BLOCK_SIZE; i++) {
            bytes[i] = 0;
        }
    }
    return bytes;
}

vector<char*> Blocks::split_message(char* m, int size) {
    int n;
    if (size % BLOCK_SIZE == 0) {
        n = size / BLOCK_SIZE;
    } else {
        n = size / BLOCK_SIZE + 1;
    }

    vector<char*> result;
    result.resize(n);
    char* p = m; 
    int i;
    for (i = 0; i < size / BLOCK_SIZE; i++) {
        result[i] = p;
        p += BLOCK_SIZE;
    }
    
    if (n != size / BLOCK_SIZE) {
        result[n-1] = pad_message(p, strlen(p));
    }

    return result;
}
