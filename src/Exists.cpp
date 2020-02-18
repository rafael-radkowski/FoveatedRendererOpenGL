#include "Exists.h"
/*!
 Verifies wheterh a file [name] exits
 @param name - the path and the name of the file.
 */
bool Exists (const std::string& name)
{
  //  if (std::experimental::filesystem::exists(name)) {
  //      return true;
  //  } else {
//		return false;
  //  }

    struct stat buffer;
   return (stat (name.c_str(), &buffer) == 0);

}
