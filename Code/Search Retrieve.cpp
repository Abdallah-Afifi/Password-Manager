// Search and retrieve password for a given domain and username
// Returns an empty string if the domain and username are not found
string search_credentials(const vector<Credential>& credentials, const string& domain, const string& username, const string& password) {
    // Search for the credential with the matching domain and username
    for (const auto& cred : credentials) {
        if (cred.domain == domain && cred.username == username) {
            // Decrypt the password
            string decrypted_password = aes_decrypt(cred.password, derive_key(password, cred.domain), cred.domain);

            // Return the decrypted password
            return decrypted_password;
        }
    }

    // If not found, return an empty string
    return "";
}
