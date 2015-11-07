#include <stdlib.h>
#include <string.h>
#include <vector>

using std::vector;

#define BLOCK_SIZE 8 // in bytes!!!
#define KEY_SIZE 32

class Blocks {
private:
    vector<char*> content;
    bool padded;
    
    char* pad_message(char* m, int size);
    vector<char*> split_message(char* m, int size); 

public:
    Blocks(char* m, int size);
    ~Blocks();

    vector<char*> getBlocks();
};

