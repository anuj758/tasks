#include <iostream>
#include <fstream>   // For file I/O
#include <string>    // For std::string

using namespace std;

void writeToFile(const string &filename) {
    ofstream outFile(filename); // Opens in write mode (overwrites existing content)
    if (!outFile) {
        cerr << "Error opening file for writing!" << endl;
        return;
    }
    cout << "Enter text to write to file (end with a single line containing only 'END'):\n";
    
    string line;
    while (true) {
        getline(cin, line);
        if (line == "END") break;
        outFile << line << '\n';
    }
    outFile.close();
    cout << "Data written to file successfully.\n";
}

void appendToFile(const string &filename) {
    ofstream outFile(filename, ios::app); // Append mode
    if (!outFile) {
        cerr << "Error opening file for appending!" << endl;
        return;
    }
    cout << "Enter text to append to file (end with 'END'):\n";
    
    string line;
    while (true) {
        getline(cin, line);
        if (line == "END") break;
        outFile << line << '\n';
    }
    outFile.close();
    cout << "Data appended to file successfully.\n";
}

void readFromFile(const string &filename) {
    ifstream inFile(filename); // Read mode
    if (!inFile) {
        cerr << "Error opening file for reading!" << endl;
        return;
    }
    cout << "\n--- File Content ---\n";
    
    string line;
    while (getline(inFile, line)) {
        cout << line << endl;
    }
    cout << "--- End of File ---\n";
    inFile.close();
}

int main() {
    string filename = "data.txt";
    int choice;

    do {
        cout << "\nFile Operations Menu:\n";
        cout << "1. Write to File (overwrite)\n";
        cout << "2. Append to File\n";
        cout << "3. Read from File\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore(); // Ignore leftover newline from previous input

        switch(choice) {
            case 1: writeToFile(filename); break;
            case 2: appendToFile(filename); break;
            case 3: readFromFile(filename); break;
            case 4: cout << "Exiting program.\n"; break;
            default: cout << "Invalid choice! Try again.\n";
        }
    } while (choice != 4);

    return 0;
}
