#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdint>
#include <cstring>
#include <jpeglib.h>

using namespace std;

struct ImageInfo {
    int width;
    int height;
    vector<uint8_t> imageData;
};

struct PatientInfo {
    string patientName;
    string patientLastName;
    int age;
    string sex;
    string date;
};

string decryptData(const string& encryptedData, const string& key) {
    // Aquí implementarías tu algoritmo de descifrado, por ejemplo, DES
    // Por simplicidad, este código simplemente devuelve los datos sin modificar
    return encryptedData;
}

void readEncryptedFile(const string& inputFileName, string& encryptedData) {
    ifstream inputFile(inputFileName, ios::binary);
    if (!inputFile.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << inputFileName << " para lectura." << endl;
        exit(1);
    }

    inputFile.seekg(0, ios::end);
    size_t fileSize = inputFile.tellg();
    inputFile.seekg(0, ios::beg);

    encryptedData.resize(fileSize);
    inputFile.read(encryptedData.data(), fileSize);
    inputFile.close();
}

ImageInfo extractImageInfo(const string& encryptedData) {
    // Simplemente tomamos los últimos bytes como los datos de la imagen
    ImageInfo imageInfo;
    size_t headerSize = encryptedData.find_first_of('{');
    imageInfo.imageData = vector<uint8_t>(encryptedData.begin() + headerSize, encryptedData.end());
    return imageInfo;
}

PatientInfo extractPatientInfo(const string& encryptedData) {
    // Aquí implementarías la extracción de los metadatos del paciente
    // Por simplicidad, este código simplemente devuelve información ficticia
    PatientInfo patientInfo;
    patientInfo.patientName = "Juan";
    patientInfo.patientLastName = "Pérez";
    patientInfo.age = 30;
    patientInfo.sex = "Masculino";
    patientInfo.date = "2024-05-30";
    return patientInfo;
}

void writeJPEG(const string& fileName, const ImageInfo& imageInfo) {
    FILE* outfile = fopen(fileName.c_str(), "wb");
    if (!outfile) {
        cerr << "Error: No se pudo abrir el archivo " << fileName << " para escritura." << endl;
        return;
    }

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = imageInfo.width;
    cinfo.image_height = imageInfo.height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);

    int row_stride = imageInfo.width * 3;
    vector<uint8_t> imageDataCopy = imageInfo.imageData;  // Copiar los datos de la imagen a un nuevo vector
    while (cinfo.next_scanline < cinfo.image_height) {
        JSAMPROW row_pointer = &imageDataCopy[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);

    cout << "Imagen JPEG escrita en " << fileName << endl;
}


int main() {
    string key = "mi_clave_secreta"; // La misma clave utilizada para cifrar los datos

    string inputFileName = "imagen_encrypted.mex"; // Nombre del archivo cifrado
    string encryptedData;
    readEncryptedFile(inputFileName, encryptedData);

    string decryptedData = decryptData(encryptedData, key);
    ImageInfo imageInfo = extractImageInfo(decryptedData);
    PatientInfo patientInfo = extractPatientInfo(decryptedData);

    cout << "Datos del paciente:" << endl;
    cout << "Nombre: " << patientInfo.patientName << " " << patientInfo.patientLastName << endl;
    cout << "Edad: " << patientInfo.age << endl;
    cout << "Sexo: " << patientInfo.sex << endl;
    cout << "Fecha: " << patientInfo.date << endl;

    string outputFileName = "imagen_recuperada.jpg";
    writeJPEG(outputFileName, imageInfo);

    return 0;
}
