#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <bitset>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct Patient {
    std::string name;
    int age;
    float height;
    float weight;
    std::string diagnosis;
    std::string diagnosisDate;
};

struct Node {
    unsigned char data;
    unsigned int freq;
    Node *left, *right;

    Node(unsigned char data, unsigned int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

struct compare {
    bool operator()(Node* l, Node* r) {
        return (l->freq > r->freq);
    }
};

Node* buildHuffmanTree(const std::unordered_map<unsigned char, unsigned int> &freq) {
    std::priority_queue<Node*, std::vector<Node*>, compare> minHeap;
    for (auto pair : freq) {
        minHeap.push(new Node(pair.first, pair.second));
    }

    while (minHeap.size() != 1) {
        Node *left = minHeap.top(); minHeap.pop();
        Node *right = minHeap.top(); minHeap.pop();
        Node *top = new Node('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        minHeap.push(top);
    }
    return minHeap.top();
}

void storeCodes(Node* root, std::string str, std::unordered_map<unsigned char, std::string> &huffmanCode) {
    if (!root) return;
    if (root->data != '$') huffmanCode[root->data] = str;
    storeCodes(root->left, str + "0", huffmanCode);
    storeCodes(root->right, str + "1", huffmanCode);
}

std::string decode(Node* root, const std::string &binaryString) {
    std::string decodedString;
    Node* currentNode = root;
    for (char bit : binaryString) {
        if (bit == '0') {
            currentNode = currentNode->left;
        } else {
            currentNode = currentNode->right;
        }

        if (!currentNode->left && !currentNode->right) {
            decodedString += currentNode->data;
            currentNode = root;
        }
    }
    return decodedString;
}

void saveImage(const std::vector<unsigned char> &imageData, int width, int height, int channels, const std::string &filename) {
    stbi_write_jpg(filename.c_str(), width, height, channels, imageData.data(), 100);
}

void readFromFile(const std::string &filename, Patient &patient, std::vector<unsigned char> &compressedData, int &width, int &height, int &channels) {
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile.is_open()) {
        std::cerr << "Error opening file for reading" << std::endl;
        exit(1);
    }

    std::string line;
    std::getline(inFile, line); patient.name = line.substr(6);
    std::getline(inFile, line); patient.age = std::stoi(line.substr(5));
    std::getline(inFile, line); patient.height = std::stof(line.substr(8));
    std::getline(inFile, line); patient.weight = std::stof(line.substr(8));
    std::getline(inFile, line); patient.diagnosis = line.substr(11);
    std::getline(inFile, line); patient.diagnosisDate = line.substr(16);

    inFile.read(reinterpret_cast<char*>(&width), sizeof(width));
    inFile.read(reinterpret_cast<char*>(&height), sizeof(height));
    inFile.read(reinterpret_cast<char*>(&channels), sizeof(channels));

    unsigned char byte;
    while (inFile.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
        compressedData.push_back(byte);
    }

    inFile.close();
}

std::string byteArrayToBinaryString(const std::vector<unsigned char> &bytes) {
    std::string binaryString;
    for (unsigned char byte : bytes) {
        binaryString += std::bitset<8>(byte).to_string();
    }
    return binaryString;
}

int main() {
    Patient patient;
    std::vector<unsigned char> compressedData;
    int width, height, channels;

    readFromFile("patient_data.pap", patient, compressedData, width, height, channels);

    std::cout << "Patient Data:" << std::endl;
    std::cout << "Name: " << patient.name << std::endl;
    std::cout << "Age: " << patient.age << std::endl;
    std::cout << "Height: " << patient.height << std::endl;
    std::cout << "Weight: " << patient.weight << std::endl;
    std::cout << "Diagnosis: " << patient.diagnosis << std::endl;
    std::cout << "Diagnosis Date: " << patient.diagnosisDate << std::endl;

    std::unordered_map<unsigned char, unsigned int> freq;
    for (unsigned char byte : compressedData) {
        freq[byte]++;
    }

    Node* root = buildHuffmanTree(freq);
    std::unordered_map<unsigned char, std::string> huffmanCode;
    storeCodes(root, "", huffmanCode);

    std::string binaryString = byteArrayToBinaryString(compressedData);
    std::string decodedString = decode(root, binaryString);

    std::vector<unsigned char> imageData(decodedString.begin(), decodedString.end());

    saveImage(imageData, width, height, channels, "imagenSalida.jpg");

    std::cout << "Image saved as imagenSalida.jpg" << std::endl;

    return 0;
}
