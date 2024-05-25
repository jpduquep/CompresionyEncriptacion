#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include "common.h"





// Función para generar el árbol de Huffman
HuffmanNode* buildHuffmanTree(const std::map<char, int>& frequencies) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CompareNodes> pq;

    for (auto& pair : frequencies) {
        HuffmanNode* node = new HuffmanNode;
        node->data = pair.first;
        node->frequency = pair.second;
        node->left = nullptr;
        node->right = nullptr;
        pq.push(node);
    }

    while (pq.size() != 1) {
        HuffmanNode* left = pq.top();
        pq.pop();
        HuffmanNode* right = pq.top();
        pq.pop();

        HuffmanNode* newNode = new HuffmanNode;
        newNode->data = '\0';
        newNode->frequency = left->frequency + right->frequency;
        newNode->left = left;
        newNode->right = right;

        pq.push(newNode);
    }

    return pq.top();
}

// Función auxiliar para generar los códigos Huffman recursivamente
void generateCodes(HuffmanNode* root, std::string code, std::map<char, std::string>& codes) {
    if (!root)
        return;

    if (root->data != '\0') {
        codes[root->data] = code;
    }

    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}

// Función para comprimir utilizando el algoritmo de Huffman
std::vector<int> compressHuffman(const std::string& data) {
    std::map<char, int> frequencies;
    for (char c : data) {
        frequencies[c]++;
    }

    HuffmanNode* root = buildHuffmanTree(frequencies);
    std::map<char, std::string> codes;
    generateCodes(root, "", codes);

    std::vector<int> compressedData;
    for (char c : data) {
        std::string code = codes[c];
        for (char bit : code) {
            compressedData.push_back(bit - '0');
        }
    }

    return compressedData;
}

// Función para encriptar los datos (XOR con una clave)
void encryptData(std::vector<int>& data, const std::string& key) {
    for (int& bit : data) {
        bit ^= key[bit % key.size()];
    }
}

int main() {
    std::string imagePath;
    std::string name, age, date, sex, diagnosis;
    std::string encryptionKey = "mykey"; // Clave de encriptación

    // Pedir los datos al usuario
    std::cout << "Ingrese la ruta de la imagen (.jpg): ";
    std::cin >> imagePath;
    std::cout << "Nombre del paciente: ";
    std::cin >> name;
    std::cout << "Edad: ";
    std::cin >> age;
    std::cout << "Fecha: ";
    std::cin >> date;
    std::cout << "Sexo: ";
    std::cin >> sex;
    std::cout << "Diagnóstico: ";
    std::cin >> diagnosis;

    // Leer la imagen
    std::ifstream imageFile(imagePath, std::ios::binary);
    if (!imageFile) {
        std::cerr << "Error al abrir la imagen." << std::endl;
        return 1;
    }
    std::string imageData((std::istreambuf_iterator<char>(imageFile)), std::istreambuf_iterator<char>());
    imageFile.close();

    // Comprimir los datos
    std::vector<int> compressedData = compressHuffman(name + " " + age + " " + date + " " + sex + " " + diagnosis + " " + imageData);

    // Encriptar los datos comprimidos
    encryptData(compressedData, encryptionKey);

    // Escribir los datos comprimidos y encriptados en un archivo .pame
    std::ofstream outFile("paciente.pame", std::ios::binary);
    for (int bit : compressedData) {
        outFile.write(reinterpret_cast<const char*>(&bit), sizeof(bit));
    }
    outFile.close();

    std::cout << "Archivo comprimido y encriptado correctamente." << std::endl;

    return 0;
}
