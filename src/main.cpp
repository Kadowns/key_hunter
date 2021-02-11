#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <filesystem>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

int main(int argc, char *argv[]) {

    std::string keysPath, targetPath, outputFilename;

    try {
        for (int i = 0; i < argc; ++i) {
            if (strcmp(argv[i], "-k") == 0) {
                keysPath = argv[i + 1];
            } else if (strcmp(argv[i], "-t") == 0) {
                targetPath = argv[i + 1];
            }
            else if (strcmp(argv[i], "-o") == 0){
                outputFilename = argv[i + 1];
            }
        }
    } catch (const std::exception& e){
        std::cout << "Failed to parse arguments" << e.what() << std::endl;
        return 1;
    }

    if (keysPath.empty()){
        std::cout << "No keys path provided (-k path), closing..." << std::endl;
        return 1;
    }
    else {
        std::cout << "Key path provided: " << keysPath << std::endl;
    }

    if (targetPath.empty()){
        std::cout << "No target path provided (-t path), closing..." << std::endl;
        return 1;
    }
    else {
        std::cout << "Target path provided: " << targetPath << std::endl;
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

    rapidjson::Document filteredKeysJson;
    filteredKeysJson.SetObject();


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
                    filteredKeysJson.AddMember(it->name, it->value, filteredKeysJson.GetAllocator());
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
    std::cout << "Total before keys: " << keysJson.MemberCount() << std::endl;
    std::cout << "Total after keys: " << filteredKeysJson.MemberCount() << std::endl;


    if (!outputFilename.empty()){
        std::cout << "Saving filtered json to " << outputFilename << std::endl;
        rapidjson::StringBuffer sb;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(sb);
        filteredKeysJson.Accept(writer);

        std::ofstream out(outputFilename);
        out << sb.GetString();
        out.close();
    }
    else {
        std::cout << "No output filename provided (-o filename)" << std::endl;
    }

    return 0;
}
