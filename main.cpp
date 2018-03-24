#include <iostream>
#include <dirent.h>
#include <string>
#include <list>
#include <sys/stat.h>
#include <unistd.h>

const int PROPER_ARGC = 2;

struct Directory_state {
    DIR *dp;
    std::string path;
};

void print_filename(const size_t level, const std::string& fullpath) {
    std::cout << std::string(level, '\t') << fullpath << std::endl;
}

int show_directory_tree(const char* initial_path) {

    std::list<Directory_state> stack;
    struct stat statbuf;
    struct dirent* dirp;

    Directory_state state;
    state.dp = nullptr;
    state.path = initial_path;

    stack.push_back(state);

    while (!stack.empty()) {
        state = stack.back();
        stack.pop_back();

        bool directory = true;

        if (state.dp == nullptr) {
            if (lstat(state.path.c_str(), &statbuf) < 0) {
                directory = false;
            } else {
                print_filename(stack.size(), state.path);
                if (S_ISDIR(statbuf.st_mode) != 0) {
                    state.dp = opendir(state.path.c_str());
                    if (state.dp == nullptr) {
                        return -1;
                    }
                    chdir(state.path.c_str());
                } else {
                    directory = false;
                }
            }
        }
        if (directory) {
            bool add = false;
            while (!add && (dirp = readdir(state.dp)) != nullptr) {
                std::string d_name(dirp->d_name);
                if (d_name != "." && d_name != "..") {
                    stack.push_back(state);

                    state.dp = nullptr;
                    state.path = d_name;

                    stack.push_back(state);
                    add = true;
                }
            }

            if (!add) {
                chdir("..");
            }
        }
    }

    return 0;
}


int main(int argc, char* argv[]) {

    if (argc != PROPER_ARGC) {
        std::cerr << "Need " << PROPER_ARGC - 1 << " argument. "
                  << argc - 1 << " were given." << std::endl;
        return -1;
    }

    show_directory_tree(argv[1]);

    return 0;
}


