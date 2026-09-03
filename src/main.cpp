#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>

#include <sys/wait.h>
#include <unistd.h>

namespace fs = std::filesystem;

void handle_child(std::vector<std::string> args)
{
    fs::path executable(args[0]);
    std::vector<char*> argv_vec;
    argv_vec.reserve(args.size() + 1);

    for (std::string& arg : args)
    {
        argv_vec.push_back(const_cast<char*>(arg.c_str()));
    }
    argv_vec.push_back(nullptr);

    char* const* argv = argv_vec.data();

    execvpe(argv[0], argv, environ);
    // anything after should be unreachable if success

    int err = errno;
    if (err == ENOENT)
    {
        std::cerr << "command not found: " << executable << "\n";
    }
    else if (err == EACCES)
    {
        std::cerr << "permission denied: " << executable << "\n";
    }
    else
    {
        std::cerr << "execution failed: error code " << err << "\n";
    }

    std::exit(err == ENOENT ? 127 : 126); 
}

void handle_parent(pid_t child_pid)
{
    int status;
    waitpid(child_pid, &status, 0);

    if (WIFEXITED(status))
    {
        int exit_code = WEXITSTATUS(status);
        // TODO: inspect error code
    }
    else if (WIFSIGNALED(status))
    {
        int signal = WTERMSIG(status);
        // TODO: inspect status
    }
}

int main(int argc, char* argv[])
{
    std::string input;

    while (std::getline(std::cin, input))
    {
        if (input == "exit")
        {
            break;
        }

        std::stringstream input_stream(input);
        std::vector<std::string> input_args;
        std::string arg;
        while (input_stream >> arg)
        {
            input_args.push_back(std::move(arg));
            arg.clear();
        }

        if (input_args.empty())
        {
            break;
        }

        pid_t pid = fork();

        if (pid < 0)
        {
            std::cerr << "fork failed" << std::endl;
            break;
        }
        else if (pid == 0)
        {
            // child process
            handle_child(std::move(input_args));
        }
        else
        {
            // parent process
            handle_parent(pid);
        }
    }

    return 0;
}