#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/scrypt.h>

using namespace std;

// Password Manager Data Structure
struct Credential {
    string domain;
    string username;
    string password;
};

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
string aes_encrypt(const string& plaintext, const string& key, const string& iv) {
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

    return string((char*)ciphertext, ciphertext_len);
}

// Decrypt a ciphertext using AES-256 in CBC mode
// Returns the plaintext as a string
string aes_decrypt(const string& ciphertext, const string& key, const string& iv) {
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

    return string((char*)plaintext, plaintext_len);
}

// Generate a random initialization vector for AES
string generate_iv() {
    unsigned char iv[AES_IV_LENGTH];
    RAND_bytes(iv, AES_IV_LENGTH);
    return string((char*)iv, AES_IV_LENGTH);
}

// Derive a key from a password using scrypt
string derive_key(const string& password, const string& salt) {
    unsigned char derived_key[AES_KEY_LENGTH];
    int result = EVP_PBE_scrypt((const char*)password.c_str(), password.length(), (const unsigned char*)salt.c_str(), salt_length, N, r, p, AES_KEY_LENGTH, derived_key);
    if (result != 1) {
        cerr << "Error deriving key" << endl;
        exit(1);
    }
    return string((char*)derived_key, AES_KEY_LENGTH);
}

// Encrypt credentials and write to file
void write_credentials_to_file(const vector<Credential>& credentials, const string& password, const string& file_path) {
    ofstream output_file(file_path, ios::out | ios::binary);
    if (!output_file.is_open()) {
        cerr << "Error Opening output file" << endl;
exit(1);
}
// Generate random salt for key derivation
unsigned char salt[salt_length];
RAND_bytes(salt, salt_length);
string salt_string((char*)salt, salt_length);

// Derive key from password and salt
string key = derive_key(password, salt_string);

// Generate random initialization vector for AES
string iv = generate_iv();

// Write salt and iv to file
output_file.write(salt_string.c_str(), salt_string.length());
output_file.write(iv.c_str(), iv.length());

// Encrypt and write each credential to file
for (const auto& credential : credentials) {
    // Serialize credential to string
    string credential_string = credential.domain + ":" + credential.username + ":" + credential.password;

    // Encrypt credential
    string encrypted_credential = aes_encrypt(credential_string, key, iv);

    // Write encrypted credential to file
    output_file.write(encrypted_credential.c_str(), encrypted_credential.length());
}

output_file.close();
}

// Read encrypted credentials from file and decrypt
vector<Credential> read_credentials_from_file(const string& password, const string& file_path) {
ifstream input_file(file_path, ios::in | ios::binary);
if (!input_file.is_open()) {
cerr << "Error opening input file" << endl;
exit(1);
}
// Read salt and iv from file
char salt_cstr[salt_length];
input_file.read(salt_cstr, salt_length);
string salt(salt_cstr, salt_length);

char iv_cstr[AES_IV_LENGTH];
input_file.read(iv_cstr, AES_IV_LENGTH);
string iv(iv_cstr, AES_IV_LENGTH);

// Derive key from password and salt
string key = derive_key(password, salt);

// Read encrypted credentials from file and decrypt
vector<Credential> credentials;
while (input_file.peek() != EOF) {
    // Read encrypted credential from file
    int encrypted_credential_length = 0;
    input_file.read((char*)&encrypted_credential_length, sizeof(int));
    char* encrypted_credential_cstr = new char[encrypted_credential_length];
    input_file.read(encrypted_credential_cstr, encrypted_credential_length);
    string encrypted_credential(encrypted_credential_cstr, encrypted_credential_length);

    // Decrypt credential
    string credential_string = aes_decrypt(encrypted_credential, key, iv);

    // Deserialize credential from string
    size_t colon_pos = credential_string.find(':');
    string domain = credential_string.substr(0, colon_pos);
    credential_string.erase(0, colon_pos + 1);
    colon_pos = credential_string.find(':');
    string username = credential_string.substr(0, colon_pos);
    credential_string.erase(0, colon_pos + 1);
    string password = credential_string;

    // Create and append credential to vector
    Credential credential = { domain, username, password };
    credentials.push_back(credential);

    delete[] encrypted_credential_cstr;
}

input_file.close();

return credentials;
}
oid read_credentials_from_file(vector<Credential>& credentials, const string& password, const string& file_path) {
// Read encrypted data from file
ifstream input_file(file_path, ios::in | ios::binary);
if (!input_file.is_open()) {
cerr << "Error opening file" << endl;
exit(1);
}
input_file.seekg(0, ios::end);
int file_size = input_file.tellg();
input_file.seekg(0, ios::beg);

char* encrypted_data = new char[file_size];
input_file.read(encrypted_data, file_size);

input_file.close();

// Derive key from password
string salt(32, ' '); // generate a string of 32 spaces
memcpy((char*)salt.c_str(), encrypted_data, 32);
string key = derive_key(password, salt);

// Decrypt data
string iv = string(encrypted_data + 32, AES_IV_LENGTH);
string ciphertext(encrypted_data + 32 + AES_IV_LENGTH, file_size - 32 - AES_IV_LENGTH);
string plaintext = aes_decrypt(ciphertext, key, iv);

// Parse data
const char* delimiter = "\n";
char* line = strtok((char*)plaintext.c_str(), delimiter);

while (line != NULL) {
    Credential credential;
    credential.domain = line;
    line = strtok(NULL, delimiter);
    credential.username = line;
    line = strtok(NULL, delimiter);
    credential.password = line;
    line = strtok(NULL, delimiter);
    credentials.push_back(credential);
}

delete[] encrypted_data;
}

int main() {
vector<Credential> credentials;
// Write credentials to file
write_credentials_to_file(credentials, "mypassword", "credentials.bin");

// Read credentials from file
vector<Credential> read_credentials;
read_credentials_from_file(read_credentials, "mypassword", "credentials.bin");

return 0;
}
