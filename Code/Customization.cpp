// Allow the user to customize password generation options
void customize_password_generation(int& password_length, bool& require_uppercase, bool& require_lowercase, bool& require_digits, bool& require_symbols) {
    std::cout << "Customize password generation options:" << std::endl;
    std::cout << "Password length (8-128): ";
    std::cin >> password_length;
    if (password_length < 8 || password_length > 128) {
        std::cerr << "Invalid password length" << std::endl;
        exit(1);
    }
    std::cout << "Require uppercase letters? (y/n): ";
    char response;
    std::cin >> response;
    require_uppercase = (response == 'y' || response == 'Y');
    std::cout << "Require lowercase letters? (y/n): ";
    std::cin >> response;
    require_lowercase = (response == 'y' || response == 'Y');
    std::cout << "Require digits? (y/n): ";
    std::cin >> response;
    require_digits = (response == 'y' || response == 'Y');
    std::cout << "Require symbols? (y/n): ";
    std::cin >> response;
    require_symbols = (response == 'y' || response == 'Y');
}

// Here's an example of how this function can be used in conjunction with the generate_password() function from the previous step:


// Generate a secure random password with user-specified options
std::string generate_password(int password_length, bool require_uppercase, bool require_lowercase, bool require_digits, bool require_symbols) {
    std::string password;
    bool meets_requirements = false;
    while (!meets_requirements) {
        password = generate_random_string(password_length);
        meets_requirements = true;
        if (require_uppercase && !contains_uppercase(password)) {
            meets_requirements = false;
        }
        if (require_lowercase && !contains_lowercase(password)) {
            meets_requirements = false;
        }
        if (require_digits && !contains_digit(password)) {
            meets_requirements = false;
        }
        if (require_symbols && !contains_symbol(password)) {
            meets_requirements = false;
        }
    }
    return password;
}

int main() {
    int password_length = 16;
    bool require_uppercase = true;
    bool require_lowercase = true;
    bool require_digits = true;
    bool require_symbols = true;

    customize_password_generation(password_length, require_uppercase, require_lowercase, require_digits, require_symbols);

    std::string password = generate_password(password_length, require_uppercase, require_lowercase, require_digits, require_symbols);

    std::cout << "Generated password: " << password << std::endl;

    return 0;
}
