#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <filesystem>

#include <rapidjson/document.h>

int main(int argc, char *argv[]) {

    std::string keysPath, targetPath;

    try {
        for (int i = 0; i < argc; ++i) {
            if (strcmp(argv[i], "-k") == 0) {
                keysPath = argv[i + 1];
            } else if (strcmp(argv[i], "-t") == 0) {
                targetPath = argv[i + 1];
            }
        }
    } catch (const std::exception& e){
        std::cout << "Failed to parse arguments" << e.what() << std::endl;
        return 1;
    }

    std::cout << "keys path: " << keysPath << std::endl << "target path" << targetPath << std::endl;

    std::map<std::string, std::string> files;

    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(targetPath)){
        if (dirEntry.is_regular_file()){
            std::ifstream ifs(dirEntry.path());
            std::string content(std::istreambuf_iterator<char>(ifs),(std::istreambuf_iterator<char>()) );
            files[dirEntry.path().filename().string()] = content;
        }
    }

    std::ifstream ifs(keysPath);
    std::string content(std::istreambuf_iterator<char>(ifs),(std::istreambuf_iterator<char>()) );

    rapidjson::Document keysJson;
    keysJson.Parse(content.c_str());

    std::map<std::string, int> foundKeys;
    std::set<std::string> notFoundKeys;

    for (auto it = keysJson.MemberBegin(); it != keysJson.MemberEnd(); it++){
        std::string key = it->name.GetString();
        for (auto& file : files){
            if (file.second.find("\"" + key + "\"") != std::string::npos) {
                if (foundKeys.find(key) != foundKeys.end()){
                    foundKeys[key]++;
                }
                else {
                    foundKeys[key] = 1;
                }
            }
        }
        if (foundKeys.find(key) == foundKeys.end()){
            notFoundKeys.insert(key);
        }
    }

    for (auto& kv : foundKeys){
        std::cout << "-Found key - " << kv.first << " - " << kv.second << " times" << std::endl;
    }

    for (auto& key : notFoundKeys){
        std::cout << "-Key not found - " << key << std::endl;
    }

    std::cout << "Keys found: " << foundKeys.size() << std::endl;
    std::cout << "Keys not found: " << notFoundKeys.size() << std::endl;

    return 0;
}
