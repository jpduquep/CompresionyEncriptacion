#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdint>
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
    // Aquí implementa tu algoritmo de descifrado
    return encryptedData;
}

void readEncryptedFile(const string& inputFileName, ImageInfo& imageInfo, PatientInfo& patientInfo, const string& key) {
    ifstream inputFile(inputFileName, ios::binary);
    if (!inputFile.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << inputFileName << " para lectura." << endl;
        return;
    }

    stringstream encryptedDataStream;
    encryptedDataStream << inputFile.rdbuf();
    string encryptedData = encryptedDataStream.str();

    string decryptedData = decryptData(encryptedData, key);

    // Extraer metadatos del encabezado
    stringstream headerStream(decryptedData.substr(0, decryptedData.find("}") + 1));
    char comma;
    headerStream >> comma; // Consumir el primer '{'
    headerStream >> comma; // Consumir el primer '"'

    headerStream >> comma; // Ignorar el primer '"'
    headerStream >> imageInfo.height >> comma >> imageInfo.width >> comma;
    headerStream >> comma; // Ignorar el segundo '"'
    headerStream.ignore(1); // Ignorar la coma
    getline(headerStream, patientInfo.sex, '"');
    headerStream >> comma;
    headerStream.ignore(1);
    getline(headerStream, patientInfo.patientName, '"');
    headerStream >> comma;
    headerStream.ignore(1);
    getline(headerStream, patientInfo.patientLastName, '"');
    headerStream >> comma;
    headerStream.ignore(1);
    getline(headerStream, patientInfo.date, '"');

    // Extraer datos de la imagen
    imageInfo.imageData = vector<uint8_t>(decryptedData.begin() + headerStream.tellg(), decryptedData.end());

    inputFile.close();
}

void writeJPEG(const string& outputFileName, const ImageInfo& imageInfo) {
    FILE* outfile = fopen(outputFileName.c_str(), "wb");
    if (!outfile) {
        cerr << "Error: No se pudo abrir el archivo " << outputFileName << " para escritura." << endl;
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
    jpeg_set_quality(&cinfo, 75, TRUE);

    jpeg_start_compress(&cinfo, TRUE);

    vector<uint8_t> scanlineBuffer(imageInfo.width * 3);
    JSAMPROW rowPointer = &scanlineBuffer[0];
    while (cinfo.next_scanline < cinfo.image_height) {
        int offset = cinfo.next_scanline * imageInfo.width * 3;
        for (int i = 0; i < imageInfo.width * 3; ++i) {
            scanlineBuffer[i] = imageInfo.imageData[offset + i];
        }
        jpeg_write_scanlines(&cinfo, &rowPointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    fclose(outfile);

    cout << "Imagen desencriptada guardada como " << outputFileName << endl;
}

int main() {
    string key = "mi_clave_secreta"; // Clave secreta para descifrar los datos

    ImageInfo imageInfo;
    PatientInfo patientInfo;
    string inputFileName = "imagenEncriptada.mex";
    string outputFileName = "imagenRecuperada.jpg";

    readEncryptedFile(inputFileName, imageInfo, patientInfo, key);

    cout << "Metadatos de la imagen:" << endl;
    cout << "Nombre del paciente: " << patientInfo.patientName << " " << patientInfo.patientLastName << endl;
    cout << "Edad del paciente: " << patientInfo.age << endl;
    cout << "Sexo del paciente: " << patientInfo.sex << endl;
    cout << "Fecha: " << patientInfo.date << endl;

    writeJPEG(outputFileName, imageInfo);

    return 0;
}
