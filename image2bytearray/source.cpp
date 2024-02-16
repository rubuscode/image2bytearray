#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct ImageInfo {
    int width;
    int height;
};

std::vector<unsigned char> imageToBytes(const char* imagePath, ImageInfo& info) {
    int channels;
    unsigned char* image = stbi_load(imagePath, &info.width, &info.height, &channels, 0);
    if (!image) {
        std::cerr << "failed to load image: " << imagePath << std::endl;
        exit(EXIT_FAILURE);
    }
    std::vector<unsigned char> imageBytes(image, image + info.width * info.height * channels);
    stbi_image_free(image);
    return imageBytes;
}

void writeBytesToFile(const char* outputPath, const std::vector<unsigned char>& imageBytes) {
    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "failed to open output file: " << outputPath << std::endl;
        exit(EXIT_FAILURE);
    }
    outputFile << "// converter by rubus ~# https://github.com/rubuscode\n"
        << "// file size: " << imageBytes.size() << "\n"
        << "unsigned char image[] = {\n";
    size_t lineLength = 0;
    for (size_t i = 0; i < imageBytes.size(); ++i) {
        if (lineLength == 0)
            outputFile << "\t";
        outputFile << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(imageBytes[i]);
        if (i < imageBytes.size() - 1)
            outputFile << ", ";
        if (++lineLength == 16) {
            outputFile << '\n';
            lineLength = 0;
        }
    }

    outputFile << "\n};\n";
    outputFile.close();
    std::cout << "image bytes written to: " << outputPath << std::endl;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "usage: " << argv[0] << " <input_image_path> <output_header_path>" << std::endl;
        return EXIT_FAILURE;
    }
    const char* imagePath = argv[1];
    std::string outputHeaderPathWithExtension = std::string(argv[2]) + ".h";
    const char* outputHeaderPath = outputHeaderPathWithExtension.c_str();
    ImageInfo info;
    std::vector<unsigned char> imageBytes = imageToBytes(imagePath, info);
    writeBytesToFile(outputHeaderPath, imageBytes);
    return EXIT_SUCCESS;
}
