// Securely get a password from the user
std::string get_password(const char* prompt) {
    char* password = getpass(prompt);
    return std::string(password);
}

// Securely get input from the user
std::string get_input(const char* prompt) {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}
