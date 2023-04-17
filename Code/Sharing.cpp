#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/scrypt.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

using namespace std;

// Password Manager Data Structure
struct Credential {
    string domain;
    string username;
    string password;
    vector<string> shared_with; // list of usernames with whom the password is shared
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

// RSA key length
const int RSA_KEY_LENGTH = 2048;

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
    return string((char*)derived_key;
                  }
// Generate a random salt for scrypt
string generate_salt() {
unsigned char salt[salt_length];
RAND_bytes(salt, salt_length);
return string((char*)salt, salt_length);
}

// Generate a new RSA key pair
// Returns the private key as a string and the public key as a BIO*
pair<string, BIO*> generate_rsa_keypair() {
RSA* rsa = RSA_new();
BIGNUM* e = BN_new();
BN_set_word(e, RSA_F4);
RSA_generate_key_ex(rsa, RSA_KEY_LENGTH, e, NULL);
BIO* bio = BIO_new(BIO_s_mem());
PEM_write_bio_RSAPrivateKey(bio, rsa, NULL, NULL, 0, NULL, NULL);
char* private_key;
long len = BIO_get_mem_data(bio, &private_key);
string private_key_str(private_key, len);

pair<string, BIO*> keypair(private_key_str, bio);

return keypair;
}
// Share a password with a user by encrypting the password using their public key
// Returns the ciphertext as a string
string share_password(const string& password, const string& public_key) {
RSA* rsa = RSA_new();
BIO* bio = BIO_new_mem_buf((void*)public_key.c_str(), -1);
PEM_read_bio_RSAPublicKey(bio, &rsa, NULL, NULL);
int ciphertext_len = RSA_size(rsa);
unsigned char* ciphertext = new unsigned char[ciphertext_len];

int result = RSA_public_encrypt(password.length(), (const unsigned char*)password.c_str(), ciphertext, rsa, RSA_PKCS1_PADDING);
if (result == -1) {
    cerr << "Error encrypting password" << endl;
    exit(1);
}

RSA_free(rsa);
BIO_free(bio);

return string((char*)ciphertext, ciphertext_len);
}
// Decrypt a shared password using the user's private key
// Returns the plaintext as a string
string decrypt_shared_password(const string& ciphertext, const string& private_key) {
RSA* rsa = RSA_new();
BIO* bio = BIO_new_mem_buf((void*)private_key.c_str(), -1);
PEM_read_bio_RSAPrivateKey(bio, &rsa, NULL, NULL);
int plaintext_len = RSA_private_decrypt(ciphertext.length(), (const unsigned char*)ciphertext.c_str(), (unsigned char*)plaintext, rsa, RSA_PKCS1_PADDING);
if (plaintext_len == -1) {
    cerr << "Error decrypting shared password" << endl;
    exit(1);
}

RSA_free(rsa);
BIO_free(bio);

return string((char*)plaintext, plaintext_len);
}
// Store a new credential in the password manager
void store_credential(const Credential& credential, const string& password) {
// Generate a salt and derive a key from the master password
string salt = generate_salt();
string key = derive_key(password, salt);
// Generate a random initialization vector for AES
string iv = generate_iv();

// Encrypt the password using AES
string encrypted_password = aes_encrypt(credential.password, key, iv);

// Share the password with each user in the shared_with list
vector<string> shared_passwords;
for (string user : credential.shared_with) {
    string shared_password = share_password(credential.password, user.public_key);
    shared_passwords.push_back(shared_password);
}

// Write the credential data to a file
ofstream file("passwords.dat", ios::out | ios::app | ios::binary);
if (!file) {
    cerr << "Error opening file" << endl;
exit(1);
}

file.write((char*)&credential, sizeof(Credential));
file.close();

}

// Search and retrieve a credential by domain and username
Credential get_credential(const string& domain, const string& username, const string& password) {
// Derive a key from the user's password
string derived_key = derive_key(password, domain + username);
// Read the credentials from the file
ifstream file("passwords.dat", ios::in | ios::binary);
if (!file) {
    cerr << "Error opening file" << endl;
    exit(1);
}

Credential credential;
while (file.read((char*)&credential, sizeof(Credential))) {
    // Decrypt the password
    string decrypted_password = aes_decrypt(credential.password, derived_key, credential.domain);

    if (credential.domain == domain && credential.username == username) {
        // Credential found, return it
        credential.password = decrypted_password;
        file.close();
        return credential;
    }
}

// Credential not found
file.close();
cerr << "Credential not found" << endl;
exit(1);
}
// Share a credential with another user
void share_credential(const string& domain, const string& username, const string& password, const string& share_with_username, const string& share_with_password) {
// Derive a key from the user's password
string derived_key = derive_key(password, domain + username);
// Read the credentials from the file
ifstream file("passwords.dat", ios::in | ios::binary);
if (!file) {
    cerr << "Error opening file" << endl;
    exit(1);
}

vector<Credential> credentials;
Credential credential;
while (file.read((char*)&credential, sizeof(Credential))) {
    // Decrypt the password
    string decrypted_password = aes_decrypt(credential.password, derived_key, credential.domain);

    if (credential.domain == domain && credential.username == username) {
        // Credential found, share it with the other user
        RSA* rsa = RSA_new();
        rsa = PEM_read_RSA_PUBKEY(fopen((share_with_username + ".pub").c_str(), "rb"), &rsa, NULL, NULL);
        if (rsa == NULL) {
            cerr << "Error reading public key for user " << share_with_username << endl;
            exit(1);
        }

        unsigned char encrypted_key[RSA_size(rsa)];
        int encrypted_key_length = RSA_public_encrypt(AES_KEY_LENGTH, (const unsigned char*)derived_key.c_str(), encrypted_key, rsa, RSA_PKCS1_OAEP_PADDING);

        credential.shared_with.push_back(share_with_username);

        // Encrypt the password with a new AES key
        string new_key = generate_iv();
        string encrypted_password = aes_encrypt(decrypted_password, new_key, generate_iv());

        // Encrypt the new AES key with the recipient's public key
        RSA* recipient_rsa = RSA_new();
        recipient_rsa = PEM_read_RSA_PUBKEY(fopen((share_with_username + ".pub").c_str(), "rb"), &recipient_rsa, NULL, NULL);
        if (recipient_rsa == NULL) {
            cerr << "Error reading public key for user " << share_with_username << endl;
            exit(1);
        }

        unsigned char encrypted_new_key[RSA_size(recipient_rsa)];
        int encrypted_new_key_length = RSA_public_encrypt(new_key.length(), (const unsigned char*)new_key.c_str(), encrypted_new_key, recipient_rsa, RSA_PKCS1_OAEP_PADDING);

        / Write the updated credential to the file
file.seekp(pos);
file.write(reinterpret_cast<char*>(&credential), sizeof(Credential));
cout << "Password for " << domain << " has been updated." << endl;
return true;
}
return false;
}

// Share a password with another user
bool share_password(const string& domain, const string& username, const string& shared_with_username) {
// Load the credential data from the file
vector<Credential> credentials = load_credentials();
// Find the credential with the matching domain and username
auto it = find_if(credentials.begin(), credentials.end(), [&](const Credential& c) {
    return c.domain == domain && c.username == username;
});

// Check if the credential was found
if (it != credentials.end()) {
    Credential& credential = *it;

    // Check if the user is already shared with
    if (find(credential.shared_with.begin(), credential.shared_with.end(), shared_with_username) != credential.shared_with.end()) {
        cerr << "Password for " << domain << " is already shared with " << shared_with_username << "." << endl;
        return false;
    }

    // Add the shared username to the list
    credential.shared_with.push_back(shared_with_username);

    // Write the updated credential to the file
    ofstream file("passwords.dat", ios::out | ios::binary);
    if (!file) {
        cerr << "Error opening file for writing" << endl;
        exit(1);
    }

    for (const Credential& c : credentials) {
        file.write(reinterpret_cast<const char*>(&c), sizeof(Credential));
    }

    cout << "Password for " << domain << " has been shared with " << shared_with_username << "." << endl;
    return true;
}

cerr << "Password for " << domain << " was not found." << endl;
return false;
}
// Main function
int main() {
// Load the RSA key pair
RSA* rsa_keypair = load_rsa_keypair("keypair.pem");
// Print the menu
while (true) {
    cout << "Enter a command:" << endl;
    cout << "  1. Add password" << endl;
    cout << "  2. Retrieve password" << endl;
    cout << "  3. Update password" << endl;
    cout << "  4. Share password" << endl;
    cout << "  5. Exit" << endl;
    int command;
    cin >> command;

    switch (command) {
        case 1: {
            add_password(rsa_keypair);
            break;
        }
        case 2: {
            retrieve_password(rsa_keypair);
            break;
        }
        case 3: {
            update_password();
            break;
        }
        case 4: {
            share_password();
            break;
        }
        case 5: {
            // Free the RSA key pair
            RSA_free(rsa_keypair);
            return 0;
        }
        default: {
            cerr << "Invalid command" << endl;
            break;
        }
    }
}
return 0;
}


