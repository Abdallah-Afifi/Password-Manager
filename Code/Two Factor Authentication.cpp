#include <iostream>
#include <cstring>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <openssl/hmac.h>

using namespace std;

// Secret key for TOTP
const string TOTP_SECRET = "my_totp_secret_key";

// Number of digits in TOTP code
const int TOTP_DIGITS = 6;

// Time interval for TOTP in seconds
const int TOTP_INTERVAL = 30;

// Generate a TOTP code using a secret key and the current time
string generate_totp_code(const string& secret_key) {
    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    int timestamp = now / TOTP_INTERVAL;
    string timestamp_str = to_string(timestamp);

    // Convert secret key to base32
    BIO* mem = BIO_new(BIO_s_mem());
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO* b32 = BIO_new(BIO_f_base32());
    BIO_push(b64, mem);
    BIO_push(b32, b64);
    BIO_write(mem, secret_key.c_str(), secret_key.length());
    BIO_flush(mem);
    char* base32_secret_key;
    long base32_secret_key_len = BIO_get_mem_data(mem, &base32_secret_key);

    // Compute HMAC-SHA1 hash of timestamp using secret key
    unsigned char hmac_sha1_result[SHA_DIGEST_LENGTH];
    HMAC(EVP_sha1(), base32_secret_key, base32_secret_key_len, (unsigned char*)timestamp_str.c_str(), timestamp_str.length(), hmac_sha1_result, NULL);

    // Get the 4-byte dynamic offset from the last byte of the hash
    int offset = hmac_sha1_result[SHA_DIGEST_LENGTH - 1] & 0xf;

    // Get the 4-byte binary code starting at the offset
    int binary_code = (hmac_sha1_result[offset] & 0x7f) << 24 |
                      (hmac_sha1_result[offset + 1] & 0xff) << 16 |
                      (hmac_sha1_result[offset + 2] & 0xff) << 8 |
                      (hmac_sha1_result[offset + 3] & 0xff);

    // Convert binary code to TOTP code
    int totp_code = binary_code % (int)pow(10, TOTP_DIGITS);
    string totp_code_str = to_string(totp_code);
    while (totp_code_str.length() < TOTP_DIGITS) {
        totp_code_str = "0" + totp_code_str;
    }

    return totp_code_str;
}

// Verify a TOTP code with a secret key
bool verify_totp_code(const string& totp_code, const string& secret_key) {
    string correct_totp_code = generate_totp_code(secret_key);
    return (totp_code == correct_totp_code);
}

int main() {
    // Generate a TOTP code and print it
    string totp_code = generate_totp_code(TOTP_SECRET);
    cout << "TOTP Code: " << totp_code << endl;

    // Verify a TOTP code and print the result
    string input_totp_code;
    cout << "Enter TOTP Code: ";
    cin >> input_totp_code;
    bool is_valid_totp_code = verify_totp_code(input_totp_code, TOTP_SECRET);
    cout << "Is TOTP Code Valid? " << (is_valid_totp_code ? "Yes" : "No") << endl;
if (is_valid_totp_code) {
    // Perform the requested operation
    switch (choice) {
        case 1:
            add_credential(credentials, shared_credentials);
            break;
        case 2:
            search_credentials(credentials);
            break;
        case 3:
            retrieve_credential(credentials, shared_credentials);
            break;
        case 4:
            share_credential(credentials, shared_credentials);
            break;
        case 5:
            list_credentials(credentials, shared_credentials);
            break;
        case 6:
            exit(0);
        default:
            cerr << "Invalid choice" << endl;
            exit(1);
    }
} else {
    cerr << "Invalid TOTP code" << endl;
    exit(1);
}

return 0;
}
// Verify a TOTP code using the secret key
bool verify_totp_code(const string& totp_code, const string& totp_secret) {
int code_digits = totp_code.length();
unsigned char hash[SHA1_DIGEST_LENGTH];
int secret_length = totp_secret.length();
unsigned char* secret = new unsigned char[secret_length];
memcpy(secret, totp_secret.c_str(), secret_length);
// Calculate the TOTP value based on the current time
unsigned long long timestamp = time(NULL) / 30;
unsigned char msg[8];
for (int i = 8; i--; timestamp >>= 8) {
    msg[i] = static_cast<unsigned char>(timestamp);
}

// Calculate the HMAC-SHA1 value
HMAC(EVP_sha1(), secret, secret_length, msg, 8, hash, NULL);

// Determine the offset and length of the code in the hash value
int offset = hash[19] & 0xf;
int length = 4;
if (code_digits == 6) {
    offset = hash[19] & 0x3;
    length = 6;
}

// Extract the code from the hash value
int binary_code = ((hash[offset] & 0x7f) << 24) | ((hash[offset + 1] & 0xff) << 16) | ((hash[offset + 2] & 0xff) << 8) | (hash[offset + 3] & 0xff);
string expected_code = to_string(binary_code % static_cast<int>(pow(10, length)));

return (expected_code == totp_code);
}
