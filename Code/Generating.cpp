#include <iostream>
#include <cstring>
#include <openssl/rand.h>

// Generate a random password of a specified length
std::string generate_password(int length) {
    // Define character sets to use for password generation
    std::string uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string lowercase = "abcdefghijklmnopqrstuvwxyz";
    std::string numbers = "0123456789";
    std::string symbols = "!@#$%^&*()_+-={}[]\\|;:'\",.<>?/";

    // Concatenate character sets into a single set
    std::string all_characters = uppercase + lowercase + numbers + symbols;

    // Generate a random password using OpenSSL's random number generator
    unsigned char password[length];
    RAND_bytes(password, length);

    // Convert the password to a string and filter out any characters not in the set of allowed characters
    std::string password_string = std::string((char*)password, length);
    std::string filtered_password;
    for (char c : password_string) {
        if (all_characters.find(c) != std::string::npos) {
            filtered_password += c;
        }
    }

    // Pad the password with additional characters from the set of allowed characters until it reaches the desired length
    while (filtered_password.length() < length) {
        int index = RAND_bytes(password, 1) % all_characters.length();
        filtered_password += all_characters[index];
    }

    return filtered_password;
}

int main() {
    // Example usage:
    std::string password = generate_password(16);
    std::cout << "Generated password: " << password << std::endl;
    return 0;
}
