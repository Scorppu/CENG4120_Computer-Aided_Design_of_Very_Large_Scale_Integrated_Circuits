#include <iostream>
#include <stack>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

struct Module {
    char id;
    int width = 0;
    int height = 0;
    int x, y; // Position of the module in the layout
    Module* leftChild = nullptr;
    Module* rightChild = nullptr;
    vector<pair<int, int>> shapeCurve; // Shape curve of the module
};

// Module* parseAndBuildTree(const string& expression, Module modules[], int N) {
//     stack<Module*> moduleStack;
//     for (char c : expression) {
//         if (isspace(c)) {
//             continue; // Skip spaces
//         } else if (isdigit(c)) {
//             // Find the module corresponding to the character
//             int index = c - '0';
//             modules[index].id = c;
//             moduleStack.push(&modules[index]);
//         } else if (c == '+' || c == '*') {
//             Module* rightChild = moduleStack.top();
//             moduleStack.pop();
//             Module* leftChild = moduleStack.top();
//             moduleStack.pop();
            
//             // Create a new parent module
//             Module* parent = new Module();
//             parent->id = c;
//             parent->leftChild = leftChild;
//             parent->rightChild = rightChild;
            
//             moduleStack.push(parent);
//         }
//     }
//     // The root of the slicing tree is the last element in the stack
//     return moduleStack.top();
// }

Module* parseAndBuildTree(const string& expression, Module modules[], int N) {
    stack<Module*> moduleStack;
    istringstream iss(expression);
    string token;

    while (iss >> token) {
        if (token == "+" || token == "*") {
            Module* rightChild = moduleStack.top();
            moduleStack.pop();
            Module* leftChild = moduleStack.top();
            moduleStack.pop();
            
            // Create a new parent module
            Module* parent = new Module();
            parent->id = token[0]; // Use the operator as the id
            parent->leftChild = leftChild;
            parent->rightChild = rightChild;
            
            moduleStack.push(parent);
        } else {
            // Convert token to an integer index
            int index = stoi(token);
            modules[index].id = '0' + index; // Set id as a character
            moduleStack.push(&modules[index]);
        }
    }
    // The root of the slicing tree is the last element in the stack
    return moduleStack.top();
}


void printTree(Module * root) {
    // Prints the tree in Post-order traversal (only used in testing)
    if (root == NULL) {
        return;
    }
    printTree(root->leftChild);
    printTree(root->rightChild);
    cout << root->id << " ";
}

void printShapeCurve(Module * root) {
    // Prints the shape curve of each module in the tree in post-order traversal (only used in testing)
    if (root == NULL) {
        return;
    }
    printShapeCurve(root->leftChild);
    printShapeCurve(root->rightChild);

    for (auto& curve : root->shapeCurve) {
        cout << "(" << curve.first << " " << curve.second << ") ";
    }
    cout << endl;
}

void combineHorizontally(Module& parent, Module& leftChild, Module& rightChild) {
    // Combine the shape curves of the children horizontally
    for (auto& leftCurve : leftChild.shapeCurve) {
        for (auto& rightCurve : rightChild.shapeCurve) {
            int width = leftCurve.first + rightCurve.first;
            int height = max(leftCurve.second, rightCurve.second);
            parent.shapeCurve.push_back({width, height});
        }
    }
}

void combineVertically(Module& parent, Module& leftChild, Module& rightChild) {
    // Combine the shape curves of the children vertically
    for (auto& leftCurve : leftChild.shapeCurve) {
        for (auto& rightCurve : rightChild.shapeCurve) {
            int width = max(leftCurve.first, rightCurve.first);
            int height = leftCurve.second + rightCurve.second;
            parent.shapeCurve.push_back({width, height});
        }
    }
}

void computeShapeCurve(Module& m) {
    if (m.leftChild == nullptr && m.rightChild == nullptr) {
        // Leaf node: Initialize shape curve with the block's dimensions
        m.shapeCurve.push_back({m.width, m.height});
        if (m.width != m.height) { // If block can be rotated
            m.shapeCurve.push_back({m.height, m.width});
        }
    } else {
        computeShapeCurve(*m.leftChild);
        computeShapeCurve(*m.rightChild);

        // Combine the shape curves of the children based on whether the cutline is horizontal or vertical
        if (m.id == '+') { // Horizontal cutline
            combineHorizontally(m, *m.leftChild, *m.rightChild);
        } else if (m.id == '*') { // Vertical cutline
            combineVertically(m, *m.leftChild, *m.rightChild);
        }
    }
}

