#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <string>
using namespace std;

mutex fileMutex; // for safe concurrent writes

// Processes a single chunk: reverses bytes (acts as both compress & decompress)
void processChunk(const string &inputFile, const string &outputFile,
                  size_t startPos, size_t chunkSize, int threadID) {
    // Open input
    ifstream in(inputFile, ios::binary);
    if (!in) {
        cerr << "Thread " << threadID << ": Error opening input file!\n";
        return;
    }
    in.seekg(startPos);

    // Read chunk
    vector<char> buffer(chunkSize);
    in.read(buffer.data(), chunkSize);
    size_t bytesRead = in.gcount();
    in.close();

    // Reverse bytes (dummy compression/decompression)
    reverse(buffer.begin(), buffer.begin() + bytesRead);

    // Write to the same position in output file
    {
        lock_guard<mutex> lock(fileMutex);
        fstream out(outputFile, ios::binary | ios::in | ios::out);
        if (!out) {
            cerr << "Thread " << threadID << ": Error opening output file!\n";
            return;
        }
        out.seekp(startPos);
        out.write(buffer.data(), bytesRead);
        out.close();
    }

    // Log
    lock_guard<mutex> lock(fileMutex);
    cout << "Thread " << threadID << ": Processed "
         << bytesRead << " bytes starting at " << startPos << "\n";
}

// Generic operation runner (used for both compression & decompression)
void runOperation(const string &inputFile, const string &outputFile, int numThreads) {
    // Determine file size
    ifstream file(inputFile, ios::binary | ios::ate);
    if (!file) {
        cerr << "Error: Could not open " << inputFile << "\n";
        return;
    }
    size_t fileSize = file.tellg();
    file.close();

    if (fileSize == 0) {
        cerr << "Error: File is empty.\n";
        return;
    }

    // Chunk size calculation (ceiling division)
    size_t chunkSize = (fileSize + numThreads - 1) / numThreads;

    // Pre-allocate output file
    {
        ofstream initFile(outputFile, ios::binary | ios::trunc);
        initFile.seekp(fileSize - 1);
        initFile.write("", 1);
    }

    // Create threads
    vector<thread> threads;
    for (int i = 0; i < numThreads; i++) {
        size_t startPos = i * chunkSize;
        if (startPos >= fileSize) break;
        size_t currentChunkSize = min(chunkSize, fileSize - startPos);
        threads.emplace_back(processChunk, inputFile, outputFile, startPos, currentChunkSize, i);
    }

    // Wait for all threads
    for (auto &t : threads) {
        t.join();
    }
}

int main() {
    int choice;
    string inputFile, outputFile;
    int numThreads = 4; // Adjust based on CPU cores

    cout << "--- Multithreaded File Compression Tool ---\n";
    cout << "1. Compress File\n";
    cout << "2. Decompress File\n";
    cout << "Enter choice: ";
    cin >> choice;
    cin.ignore();

    cout << "Enter input file name: ";
    getline(cin, inputFile);

    if (choice == 1) {
        outputFile = "compressed_" + inputFile;
        cout << "Compressing '" << inputFile << "' -> '" << outputFile << "' ...\n";
        runOperation(inputFile, outputFile, numThreads);
        cout << "Compression completed.\n";
    }
    else if (choice == 2) {
        outputFile = "decompressed_" + inputFile;
        cout << "Decompressing '" << inputFile << "' -> '" << outputFile << "' ...\n";
        runOperation(inputFile, outputFile, numThreads);
        cout << "Decompression completed.\n";
    }
    else {
        cout << "Invalid choice.\n";
    }

    return 0;
}
