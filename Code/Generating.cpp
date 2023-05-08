#include <iostream>
#include <string>
#include <unordered_set>
#include <chrono>
#include <vector>
#include <random>
#include <algorithm>

// Generate a random password of a specified length using a given character set
std::string generate_password(int length, const std::unordered_set<char>& all_characters) {
    std::string filtered_password;
    std::vector<char> character_vector(all_characters.begin(), all_characters.end());

    std::random_device rd;
    std::mt19937_64 generator(rd());
    std::uniform_int_distribution<int> distribution(0, character_vector.size() - 1);

    // Define minimum requirements
    int min_uppercase = 1;
    int min_lowercase = 1;
    int min_numbers = 1;
    int min_special_chars = 1;

    // Generate password
    while (filtered_password.length() < length) {
        int index = distribution(generator);
        char c = character_vector[index];
        filtered_password += c;

        // Check and decrement minimum requirements
        if (std::isupper(c)) {
            min_uppercase--;
        } else if (std::islower(c)) {
            min_lowercase--;
        } else if (std::isdigit(c)) {
            min_numbers--;
        } else {
            min_special_chars--;
        }
    }

    // Exclude common patterns
    while (filtered_password.find("00") != std::string::npos || 
           filtered_password.find("11") != std::string::npos ||
           filtered_password.find("22") != std::string::npos ||
           filtered_password.find("33") != std::string::npos ||
           filtered_password.find("44") != std::string::npos ||
           filtered_password.find("55") != std::string::npos ||
           filtered_password.find("66") != std::string::npos ||
           filtered_password.find("77") != std::string::npos ||
           filtered_password.find("88") != std::string::npos ||
           filtered_password.find("99") != std::string::npos) {
        filtered_password = generate_password(length, all_characters);
    }

    // Shuffle the password
    std::shuffle(filtered_password.begin(), filtered_password.end(), generator);

    // Check and add missing characters to meet minimum requirements
    std::string missing_chars;
    if (min_uppercase > 0) missing_chars += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (min_lowercase > 0) missing_chars += "abcdefghijklmnopqrstuvwxyz";
    if (min_numbers > 0) missing_chars += "0123456789";
    if (min_special_chars > 0) missing_chars += "!@#$%^&*()_+={}[]\\|;:'\",.<>/?";

    if (!missing_chars.empty()) {
        std::shuffle(missing_chars.begin(), missing_chars.end(), generator);
        filtered_password += missing_chars.substr(0, std::max(0, min_uppercase + min_lowercase + min_numbers + min_special_chars));
    }

    return filtered_password;
}


int main() {
    std::unordered_set<char> all_characters{
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
        '=', '{', '}', '[', ']', '\\', '|', ';', ':', '\'', '\"', ',',
        '.', '<', '>', '?', '/'
    };

    int passwordLength = (rand() % 24 ) + 16;
   
    auto startTime = std::chrono::high_resolution_clock::now();

    std::string password = generate_password(passwordLength, all_characters);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto durationmic = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    auto durationmil = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "Password Length: " << passwordLength << std::endl; 
    std::cout << "Generated password: " << password << std::endl;
    std::cout << "Execution time: " << durationmic.count() << " microseconds" << std::endl;
    std::cout << "Execution time: " << durationmil.count() << " milliseconds" << std::endl;


    return 0;
}
