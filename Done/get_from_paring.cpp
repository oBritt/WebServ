
#include "../Tomweb.hpp"


bool checkIfFileExistsAndNotDirectory(std::string& path)
{
    struct stat path_stat;

    if (stat(path.c_str(), &path_stat) == 0) {
        // Check if it is a regular file
        if (S_ISREG(path_stat.st_mode)) {
            return true;
        } else {
            return false;
        }
    }
    return false;
}

location get_location(std::vector<location>& locations, std::string& path)
{
    std::string closest_so_far = "";
    int ind = 0;
    for (int i = 0; i < locations.size(); i++)
    {
        if (locations[i].URI.size() > path.size())
            continue;

        if (path.find(locations[i].URI) == 0)
        {
            if (path.size() == locations[i].URI.size())
            {
                return locations[i];
            }
            else if (locations[i].URI == "/")
            {
                if (closest_so_far == "")
                {
                    closest_so_far = locations[i].URI;
                    ind = i;
                }
            }
            else if (path[locations[i].URI.size()] == '/')
            {
                if (locations[i].URI.size() > closest_so_far.size())
                {
                    closest_so_far = locations[i].URI;
                    ind = i;
                }
            }
        }
    }
    if (closest_so_far == "")
    {
        location loc;
        loc.doesntExist = 1;
        return loc;
    }
    return locations[ind];
}

std::vector<std::string> get_allowed(location& loc)
{
    if (loc.allowed.size() == 0)
    {
        std::vector<std::string> vec(4);
        vec[0] = "POST";
        vec[1] = "DELETE";
        vec[2] = "GET";
        vec[3] = "GET";
        return vec;
    }
    return loc.allowed;
}

bool isAllowed(location& loc, std::string& method)
{
    std::vector<std::string> allowed = get_allowed(loc);
    if (find(allowed.begin(), allowed.end(), method) != allowed.end())
        return true;
    return false;
}

//should be valid errorNumb if defind new please add it to function

std::string get_path_of_standart_error(int errorNumb)
{
    std::map<int, std::string> mp;

    return mp[errorNumb];
}

std::string get_error_page(Server& serv, int errorNumb)
{
    std::map<int, std::string>::iterator it = serv.errorPages.find(errorNumb);
    if (it == serv.errorPages.end())
    {
        return get_path_of_standart_error(errorNumb);
    }
    else
    {
        std::string path = it->second;
        if (path[0] == '/')
            path = path.substr(1);
        if (checkIfFileExistsAndNotDirectory(path))
        {
            return path;
        } 
        else
        {
            return get_path_of_standart_error(errorNumb);
        }
    }
}