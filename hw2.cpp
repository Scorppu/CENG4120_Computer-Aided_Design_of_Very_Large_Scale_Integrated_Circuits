#include <iostream>

using namespace std;

int main(int argc, char * argv[])
    {
    if (argc < 2 || argc > 3) {
        printf("usage: hw2 <in_file> <out_file>");
        exit(1);
    }
    char * inFile = argv[1];
    char * outFile = argv[2];
}
