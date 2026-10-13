#pragma once

#include <string>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <unistd.h>
#endif

/**
 * @brief Helper utility to locate the executable's directory portably.
 *
 * This ensures external resources (like map.txt) can be resolved
 * relative to the executable binary regardless of the current working directory.
 */
inline std::string getExecutableDirectory()
{
#ifdef _WIN32
    char buffer[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if(len > 0 && len < MAX_PATH)
    {
        std::string path(buffer);
        size_t lastSlash = path.find_last_of("\\/");
        if(lastSlash != std::string::npos)
        {
            return path.substr(0, lastSlash);
        }
    }
    return "";
#else
    char buffer[1024];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if(len != -1)
    {
        buffer[len] = '\0';
        std::string path(buffer);
        size_t lastSlash = path.find_last_of('/');
        if(lastSlash != std::string::npos)
        {
            return path.substr(0, lastSlash);
        }
    }
    return "";
#endif
}
