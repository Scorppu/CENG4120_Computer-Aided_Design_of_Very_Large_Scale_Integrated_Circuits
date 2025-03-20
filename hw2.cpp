#include <iostream>
#include <stack>
#include <fstream>
#include <sstream>
#include <bits/stdc++.h>

using namespace std;

#define HORIZONTAL false
#define VERTICAL true

struct Module {
    char id;
    int width = 0;
    int height = 0;
    bool orientation = HORIZONTAL; // false = horizontal, true = vertical
    int coord;
    Module* left_child = nullptr;
    Module* right_child = nullptr;
};

Module* parseAndBuildTree(const string& expression, Module modules[], int N) {
    stack<Module*> moduleStack;
    for (char c : expression) {
        if (isspace(c)) {
            continue; // Skip spaces
        } else if (isdigit(c)) {
            // Find the module corresponding to the character
            int index = c - '0';
            modules[index].id = c;
            moduleStack.push(&modules[index]);
        } else if (c == '+' || c == '*') {
            Module* rightChild = moduleStack.top();
            moduleStack.pop();
            Module* leftChild = moduleStack.top();
            moduleStack.pop();
            
            // Create a new parent module
            Module* parent = new Module(); // Initialize width and height later
            parent->id = c;
            parent->left_child = leftChild;
            parent->right_child = rightChild;
            
            // Determine orientation based on operator
            // if (c == '+') {
            //     parent->orientation = HORIZONTAL;
            // } else {
            //     parent->orientation = VERTICAL;
            // }
            
            // Calculate width and height of the parent module
            // if (parent->orientation == HORIZONTAL) {
            //     parent->width = leftChild->width + rightChild->width;
            //     parent->height = max(leftChild->height, rightChild->height);
            // } else {
            //     parent->width = max(leftChild->width, rightChild->width);
            //     parent->height = leftChild->height + rightChild->height;
            // }
            
            moduleStack.push(parent);
        }
    }
    
    // The root of the slicing tree is the last element in the stack
    return moduleStack.top();
}

void printTree (Module * root) {
    if (root == NULL) {
        return;
    }
    printTree(root->left_child);
    printTree(root->right_child);

    cout << root->id << " ";
}

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

    // number of modules
    int N; 
    inputFP >> N;

    // Array to store blocks
    Module modules[N]; 

    // Read block dimensions
    for (int i = 0; i < N; i++) {
        inputFP >> modules[i].width >> modules[i].height;
    }

    // Read the Polish expression (last line)
    string line;
    string polishExpression;
    while (getline(inputFP, line)) {
        polishExpression = line;
    }
    inputFP.close();

    cout << polishExpression << endl;

    Module* root = parseAndBuildTree(polishExpression, modules, N);
    printTree(root);

    return 0;
}

void compute_shape_curve(Module& m) {
    compute_shape_curve(*m.left_child);
    compute_shape_curve(*m.right_child);
    // compute the shape curve of current module m ...
}