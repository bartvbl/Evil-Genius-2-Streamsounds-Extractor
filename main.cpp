#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

struct ContainedFile {
    std::string filename;
    unsigned int fileStartByte = 0;
    unsigned int fileLengthBytes = 0;
};

unsigned int readInt(const char* array, size_t startByte) {
    const unsigned char* arrayPointer = reinterpret_cast<const unsigned char*>(array);
    return ((unsigned int) (arrayPointer[startByte + 0]) << 0U) |
           ((unsigned int) (arrayPointer[startByte + 1]) << 8U) |
           ((unsigned int) (arrayPointer[startByte + 2]) << 16U) |
           ((unsigned int) (arrayPointer[startByte + 3]) << 24U);
}

int main(int argc, char** argv) {
    if(argc == 0) {
        std::cout << "Usage: extractor [filename].streamsounds" << std::endl;
        return 0;
    }

    std::filesystem::path inputFilePath = std::string(argv[1]);
    std::fstream inFileStream(inputFilePath, std::ios::in | std::ios::binary);
    size_t totalSize = std::filesystem::file_size(inputFilePath);
    std::vector<char> fileContents;
    fileContents.resize(totalSize);
    inFileStream.read(fileContents.data(), totalSize);

    size_t filePointer = 0;

    const unsigned int fileHeaderSize = 29;
    char fileID[fileHeaderSize];
    std::copy(fileContents.data(), fileContents.data() + fileHeaderSize, fileID);
    unsigned int fileCount = readInt(fileID, 24);
    std::cout << "This file contains " << fileCount << " files." << std::endl;
    filePointer += fileHeaderSize;

    std::cout << "Reading file header.." << std::endl;
    std::vector<ContainedFile> fileReferences;
    fileReferences.reserve(fileCount);
    for(unsigned int i = 0; i < fileCount; i++) {
        ContainedFile file;

        // Read file name

        std::vector<char> fileString;
        char nextChar = 1;
        while(nextChar != 0) {
            nextChar = fileContents.at(filePointer);
            filePointer++;
            fileString.push_back(nextChar);
        }

        // Skip to next nonzero byte.. No idea why they did this

        while(nextChar == 0) {
            filePointer++;
            nextChar = fileContents.at(filePointer);
        }

        // Read file location info

        file.fileLengthBytes = readInt(fileContents.data(), filePointer + 0);
        file.fileStartByte = readInt(fileContents.data(), filePointer + 4);
        filePointer += 8;

        // Sometimes there are some additional zero bytes. Skip those until we get to the next filename.
        
        nextChar = fileContents.at(filePointer);
        while(nextChar == 0) {
            filePointer++;
            nextChar = fileContents.at(filePointer);
        }

        fileString.push_back(0);
        file.filename = std::string(fileString.data());
        fileReferences.push_back(file);

        std::cout << "File " << fileReferences.size() << " start " << file.fileStartByte << ", length " << file.fileLengthBytes << " - " << file.filename << std::endl;

        // Make sure paths are platform independent
        
        for(unsigned int i = 0; i < file.filename.size(); i++) {
            if(file.filename[i] == '\\') {
                file.filename[i] = '/';
            }
        }
        
        // Write output file
        
        std::filesystem::path assetPath(file.filename);
        std::filesystem::create_directories(assetPath.parent_path());
        std::ofstream outStream(assetPath, std::ios::binary | std::ios::out);
        outStream.write(fileContents.data() + file.fileStartByte, file.fileLengthBytes);
    }
    
    return 0;
}
