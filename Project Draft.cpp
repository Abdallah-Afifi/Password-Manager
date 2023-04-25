#ifndef PASSWORD_MANAGER_H
#define PASSWORD_MANAGER_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class PasswordManager {
public:
  PasswordManager();
  ~PasswordManager();

  void addPassword(string website, string username, string password);
  string getPassword(string website);
  void changePassword(string website, string newPassword);
  void deletePassword(string website);

private:
  vector<string> websites;
  vector<string> usernames;
  vector<string> passwords;

  string generatePassword(int length);
  void encryptPassword(string password);
  void decryptPassword(string password);

};

#endif


#include "PasswordManager.h"

PasswordManager::PasswordManager() {
  websites.clear();
  usernames.clear();
  passwords.clear();
}

PasswordManager::~PasswordManager() {
  websites.clear();
  usernames.clear();
  passwords.clear();
}

void PasswordManager::addPassword(string website, string username, string password) {
  websites.push_back(website);
  usernames.push_back(username);
  encryptPassword(password);
  passwords.push_back(password);
}

string PasswordManager::getPassword(string website) {
  int index = -1;
  for (int i = 0; i < websites.size(); i++) {
    if (websites[i] == website) {
      index = i;
      break;
    }
  }

  if (index == -1) {
    return "";
  }

  return passwords[index];
}

void PasswordManager::changePassword(string website, string newPassword) {
  int index = -1;
  for (int i = 0; i < websites.size(); i++) {
    if (websites[i] == website) {
      index = i;
      break;
    }
  }

  if (index == -1) {
    return;
  }

  encryptPassword(newPassword);
  passwords[index] = newPassword;
}

void PasswordManager::deletePassword(string website) {
  int index = -1;
  for (int i = 0; i < websites.size(); i++) {
    if (websites[i] == website) {
      index = i;
      break;
    }
  }

  if (index == -1) {
    return;
  }

  websites.erase(websites.begin() + index);
  usernames.erase(usernames.begin() + index);
  passwords.erase(passwords.begin() + index);
}

string PasswordManager::generatePassword(int length) {
  string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  string password;

  for (int i = 0; i < length; i++) {
    password += characters[rand() % characters.length()];
  }

  return password;
}

void PasswordManager::encryptPassword(string password) {
  // TODO: Implement encryption algorithm.
}

void PasswordManager::decryptPassword(string password) {
  // TODO: Implement decryption algorithm.
}


#include <iostream>
#include "PasswordManager.h"

int main() {
  PasswordManager manager;

  // Add some passwords.
  manager.addPassword("google.com", "username", "password");
  manager.addPassword("facebook.com", "username", "password");
  manager.addPassword("twitter.com", "username", "password");

  // Get a password.
  string password = manager.getPassword("google.com");
  cout << "Password for google.com: " << password << endl;

  // Change a password.
  manager.changePassword("google.com", "newpassword");

  // Delete a password.
  manager.deletePassword("facebook.com");

  return 0;
}
