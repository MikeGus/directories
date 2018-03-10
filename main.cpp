#include <iostream>
#include <dirent.h>
#include <string>
#include <list>
#include <sys/stat.h>

const int PROPER_ARGC = 2;

struct Directory_state {
    DIR *dp;
    std::string path;
};

void print_filename(const size_t level, const std::string& fullpath) {

    std::string print_path(fullpath);
    if (print_path.length() > 1 && print_path.back() == '/') {
        print_path.back() = '\0';
    }

    std::cout << std::string(level, '\t') << std::string(print_path, print_path.rfind('/')) << std::endl;

}

int show_directory_tree(const char* initial_path) {

    std::list<Directory_state> stack;
    struct stat statbuf;
    struct dirent* dirp;

    Directory_state state;
    state.dp = nullptr;
    state.path = initial_path;

    if (lstat(state.path.c_str(), &statbuf) < 0) {
        return -1;
    }
    if (S_ISDIR(statbuf.st_mode) == 0) {
        std::cout << state.path << std::endl;
        return 0;
    }

    state.dp = opendir(state.path.c_str());
    if (state.dp == nullptr) {
        return -1;
    }

    print_filename(0, state.path);

    stack.push_back(state);
    while (!stack.empty()) {
        state = stack.back();
        stack.pop_back();

        if (lstat(state.path.c_str(), &statbuf) < 0) {
            continue;
        }

        if (S_ISDIR(statbuf.st_mode) == 0) {
            print_filename(stack.size(), state.path);
        } else {
            if (state.path.back() != '/') {
                state.path += "/";
            }
            if (state.dp == nullptr) {
                state.dp = opendir(state.path.c_str());
                print_filename(stack.size(), state.path);
                if (state.dp == nullptr) {
                    return -1;
                }
            }

            while ((dirp = readdir(state.dp)) != nullptr) {
                std::string d_name(dirp->d_name);

                if (d_name != "." && d_name != "..") {
                    stack.push_back(state);

                    state.dp = nullptr;
                    state.path += d_name;

                    stack.push_back(state);
                    break;
                }
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


