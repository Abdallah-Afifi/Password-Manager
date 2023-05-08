#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>

// Generate a random password of a specified length using a given character set
std::string generate_password(int length, const std::string& all_characters, std::mt19937_64& generator) {
    std::string filtered_password(length, ' ');

    int frequency[4] = {1, 1, 1, 1};  // Uppercase, lowercase, numbers, special characters

    // Generate password
    for (int i = 0; i < length; ++i) {
        int index = std::uniform_int_distribution<int>(0, all_characters.size() - 1)(generator);
        char c = all_characters[index];
        filtered_password[i] = c;

        if (std::isupper(c)) {
            --frequency[0];
        } else if (std::islower(c)) {
            --frequency[1];
        } else if (std::isdigit(c)) {
            --frequency[2];
        } else {
            --frequency[3];
        }

        // Check for common patterns during generation
        if (i > 0 && filtered_password[i] == filtered_password[i - 1]) {
            i = -1;  // Restart the generation loop
            std::fill(std::begin(frequency), std::end(frequency), 1);
        }
    }

    // Add missing characters to meet minimum requirements
    std::string missing_chars;
    if (frequency[0] > 0) missing_chars += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (frequency[1] > 0) missing_chars += "abcdefghijklmnopqrstuvwxyz";
    if (frequency[2] > 0) missing_chars += "0123456789";
    if (frequency[3] > 0) missing_chars += "!@#$%^&*()_+={}[]\\|;:'\",.<>/?";

    if (!missing_chars.empty()) {
        std::shuffle(missing_chars.begin(), missing_chars.end(), generator);
        filtered_password += missing_chars.substr(0, frequency[0] + frequency[1] + frequency[2] + frequency[3]);
    }

    return filtered_password;
}

int main() {
    const std::string all_characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()_+={}[]\\|;:'\",.<>/?";

    int passwordLength = 24;

    std::random_device rd;
    std::mt19937_64 generator(rd());

    auto startTime = std::chrono::high_resolution_clock::now();

    std::string password = generate_password(passwordLength, all_characters, generator);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto durationmic = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    auto durationmil = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "Password Length: " << passwordLength << std::endl;
    std::cout << "Generated password: " << password << std::endl;
    std::cout << "Execution time: " << durationmic.count() << " microseconds" << std::endl;
    std::cout << "Execution time: " << durationmil.count() << " milliseconds" << std::endl;

    return 0;
}
