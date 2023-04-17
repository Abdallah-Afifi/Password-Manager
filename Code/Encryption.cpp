#include <iostream>
#include <cstring>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/scrypt.h>

// AES key length in bytes
const int AES_KEY_LENGTH = 32;

// AES IV length in bytes
const int AES_IV_LENGTH = 16;

// Scrypt parameters
const uint64_t N = 16384;
const uint64_t r = 8;
const uint64_t p = 1;
const uint64_t salt_length = 32;

// Encrypt a plaintext using AES-256 in CBC mode
// Returns the ciphertext as a string
std::string aes_encrypt(const std::string& plaintext, const std::string& key, const std::string& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)key.c_str(), (const unsigned char*)iv.c_str());

    int ciphertext_len = plaintext.length() + AES_BLOCK_SIZE;
    unsigned char* ciphertext = new unsigned char[ciphertext_len];

    int len;
    EVP_EncryptUpdate(ctx, ciphertext, &len, (const unsigned char*)plaintext.c_str(), plaintext.length());
    ciphertext_len = len;

    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return std::string((char*)ciphertext, ciphertext_len);
}

// Decrypt a ciphertext using AES-256 in CBC mode
// Returns the plaintext as a string
std::string aes_decrypt(const std::string& ciphertext, const std::string& key, const std::string& iv) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, (const unsigned char*)key.c_str(), (const unsigned char*)iv.c_str());

    int plaintext_len = ciphertext.length();
    unsigned char* plaintext = new unsigned char[plaintext_len];

    int len;
    EVP_DecryptUpdate(ctx, plaintext, &len, (const unsigned char*)ciphertext.c_str(), ciphertext.length());
    plaintext_len = len;

    EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return std::string((char*)plaintext, plaintext_len);
}

// Generate a random initialization vector for AES
std::string generate_iv() {
    unsigned char iv[AES_IV_LENGTH];
    RAND_bytes(iv, AES_IV_LENGTH);
    return std::string((char*)iv, AES_IV_LENGTH);
}

// Derive a key from a password using scrypt
std::string derive_key(const std::string& password, const std::string& salt) {
    unsigned char derived_key[AES_KEY_LENGTH];
    int result = EVP_PBE_scrypt((const char*)password.c_str(), password.length(), (const unsigned char*)salt.c_str(), salt_length, N, r, p, AES_KEY_LENGTH, derived_key);
    if (result != 1) {
        std::cerr << "Error deriving key" << std::endl;
        exit(1);
    }
    return std::string((char*)derived_key, AES_KEY_LENGTH);
}

int main() {
    // Example usage:
    std::string password = "password123";
    std::string plaintext = "This is a secret message";
    std::string salt = "randomsalt";

    // Derive encryption key from password and salt
    std::string key = derive_key(password, salt);
    // Generate random initialization vector
    std::string iv = generate_iv();

     // Encrypt plaintext using key and IV
std::string ciphertext = aes_encrypt(plaintext, key, iv);

// Decrypt ciphertext using key and IV
std::string decrypted_text = aes_decrypt(ciphertext, key, iv);

std::cout << "Original message: " << plaintext << std::endl;
std::cout << "Encrypted message: " << ciphertext << std::endl;
std::cout << "Decrypted message: " << decrypted_text << std::endl;

return 0;

