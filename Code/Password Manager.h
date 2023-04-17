#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <string>

class PasswordManager {
  private:
    string masterPassword;
    map<string, string> passwords;
    set<string> categories;
    map<string, set<string>> categoryPasswords;
  
  public:
    PasswordManager(string password);
    void addPassword(string category, string serviceName, string password);
    void removePassword(string category, string serviceName);
    string getPassword(string category, string serviceName);
    void changeMasterPassword(string newPassword);
    void addCategory(string category);
    void removeCategory(string category);
    void changePassword(string category, string serviceName, string newPassword);
    bool categoryExists(string category);
    bool serviceExists(string category, string serviceName);
    void displayCategories();
    void displayServices(string category);
};


#endif


/* 

Explanation of the data structure:

masterPassword: stores the master password used to encrypt and decrypt all other passwords in the program.
passwords: a map that stores the user's passwords for each service, keyed by the service name.
categories: a set that stores the categories created by the user.
categoryPasswords: a map that stores the passwords for each service in a given category.
The PasswordManager class provides methods for adding, removing, and retrieving passwords, as well as for managing categories and changing the master password. The addPassword and removePassword methods add and remove passwords from both the passwords map and the categoryPasswords map. The getPassword method retrieves the password for a given service in a given category. The addCategory and removeCategory methods add and remove categories from the categories set, and the categoryExists method checks if a given category exists. The displayCategories and displayServices methods are used to display the list of categories and services in a given category, respectively. Finally, the changePassword method is used to change the password for a given service, and the changeMasterPassword method is used to change the master password.

*/
