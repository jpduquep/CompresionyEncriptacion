#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstdint>
#include <cstring>
#include <jpeglib.h>
#include <limits>


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

PatientInfo getPatientInfoFromCommandLine() {
    PatientInfo info;

    cout << "Por favor, introduce el nombre del paciente: ";
    getline(cin, info.patientName);

    cout << "Por favor, introduce el apellido del paciente: ";
    getline(cin, info.patientLastName);

    cout << "Por favor, introduce la edad del paciente: ";
    cin >> info.age;
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignorar el resto de la línea

    cout << "Por favor, introduce el sexo del paciente: ";
    getline(cin, info.sex);

    cout << "Por favor, introduce la fecha: ";
    getline(cin, info.date);

    return info;
}

ImageInfo readJPEG(const string& jpgFileName) {
    FILE* infile = fopen(jpgFileName.c_str(), "rb");
    if (!infile) {
        cerr << "Error: No se pudo abrir la imagen " << jpgFileName << endl;
        exit(1);
    }

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    ImageInfo imageInfo;
    imageInfo.width = cinfo.output_width;
    imageInfo.height = cinfo.output_height;
    imageInfo.imageData.resize(imageInfo.width * imageInfo.height * cinfo.num_components);

    JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr)&cinfo, JPOOL_IMAGE, imageInfo.width * cinfo.num_components, 1);

    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        memcpy(&imageInfo.imageData[(cinfo.output_scanline - 1) * imageInfo.width * cinfo.num_components],
               buffer[0], imageInfo.width * cinfo.num_components);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    fclose(infile);
    return imageInfo;
}

string writeBSONHeader(const ImageInfo& imageInfo, const PatientInfo& patientInfo) {
    ostringstream headerStream;
    headerStream << "{\"height\":" << imageInfo.height << ",\"width\":" << imageInfo.width
                 << ",\"sexo\":\"" << patientInfo.sex << "\",\"nombre\":\"" << patientInfo.patientName
                 << "\",\"apellido\":\"" << patientInfo.patientLastName << "\",\"fecha\":\"" << patientInfo.date << "\"}";
    return headerStream.str();
}

void encryptData(const string& data, const string& key, string& encryptedData) {
    encryptedData = data; // Aquí implementarías tu algoritmo de cifrado, por ejemplo, DES
}

void createEncryptedFile(const string& jpgFileName, const string& outputFileName, const PatientInfo& patientInfo, const string& key) {
    ImageInfo imageInfo = readJPEG(jpgFileName);
    string header = writeBSONHeader(imageInfo, patientInfo);
    string combinedData = header + string(imageInfo.imageData.begin(), imageInfo.imageData.end());

    string encryptedData;
    encryptData(combinedData, key, encryptedData);

    ofstream outputFile(outputFileName, ios::binary);
    if (!outputFile.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << outputFileName << " para escritura." << endl;
        return;
    }

    outputFile.write(encryptedData.data(), encryptedData.size());
    outputFile.close();

    cout << "Archivo " << outputFileName << " creado exitosamente." << endl;
}

int main() {
    string key = "mi_clave_secreta"; // Clave secreta para cifrar los datos

    PatientInfo patientInfo = getPatientInfoFromCommandLine();
    string jpgFileName = "imagen.jpg";
    string outputFileName = "imagen_encrypted.mex";

    createEncryptedFile(jpgFileName, outputFileName, patientInfo, key);

    return 0;
}
