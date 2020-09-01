#include "headers/get_file_names.h"

std::vector<std::string> get_file_names() {
    std::vector<std::string> file_names;

    DIR *dir_ptr = opendir("../market_data/");
    struct dirent *dir_entered_ptr;
    if (dir_ptr != NULL) {
        dir_entered_ptr = readdir(dir_ptr);

        while (dir_entered_ptr != NULL) {
            std::string curr_file_name = dir_entered_ptr -> d_name;
            
            // Add only if the file names are not . or ..
            if (curr_file_name.compare(".") && curr_file_name.compare("..")) {
                file_names.push_back(curr_file_name);
            }

            dir_entered_ptr = readdir(dir_ptr);
        }
        closedir(dir_ptr);
    }

    else {
        std::cerr << "Couldn't open market_data folder" << std::endl;
    }

    return file_names;
}