#include <iostream>
#include <stack>
#include <fstream>
#include <sstream>
#include <bits/stdc++.h>

using namespace std;

#define HORIZONTAL false
#define VERTICAL true

struct Block {
    int width;
    int height;
    bool orientation = HORIZONTAL; // false = horizontal, true = vertical 
};

int main(int argc, char * argv[]) {
    if (argc < 2 || argc > 3) {
        printf("usage: hw2 <in_file> <out_file>");
        exit(1);
    }
    char * inFile = argv[1];
    char * outFile = argv[2];

    // Handling Polish Expression
    // Open the input file
    ifstream inputFP(inFile);
    if (!inputFP.is_open()) {
        cout << "Could not open file " << inFile << endl;
        exit(1);
    }

    // number of blocks
    int N; 
    inputFP >> N;

    // Array to store blocks
    Block blocks[N]; 

    // Read block dimensions
    for (int i = 0; i < N; i++) {
        inputFP >> blocks[i].width >> blocks[i].height;
    }

    // Read the Polish expression (last line)
    string line;
    string polishExpression;
    while (getline(inputFP, line)) {
        polishExpression = line;
    }
    inputFP.close();

    cout << polishExpression << endl;
    stack<string> expressionStack;
    string tmp[2];
    string combined;
    for (char c : polishExpression) {
        if (std::isspace(c)) {
            continue; // Skip spaces
        }
        if (c == '+' || c == '*') {
            tmp[1] = expressionStack.top();
            expressionStack.pop();
            tmp[0] = expressionStack.top();
            expressionStack.pop();
            combined = "[" + tmp[0] + tmp[1] + string(1,c) + "]";
            expressionStack.push(combined);
        } else {
            expressionStack.push(string(1,c));
        }
    }

    string tmpoutputstring;
    while (!expressionStack.empty()) {
        tmpoutputstring += expressionStack.top();
        expressionStack.pop();
    }

    cout << tmpoutputstring;

    return 0;
}
