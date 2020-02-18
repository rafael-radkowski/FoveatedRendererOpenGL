#pragma once

#include <iostream>
#include <fstream>
#include <sys/stat.h>

/*!
 Verifies wheterh a file [name] exits
 @param name - the path and the name of the file.
 */
bool Exists (const std::string& name);