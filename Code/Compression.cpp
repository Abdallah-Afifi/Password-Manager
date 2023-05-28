#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <chrono>

std::vector<int> compress(const std::string& input) {
    std::unordered_map<std::string, int> dictionary;
    for (int i = 0; i < 256; ++i) {
        dictionary[std::string(1, static_cast<char>(i))] = i;
    }

    std::string current;
    std::vector<int> compressed;
    for (const char& c : input) {
        std::string next = current + c;
        if (dictionary.find(next) != dictionary.end()) {
            current = next;
        } else {
            compressed.push_back(dictionary[current]);
            dictionary[next] = dictionary.size();
            current = std::string(1, c);
        }
    }

    if (!current.empty()) {
        compressed.push_back(dictionary[current]);
    }

    return compressed;
}

std::string decompress(const std::vector<int>& compressed) {
    std::unordered_map<int, std::string> dictionary;
    for (int i = 0; i < 256; ++i) {
        dictionary[i] = std::string(1, static_cast<char>(i));
    }

    std::string current = dictionary[compressed[0]];
    std::string output = current;
    for (int i = 1; i < compressed.size(); ++i) {
        std::string entry;
        if (dictionary.find(compressed[i]) != dictionary.end()) {
            entry = dictionary[compressed[i]];
        } else if (compressed[i] == dictionary.size()) {
            entry = current + current[0];
        }

        output += entry;
        dictionary[dictionary.size()] = current + entry[0];
        current = entry;
    }

    return output;
}

void testCompression(const std::string& input) {
    std::cout << "Input: " << input << std::endl;

    auto compressStart = std::chrono::steady_clock::now();
    std::vector<int> compressed = compress(input);
    auto compressEnd = std::chrono::steady_clock::now();
    std::cout << "Compressed: ";
    for (const int& code : compressed) {
        std::cout << code << " ";
    }
    std::cout << std::endl;

    auto decompressStart = std::chrono::steady_clock::now();
    std::string decompressed = decompress(compressed);
    auto decompressEnd = std::chrono::steady_clock::now();
    std::cout << "Decompressed: " << decompressed << std::endl;

    std::cout << "-----------------------" << std::endl;

    // Calculate compression ratio
    double inputSize = input.size();
    double compressedSize = compressed.size() * sizeof(int);
    double compressionRatio = compressedSize / inputSize;
    std::cout << "Compression Ratio: " << compressionRatio << std::endl;

    // Calculate compression and decompression time
    std::chrono::duration<double, std::milli> compressTime = compressEnd - compressStart;
    std::cout << "Compression Time: " << compressTime.count() << " milliseconds" << std::endl;

    std::chrono::duration<double, std::milli> decompressTime = decompressEnd - decompressStart;
    std::cout << "Decompression Time: " << decompressTime.count() << " milliseconds" << std::endl;

    std::cout << "-----------------------" << std::endl;
}

int main() {
    // Test case using a randomly generated password
    testCompression("p3C=I<97[=<K,S-3?R6J)WgH(i$Ih!&A");

    return 0;
}
