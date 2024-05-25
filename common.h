#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <string>
#include <map>
#include <queue>

// Estructura para el nodo del árbol de Huffman
struct HuffmanNode {
    char data;
    int frequency;
    HuffmanNode *left, *right;
};

// Comparador para la cola de prioridad
struct CompareNodes {
    bool operator()(HuffmanNode* const& a, HuffmanNode* const& b) {
        return a->frequency > b->frequency;
    }
};

// Funciones para desencriptar los datos (XOR con una clave)
void decryptData(std::vector<int>& data, const std::string& key);

// Funciones para reconstruir el árbol de Huffman desde el archivo
HuffmanNode* reconstructHuffmanTree(std::ifstream& inFile);

// Funciones para descomprimir utilizando el árbol de Huffman
std::string decompressHuffman(const std::vector<int>& compressedData, HuffmanNode* root);

#endif
