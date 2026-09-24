#include "child.h"

#include <cerrno>
#include <cstdlib>

#include <unistd.h>

ChildProcess::ChildProcess(const std::vector<std::string>& args)
{
    m_argv_vec.reserve(args.size() + 1);

    for (const auto& arg : args)
    {
        m_argv_vec.push_back(const_cast<char*>(arg.c_str()));
    }
    m_argv_vec.push_back(nullptr);
}

void ChildProcess::spawn(int& error)
{
	char* const* argv = m_argv_vec.data();
    execvpe(argv[0], argv, environ);
    // anything after should be unreachable if success

    error = errno;
    std::exit(error == ENOENT ? 127 : 126); 
}

