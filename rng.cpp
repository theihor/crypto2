#include "cipher.cpp"

void xor_n_bytes(char* from, char* to, int n) {
    //cout << endl;
    for (int i = 0; i < n; i++) {
        //cout << (int)from[i] << " xor " << (int)to[i] << " = ";
        //cout << hex << (int)from[i] << " xor " << (int)to[i] << " = ";
        to[i] = (from[i]) ^ (to[i]);
        //cout << hex << (int)to[i] << endl;
    }
}

void xor_bytes(char* from, char* to) {
    xor_n_bytes(from, to, BLOCK_SIZE);
}

void copy_n_bytes(char* from, char* to, int n) {
    for (int i = 0; i < n; i++) {
        to[i] = from[i];
    }
}

void copy_bytes(char* from, char* to) {
    copy_n_bytes(from, to, BLOCK_SIZE);
}

void put_timestamp(char* x) {
    time_t t = time(NULL);
    copy_bytes((char*)&t, x);
    int r = rand();
    xor_bytes((char*)&r, x);
    xor_bytes((char*)&r, x + 4);

}

// cyclic shift left
char csl(char x, int n) {
    for (int i = 0; i < n; i++) {
        char c = x & 0x80;
        c <<= 7;
    }
}

void inverse(char* x) {
    for (int i = 0; i < BLOCK_SIZE; i++) {
        x[i] ^= 0xff;
        if (x[i] & 0x01) {
            char c = x[i] & 0x03;
            x[i] <<= 2;
            x[i] |= c;
        } else {
            x[i] >>= 1;
        }
    }
}

char* S = new char[BLOCK_SIZE];
char* D = new char[BLOCK_SIZE];
char* I;
vector<int> K;

void init_S(char* fname) {
    ifstream ifs(fname, ios::binary|ios::ate);
	ifstream::pos_type pos = ifs.tellg();
    int n = pos;
	char* input = new char[n];
	ifs.seekg(0, ios::beg);
	ifs.read(&input[0], n);
	
    for (int i = 0; i < BLOCK_SIZE; i++) S[i] = 0;

    for (int i = 0; i < n; i++) {
        S[i % BLOCK_SIZE] ^= input[i];
    }

    ifs.close();
}

void init_rng(char* fname) {
    //cout << "RNG initialization" << endl;
    time_t t = time(NULL);
    char* keyword = new char[3];
    strcpy(keyword, "RNG");
    K = gen_key(keyword);
    //long l_S = 0x1122334455667788;
    //copy_bytes((char*)&l_S, S);
    init_S(fname);
    cout << "S = "; print_hex(S, BLOCK_SIZE); cout << endl;
    put_timestamp(D);
    cout << "D = "; print_hex(D, BLOCK_SIZE); cout << endl;
    I = cipher(D, BLOCK_SIZE, K); // I = f(D, K)
    cout << "I = "; print_hex(I, BLOCK_SIZE); cout << endl << endl;
}

void free_rng() {
    delete D;
    delete I;
    delete S;
}

char* f(char* m) {
    int n = 2;
    char* message = new char[BLOCK_SIZE * n];
    for (int i = 0; i < n; i++) {
        copy_bytes(m, message + BLOCK_SIZE * i);
    }
    char* e = cipher(message, BLOCK_SIZE * n, K);

    char* res = new char[BLOCK_SIZE];
    copy_bytes(e + BLOCK_SIZE * (n - 1), res);
    
    delete e;
    delete message;

    return res;
}

char gen_bit(int index) {

    char* R = new char[BLOCK_SIZE];
    copy_bytes(I, R); // R = I
    xor_bytes(S, R); // R = I xor S
    //cout << "R = I xor S = "; print_hex(R, BLOCK_SIZE); cout << endl;

    char* X = f(R); // X = f(R, k)
    //cout << "X = f(R, K) = "; print_hex(X, BLOCK_SIZE); cout << endl;
    
    char* T = new char[BLOCK_SIZE];
    copy_bytes(X, T);
    xor_bytes(I, T); // T = X xor I
    //cout << "T = X xor I = "; print_hex(T, BLOCK_SIZE); cout << endl;
    
    S = f(T); // S = f(T, K)
    //cout << "S = f(T, K) = "; print_hex(S, BLOCK_SIZE); cout << endl;
    
    int i = index / 8;
    index %= 8;
   
    char mask = 1;
    mask <<= index;

    char bit = X[i] & mask;
    if (index == 7) {
        bit >>= 1;
        bit &= 0x7f;
        bit >>= index - 1;
    } else {
        bit >>= index;
    }

    delete R;
    delete X;
    delete T;

    return bit;
}

void gen_file(char* fname, int n, int index) {
    ofstream ofs (fname, ios::binary | ios::trunc | ios::out);
    for (int i = 0; i < n; i++) {
        char c = 0;
        for (int j = 0; j < BLOCK_SIZE; j++) {
            char bit = gen_bit(index);
            c <<= 1;
            c |= bit;
        }
        ofs << c;
    }
    ofs.close();
}

int main(int argc, char *argv[]) {

    if (argc == 5) {
        cout << "index = " << atoi(argv[4]) << endl;
        init_rng(argv[1]);
        gen_file(argv[2], atoi(argv[3]), atoi(argv[4]));


        /*
           char* e = cipher(S, BLOCK_SIZE, K);
           xor_bytes(I, e);
           cout << "f(s) xor i = "; print_hex(e, BLOCK_SIZE); cout << endl;
           xor_bytes(I, S);
           e = cipher(S, BLOCK_SIZE, K);
           cout << "f(s xor i) = "; print_hex(e, BLOCK_SIZE); cout << endl;
         */

        //cout << "t = " << t << endl;
        //cout << "sizeof(time_t) = " << sizeof(time_t) << endl;
        /*
        for (int i = 0; i < 16; i++) {
            cout << (int)gen_bit();
            //cout << endl;
        }
        cout << endl;
        */
        free_rng();
    }
}


