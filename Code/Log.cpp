#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>

using namespace std;

// Write an error message to the error log file
void log_error(const string& error_message) {
    // Get the current date and time
    auto now = chrono::system_clock::now();
    time_t current_time = chrono::system_clock::to_time_t(now);

    // Convert the time to a string
    char time_string[100];
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    // Open the error log file for appending
    ofstream log_file("error.log", ios::out | ios::app);
    if (!log_file) {
        cerr << "Error opening error log file" << endl;
        return;
    }

    // Write the error message and current time to the log file
    log_file << "[" << time_string << "] " << error_message << endl;

    // Close the log file
    log_file.close();
}

// Write a warning message to the warning log file
void log_warning(const string& warning_message) {
    // Get the current date and time
    auto now = chrono::system_clock::now();
    time_t current_time = chrono::system_clock::to_time_t(now);

    // Convert the time to a string
    char time_string[100];
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    // Open the warning log file for appending
    ofstream log_file("warning.log", ios::out | ios::app);
    if (!log_file) {
        cerr << "Error opening warning log file" << endl;
        return;
    }

    // Write the warning message and current time to the log file
    log_file << "[" << time_string << "] " << warning_message << endl;

    // Close the log file
    log_file.close();
}

// Write an info message to the info log file
void log_info(const string& info_message) {
    // Get the current date and time
    auto now = chrono::system_clock::now();
    time_t current_time = chrono::system_clock::to_time_t(now);

    // Convert the time to a string
    char time_string[100];
    strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    // Open the info log file for appending
    ofstream log_file("info.log", ios::out | ios::app);
    if (!log_file) {
        cerr << "Error opening info log file" << endl;
        return;
    }

    // Write the info message and current time to the log file
    log_file << "[" << time_string << "] " << info_message << endl;

    // Close the log file
    log_file.close();
}
