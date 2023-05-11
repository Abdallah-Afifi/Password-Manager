#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>
#include <cmath>

// Constants
const std::string ALL_CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789~`!@#$%^&*()_-+={[}]|:;'<,>.?/";
const int NUM_CHARACTER_TYPES = 4;  // Uppercase, lowercase, numbers, special characters

// Function to generate a password
std::string generate_password(int length, std::mt19937_64& generator) {
    std::uniform_int_distribution<std::size_t> distribution(0, ALL_CHARACTERS.size() - 1);

    std::string password;
    password.reserve(length);

    // Generate password
    for (int i = 0; i < length; ++i) {
        std::size_t index = distribution(generator);
        password += ALL_CHARACTERS[index];
    }

    return password;
}

// Function to calculate entropy
double calculate_entropy(int length, int num_characters) {
    return log2(pow(num_characters, length));
}

int main() {
    // User input for password length
    int passwordLength;
    input: 
    std::cout << "Enter desired password length: ";
    std::cin >> passwordLength;

    // Validate password length
    if (passwordLength < 8) {
        std::cout << "Invalid password length." << std::endl;
        goto input;
    }

    // Initialize random number generator with a secure seed
    std::random_device rd;
    std::mt19937_64 generator(rd());

    auto startTime = std::chrono::high_resolution_clock::now();

    // Generate password
    std::string password = generate_password(passwordLength, generator);

    auto endTime = std::chrono::high_resolution_clock::now();
    auto durationmic = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    auto durationmil = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    double entropy = calculate_entropy(passwordLength, ALL_CHARACTERS.size());

    std::cout << "Number of characters: " << ALL_CHARACTERS.size() << std::endl;
    std::cout << "Password Length: " << passwordLength << std::endl;
    std::cout << "Generated password: " << password << std::endl;
    std::cout << "Entropy: " << entropy << std::endl;
    std::cout << "Execution time: " << durationmic.count() << " microseconds" << std::endl;
    std::cout << "Execution time: " << durationmil.count() << " milliseconds" << std::endl;

    return 0;
}
