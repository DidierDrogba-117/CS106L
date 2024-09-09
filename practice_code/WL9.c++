#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

std::string fileToString(std::ifstream& file) {
    // Ensure the file is open and valid
    if (!file.is_open()) {
        return "";  // Return an empty string if the file isn't open
    }

    // Lambda to read the file line by line and accumulate the contents into a string
    auto accumulateFileContents = [&]() -> std::string {
        std::stringstream buffer;
        std::string line;

        // Read the file line by line
        while (std::getline(file, line)) {
            buffer << line << "\n";  // Accumulate each line with a newline
        }
        return buffer.str();  // Return the entire file contents as a string
    };

    // Call the lambda to get the file contents
    return accumulateFileContents();
}

int main() {
    std::ifstream inputFile("example.txt");

    if (!inputFile) {
        std::cerr << "Error opening file." << std::endl;
        return 1;
    }

    // Convert file contents to a string using fileToString
    std::string fileContents = fileToString(inputFile);
    
    std::cout << "File Contents:\n" << fileContents << std::endl;

    return 0;
}
