#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"  // Asegúrate de tener stb_image.h en tu proyecto

struct Patient {
    std::string name;
    int age;
    float height;
    float weight;
    std::string diagnosis;
    std::string diagnosisDate;
};

void getPatientData(Patient &patient) {
    std::cout << "Enter name: ";
    std::getline(std::cin, patient.name);
    std::cout << "Enter age: ";
    std::cin >> patient.age;
    std::cout << "Enter height: ";
    std::cin >> patient.height;
    std::cout << "Enter weight: ";
    std::cin >> patient.weight;
    std::cin.ignore();  // Clear the newline character from the input buffer
    std::cout << "Enter diagnosis: ";
    std::getline(std::cin, patient.diagnosis);
    std::cout << "Enter diagnosis date: ";
    std::getline(std::cin, patient.diagnosisDate);
}

std::vector<unsigned char> readImage(const std::string &filename, int &width, int &height, int &channels) {
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &channels, 0);
    if (data == nullptr) {
        std::cerr << "Error loading image " << filename << std::endl;
        exit(1);
    }
    std::vector<unsigned char> image(data, data + (width * height * channels));
    stbi_image_free(data);
    return image;
}

// Huffman Node
struct Node {
    unsigned char data;
    unsigned int freq;
    Node *left, *right;

    Node(unsigned char data, unsigned int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

// Compare two nodes based on their frequency
struct compare {
    bool operator()(Node* l, Node* r) {
        return (l->freq > r->freq);
    }
};

// Traverse the Huffman Tree and store Huffman Codes in a map.
void storeCodes(struct Node* root, std::string str, std::unordered_map<unsigned char, std::string> &huffmanCode) {
    if (!root) return;
    if (root->data != '$') huffmanCode[root->data] = str;
    storeCodes(root->left, str + "0", huffmanCode);
    storeCodes(root->right, str + "1", huffmanCode);
}

// Function to build the Huffman Tree and store it in huffmanCode map
void HuffmanCodes(std::vector<unsigned char> &data, std::unordered_map<unsigned char, std::string> &huffmanCode) {
    std::unordered_map<unsigned char, unsigned int> freq;
    for (unsigned char ch : data) freq[ch]++;
    std::priority_queue<Node*, std::vector<Node*>, compare> minHeap;
    for (auto pair : freq) minHeap.push(new Node(pair.first, pair.second));
    while (minHeap.size() != 1) {
        Node *left = minHeap.top(); minHeap.pop();
        Node *right = minHeap.top(); minHeap.pop();
        Node *top = new Node('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        minHeap.push(top);
    }
    storeCodes(minHeap.top(), "", huffmanCode);
}

std::string encode(std::vector<unsigned char> &data, std::unordered_map<unsigned char, std::string> &huffmanCode) {
    std::string encodedString;
    for (unsigned char ch : data) {
        encodedString += huffmanCode[ch];
    }
    return encodedString;
}

std::vector<unsigned char> getBytes(const std::string &binaryString) {
    std::vector<unsigned char> bytes;
    for (size_t i = 0; i < binaryString.size(); i += 8) {
        std::string byteString = binaryString.substr(i, 8);
        unsigned char byte = static_cast<unsigned char>(std::stoi(byteString, nullptr, 2));
        bytes.push_back(byte);
    }
    return bytes;
}

void saveToFile(const std::string &filename, Patient &patient, std::vector<unsigned char> &imageBytes) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Error opening file for writing" << std::endl;
        exit(1);
    }

    outFile << "Name: " << patient.name << "\n";
    outFile << "Age: " << patient.age << "\n";
    outFile << "Height: " << patient.height << "\n";
    outFile << "Weight: " << patient.weight << "\n";
    outFile << "Diagnosis: " << patient.diagnosis << "\n";
    outFile << "Diagnosis Date: " << patient.diagnosisDate << "\n";

    for (unsigned char byte : imageBytes) {
        outFile << byte;
    }

    outFile.close();
}

int main() {
    Patient patient;
    getPatientData(patient);

    std::string imageName;
    std::cout << "Enter image filename (with .jpg extension): ";
    std::cin >> imageName;

    int width, height, channels;
    std::vector<unsigned char> image = readImage(imageName, width, height, channels);

    std::unordered_map<unsigned char, std::string> huffmanCode;
    HuffmanCodes(image, huffmanCode);
    std::string encodedString = encode(image, huffmanCode);
    std::vector<unsigned char> compressedData = getBytes(encodedString);

    saveToFile("patient_data.pap", patient, compressedData);

    std::cout << "Data saved to patient_data.pap" << std::endl;
    return 0;
}