int computeSmallestArea(Module& m) {
    // Compute the smallest area using the shape curve of the root module
    int smallestProduct = INT_MAX;
    for (const auto& pair : m.shapeCurve) {
        int product = pair.first * pair.second;
        if (product < smallestProduct) {
            smallestProduct = product;
        }
    }
    return smallestProduct;
}

void getOptimalDimensions(Module& m, int& width, int& height) {
    // Find the point on the shape curve that gives the smallest area
    int smallestArea = INT_MAX;
    for (const auto& pair : m.shapeCurve) {
        int area = pair.first * pair.second;
        if (area < smallestArea) {
            smallestArea = area;
            width = pair.first;
            height = pair.second;
        }
    }
}

void assignCoordinates(Module& m, int x, int y) {
    if (m.leftChild == nullptr && m.rightChild == nullptr) {
        // Leaf node: Assign coordinates directly
        m.x = x;
        m.y = y;
    } else {
        // Determine the shape of the current module based on its shape curve
        // For simplicity, assume we have a function to get the optimal dimensions
        int width, height;
        getOptimalDimensions(m, width, height);

        if (m.id == '+') { // Horizontal cutline
            // Assign coordinates to children
            assignCoordinates(*m.leftChild, x, y);
            assignCoordinates(*m.rightChild, x, y + m.leftChild->height);
        } else if (m.id == '*') { // Vertical cutline
            // Assign coordinates to children
            assignCoordinates(*m.leftChild, x, y);
            assignCoordinates(*m.rightChild, x + m.leftChild->width, y);
        }

        // Update the current module's coordinates
        m.x = x;
        m.y = y;
        m.width = width;
        m.height = height;
    }
}

void outputCoordinates(Module& m, ofstream& outputFP) {
    if (m.leftChild == nullptr && m.rightChild == nullptr) {
        // Leaf node: Output vertex coordinates
        outputFP << "(" << m.x << " " << m.y << ") (" << m.x + m.width << " " << m.y << ") (" << m.x << " " << m.y + m.height << ") (" << m.x + m.width << " " << m.y + m.height << ")" << endl;
    } else {
        outputCoordinates(*m.leftChild, outputFP);
        outputCoordinates(*m.rightChild, outputFP);
    }
}


int main(int argc, char * argv[]) {
    if (argc < 2 || argc > 3) {
        printf("usage: hw2 <in_file> <out_file>");
        exit(1);
    }
    char * inFile = argv[1];
    char * outFile = argv[2];

    // Handling Polish Expression
    ifstream inputFP(inFile);
    if (!inputFP.is_open()) {
        cout << "Could not open file " << inFile << endl;
        exit(1);
    }

    int N;  // number of modules
    inputFP >> N;
    Module modules[N]; // Array to store blocks
    for (int i = 0; i < N; i++) {
        // Read block dimensions
        inputFP >> modules[i].width >> modules[i].height;
    }
    
    string line;
    string polishExpression;
    while (getline(inputFP, line)) {
        // Read the Polish expression (last line)
        polishExpression = line;
    }
    inputFP.close();

    Module* root = parseAndBuildTree(polishExpression, modules, N); // Builds the tree from the Polish expression
    printTree(root); // Prints the tree in post-order traversal
    cout << endl;

    computeShapeCurve(*root); // Computes the shape curve of each Module
    printShapeCurve(root); // Prints the shape curve of each Module (for testing only)
    assignCoordinates(*root, 0, 0); // resets the coordinates of the root module

    // Opening the output file
    ofstream outputFP(outFile);
    if (!outputFP.is_open()) {
        cout << "Could not open file " << outFile << endl;
        exit(1);
    }

    outputCoordinates(*root, outputFP); // Outputs the coordinates of each vertex
    outputFP <<  computeSmallestArea(*root); // Outputs the smallest area

    outputFP.close();
    return 0;
}

