#include <iostream>
#include <stack>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

using namespace std;

struct Module {
    char id;
    int width = 0;
    int height = 0;
    bool orientation = false; // false: original, true: rotated
    int x, y; // Position of the module in the layout
    Module* leftChild = nullptr;
    Module* rightChild = nullptr;
    vector<pair<int, int>> shapeCurve; // Shape curve of the module
};

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
    cout << root->id << " " << "(" <<root->width << " " << root->height << ")" << endl;
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
            height = pair.first;
            width = pair.second;
        }
    }
}

void assignCoordinates(Module& m, int x, int y, int width, int height) {
    bool foundOptimal = false;
    if (m.leftChild == nullptr && m.rightChild == nullptr) {
        // Leaf node:
        m.x = x;
        m.y = y;
        m.width = width;
        m.height = height;
        return;
    } else {
        if (m.id == '+') {
            // Horizontal cutline
            // Find the optimal dimensions of the left and right children
            for (auto& curve1 : m.leftChild->shapeCurve) {
                for (auto& curve2: m.rightChild->shapeCurve) {
                    if ((curve1.first + curve2.first) == height && max(curve1.second, curve2.second) == width) {
                        // Found the optimal dimensions
                        m.leftChild->x = x;
                        m.leftChild->y = y;
                        // Rotates the block if necessary
                        m.leftChild->width = curve1.second;
                        m.leftChild->height = curve1.first;

                        m.rightChild->x = x;
                        m.rightChild->y = y + curve1.first;
                        //Rotates the block if necessary
                        m.rightChild->width = curve2.second;
                        m.rightChild->height = curve2.first;
                        foundOptimal = true;
                        break;
                    }
                }
                if (foundOptimal) {
                    break;
                }
            }
        } else if (m.id == '*') {
            // Vertical cutline
            // Find the optimal dimensions of the left and right children
            for (auto& curve1 : m.leftChild->shapeCurve) {
                for (auto& curve2: m.rightChild->shapeCurve) {
                    if (curve1.first + curve2.first == width && max(curve1.second, curve2.second) == height) {
                        // Found the optimal dimensions
                        m.leftChild->x = x;
                        m.leftChild->y = y;
                        // Rotates the block if necessary
                        m.leftChild->width = curve1.first;
                        m.leftChild->height = curve1.second;

                        m.rightChild->x = x + curve1.first;
                        m.rightChild->y = y;
                        // Rotates the block if necessary
                        m.rightChild->width = curve2.first;
                        m.rightChild->height = curve2.second;
                        foundOptimal = true;
                        break;
                    }
                }
                if (foundOptimal) {
                    break;
                }
            }
        }
        assignCoordinates(*m.leftChild, m.leftChild->x, m.leftChild->y, m.leftChild->width, m.leftChild->height);
        assignCoordinates(*m.rightChild, m.rightChild->x, m.rightChild->y, m.rightChild->width, m.rightChild->height);
    }
}

void assignCoordinatesHelper(Module& m, int x, int y) {
    int width, height;
    getOptimalDimensions(m, width, height);
    assignCoordinates(m, x, y, width, height);
}

// Function to collect all leaf nodes into a vector
void collectLeafNodes(Module* m, vector<Module*>& leafNodes) {
    if (m == nullptr) {
        return;
    }
    if (m->leftChild == nullptr && m->rightChild == nullptr) {
        leafNodes.push_back(m);
    } else {
        collectLeafNodes(m->leftChild, leafNodes);
        collectLeafNodes(m->rightChild, leafNodes);
    }
}

// Modified function to output coordinates in order of Module.id
void outputCoordinates(Module& m, ofstream& outputFP) {
    vector<Module*> leafNodes;
    collectLeafNodes(&m, leafNodes);

    // Sort leaf nodes by their id
    sort(leafNodes.begin(), leafNodes.end(), [](Module* a, Module* b) {
        return a->id < b->id;
    });

    // Output vertex coordinates for each leaf node in sorted order
    for (Module* node : leafNodes) {
        outputFP << "(" << node->x << " " << node->y << ") (" << node->x + node->width << " " << node->y << ") (" << node->x << " " << node->y + node->height << ") (" << node->x + node->width << " " << node->y + node->height << ")" << endl;
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
    // printTree(root); // Prints the tree in post-order traversal
    cout << endl;

    computeShapeCurve(*root); // Computes the shape curve of each Module
    // printShapeCurve(root); // Prints the shape curve of each Module (for testing only)
    assignCoordinatesHelper(*root, 0, 0); // resets the coordinates of the root module

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

