#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "common.h"

// Funciones para desencriptar los datos (XOR con una clave)
void decryptData(std::vector<int>& data, const std::string& key) {
    for (int& bit : data) {
        bit ^= key[bit % key.size()];
    }
}

HuffmanNode* reconstructHuffmanTree(std::istream& inStream) {
    char nodeType;
    inStream.get(nodeType);

    if (inStream.eof()) {
        std::cerr << "Error: Fin de archivo inesperado al reconstruir el árbol de Huffman." << std::endl;
        return nullptr;
    }

    if (nodeType == 'L') { // Nodo hoja
        char data;
        inStream.get(data);
        return new HuffmanNode{data, 0, nullptr, nullptr};
    } else if (nodeType == 'I') { // Nodo interno
        HuffmanNode* left = reconstructHuffmanTree(inStream);
        HuffmanNode* right = reconstructHuffmanTree(inStream);
        return new HuffmanNode{'\0', 0, left, right};
    } else {
        std::cerr << "Error: Tipo de nodo desconocido al reconstruir el árbol de Huffman." << std::endl;
        return nullptr;
    }
}

std::string decompressHuffman(const std::vector<int>& compressedData, HuffmanNode* root) {
    if (root == nullptr) {
        std::cerr << "Error: Árbol de Huffman nulo." << std::endl;
        return "";
    }

    std::string decompressedData;
    const HuffmanNode* currentNode = root;

    for (int bit : compressedData) {
        if (currentNode == nullptr) {
            std::cerr << "Error: Nodo nulo durante la descompresión." << std::endl;
            return "";
        }

        if (bit == 0) { // Mover al nodo izquierdo
            currentNode = currentNode->left;
        } else { // Mover al nodo derecho
            currentNode = currentNode->right;
        }

        if (currentNode->left == nullptr && currentNode->right == nullptr) { // Nodo hoja
            decompressedData += currentNode->data;
            currentNode = root; // Volver a la raíz para el siguiente bit
        }
    }

    return decompressedData;
}

int main() {
    std::string encryptionKey = "mykey"; // Clave de desencriptación

    // Leer el archivo .pame
    std::ifstream inFile("paciente.pame", std::ios::binary);
    if (!inFile) {
        std::cerr << "Error al abrir el archivo .pame." << std::endl;
        return 1;
    }

    // Leer los datos comprimidos y encriptados desde el archivo .pame
    std::vector<int> compressedData;
    int bit;
    while (inFile.read(reinterpret_cast<char*>(&bit), sizeof(bit))) {
        compressedData.push_back(bit);
    }
    inFile.close(); // Cerramos el archivo después de leer los datos

    // Desencriptar los datos
    decryptData(compressedData, encryptionKey);

    // Reconstruir el árbol de Huffman desde los datos comprimidos
    std::istringstream dataStream(std::string(compressedData.begin(), compressedData.end()));
    HuffmanNode* root = reconstructHuffmanTree(dataStream);

    if (root == nullptr) {
        std::cerr << "Error al reconstruir el árbol de Huffman." << std::endl;
        return 1;
    }

    // Descomprimir los datos utilizando el árbol de Huffman
    std::string rawData = decompressHuffman(compressedData, root);

    if (rawData.empty()) {
        std::cerr << "Error al descomprimir los datos." << std::endl;
        return 1;
    }

    // Extraer los metadatos y la imagen
    std::string name, age, date, sex, diagnosis, imageData;
    std::istringstream iss(rawData);
    iss >> name >> age >> date >> sex >> diagnosis;
    std::getline(iss, imageData);

    // Imprimir los metadatos
    std::cout << "Nombre: " << name << std::endl;
    std::cout << "Edad: " << age << std::endl;
    std::cout << "Fecha: " << date << std::endl;
    std::cout << "Sexo: " << sex << std::endl;
    std::cout << "Diagnóstico: " << diagnosis << std::endl;

    // Guardar la imagen
    std::ofstream imageFile("imagen_recuperada.jpg", std::ios::binary);
    imageFile << imageData;
    imageFile.close();

    std::cout << "Datos recuperados correctamente." << std::endl;

    return 0;
}
