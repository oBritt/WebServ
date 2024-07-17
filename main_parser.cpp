
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <unistd.h>


struct location
{
    std::string url;
    std::vector<std::string> allowed;
    std::string root;
    bool autoindex;
    std::vector<std::string> indexes;
    std::vector<std::string> cgi_path;
    std::vector<std::string> cgi_ex;
    std::string returning;
};

struct server
{
    int host;
    int port;
    std::string servername;
    std::map<int, std::string> errorpages;
    int client_max_body_size;
    std::vector<location> locations;
};

int seenRoot = 0;

long long my_atoi(std::string numb)
{
    std::stringstream ss(numb);
    long long num;

    ss >> num;
    return num;
}

int get_ip_as_number(std::string add, int& err)
{
    int out = 0;
    int numb = 0;
    for (int i = 0; i < 3; i++)
    {
        if (add.find('.') == 0)
        {
            err = 1;
            return 0;
        }
        std::string temp = add.substr(0, add.find("."));
        if (temp == "")
        {
            err =1;
            return 0;
        }
        add = add.substr(add.find(".") + 1, add.size());
        out <<= 8;
        numb = my_atoi(temp);
        if (numb > 255)
        {
            err = 1;
            return 0;
        }
        out |= numb;
    }
    out <<= 8;
    numb = my_atoi(add);
    if (add == "")
    {
        err =1;
        return 0;
    }
    if (numb > 255)
    {
        err = 1;
        return 0;
    }
    out |= numb;
    return out;
}

bool isWhiteSpace(char c)
{
    if (c <= 9 && c >= 13)
        return true;
    if (c == ' ')
        return true;
    return false;
}

bool isNumber(std::string& number)
{
    for (int i = 0; i < number.size(); i++)
    {
        if (!(number[i] >= '0' && number[i] <= '9'))
            return false;
    }
    return true;
}

std::string getFistWordAndDelete(std::string& str)
{
    std::string out = "";
    int i = 0;
    for (; i < str.size(); i++)
    {
        if (str[i] == ' ')
            break ;
        out.push_back(str[i]);
    }
    if (i != str.size())
        str = str.substr(i + 1, str.size());
    else
        str = "";
    return out;
}

std::string update_spaces(std::string& input)
{
    std::string output = "";
    int flag = 1;
    for (int i = 0; i < input.size(); i++)
    {
        if (isWhiteSpace(input[i]))
        {
            if (flag)
                continue;
            flag = 1;
            output.push_back(input[i]);
            
            continue;
        }
        if (input[i] == '#')
        {
            if (output.size() >= 1 && isWhiteSpace(output[output.size() - 1]))
                output.pop_back();
            return (output);
        }
        if ((input[i] == '{' || input[i] == '}' || input[i] == ';') && output.back() == ' ')
            output.pop_back();
        output.push_back(input[i]);
        if (input[i] != '{' && input[i] != '}')
            flag = 0;
    }
    if (output.size() >= 1 && isWhiteSpace(output[output.size() - 1]))
        output.pop_back();
    return output;
}

bool areParanthesesOk(std::string& output)
{
    int count = 0;
    int seen = 0;
    for (int i = 0; i < output.size(); i++)
    {
        if (output[i] == '{')
        {
            count++;
            if (count > 2)
                return false;
        }
        if (output[i] == '}')
        {
            count--;
            if (count < 0)
                return false;
        }
    }
    if (count != 0)
        return false;
    return true;
}

std::vector<std::string> getServers(std::string& output)
{
    std::vector<std::string> returnValue;
    int count = 0;
    int current = 0;
    int prev = 0;
    for (int i = 0; i < output.size(); i++)
    {
        if (output[i] == '{')
        {
            count++;
        }
        if (output[i] == '}')
        {
            count--;
            if (count == 0)
            {
                returnValue.push_back(output.substr(prev, current - prev + 1));
                // std::cout << output.substr(prev, current - prev + 1)<< std::endl;
                prev = current + 1;
            }
        }
        current++;
    }
    return returnValue;
}

bool isLocation(std::string& serv)
{
    for (int i = 0; i < serv.size(); i++)
    {
        if (serv[i] == '{')
            return true;
        else if (serv[i] == ';')
            return false;
    }
    return true;
}

void handleHost(std::string &ip, server& s, int& err)
{
    ip.pop_back();
    if (!(ip.size() >= 7 && ip.size() <= 15))
    {
        err =1;
        return ;
    }
    int count = 0;
    for (int i = 0; i < ip.size(); i++)
    {
        if (!(ip[i] == '.' || (ip[i] >= '0' && ip[i] <= '9')))
        {
            err = 1;
            return ;
        }
        if (ip[i] == '.')
            count++;
    }
    if (count != 3)
    {
        err = 1;
        return ;
    }
    s.host = get_ip_as_number(ip, err);
}

void handlePort(std::string &port, server& s, int& err)
{
    port.pop_back();
    if (port.size() == 0 || port.size() > 5 || !isNumber(port))
    {
        err = 1;
        return ;
    }
    int temp = my_atoi(port);
    if (temp > 0xFFFF)
    {
        err = 1;
        return ;
    }
    s.port = temp;
}

void handleServerName(std::string& serverName, server& s, int& err)
{
    serverName.pop_back();
    if (serverName == "_")
        return ;
    if (serverName.find(' ') != std::string::npos)
    {
        err = 1;
        return ;
    }
    s.servername = serverName; 
}

void handleErrorPage(std::string& errorPage, server& s, int& err)
{
    errorPage.pop_back();
    if (errorPage.find(' ') == std::string::npos)
    {
        err = 1;
        return ;
    }
    std::string errCode = getFistWordAndDelete(errorPage);
    if (errCode.size() > 9 || !isNumber(errCode))
    {
        err = 1;
        return;
    }
    long long error = my_atoi(errCode);
    if (error > 2147483647)
    {
        err = 1;
        return ;
    }
    s.errorpages[error] = errorPage;
}

void handleMaxBody(std::string& body, server& s, int& err)
{
    body.pop_back();
    if (body.find(' ') != std::string::npos)
    {
        err = 1;
        return ;
    }
    if (body.size() > 9 || !isNumber(body))
    {
        err = 1;
        return;
    }
    long long numb = my_atoi(body);
    if (numb > 2147483647)
    {
        err = 1;
        return ;
    }
    s.client_max_body_size = numb;
}

void add_to_loc(std::string& temp, location& loc, int & err)
{
    std::string possibles[4] = {"PUT", "GET", "DELETE", "POST"};

    int i = 0;
    for (; i < 4; i++)
    {
        if (possibles[i] == temp)
        {
            break;
        }
    }
    if (i == 4)
    {
        err = 1;
        return ;
    }
    if (std::find(loc.allowed.begin(), loc.allowed.end(), possibles[i]) == loc.allowed.end())
    {
        loc.allowed.push_back(possibles[i]);
    }
}

void handleIndex(std::string& indexes, location& loc)
{
    while (indexes.find(' ') != std::string::npos)
    {
        loc.indexes.push_back(getFistWordAndDelete(indexes));
    }
    loc.indexes.push_back(indexes);
}


void handleAllowMethods(std::string& methods, location &loc, int& err)
{
    std::string temp;
    while (methods.find(' ') != std::string::npos)
    {
        temp = methods.substr(0, methods.find(' '));
        methods = methods.substr(methods.find(' ') + 1, methods.size());
        add_to_loc(temp, loc, err);
    }
    add_to_loc(methods, loc, err);
}

void handleAutoindex(std::string& str, location &loc, int &err)
{
    if (str == "on" || str == "off")
    {
        loc.autoindex = (str == "on");
    }
    else
        err = 1;
}

void handleCgiPath(std::string& path, location& loc, int &err)
{
    std::string temp; 
    while (path.find(' ') != std::string::npos)
    {
        temp = path.substr(0, path.find(' '));
        path = path.substr(path.find(' ') + 1, path.size());
        if (temp[0] != '/' || access(temp.c_str(), F_OK) != 0)
        {
            err = 1;
            return ;
        }
        if (find(loc.cgi_path.begin(), loc.cgi_path.end(), temp) == loc.cgi_path.end())
            loc.cgi_path.push_back(temp);
    }
    temp = path;
    if (temp[0] != '/' || access(temp.c_str(), F_OK) != 0)
    {
        err = 1;
        return ;
    }
    if (find(loc.cgi_path.begin(), loc.cgi_path.end(), temp) == loc.cgi_path.end())
        loc.cgi_path.push_back(temp);
}

void handleCgiEx(std::string& exec, location& loc, int &err)
{
    std::string temp; 
    while (exec.find(' ') != std::string::npos)
    {
        temp = exec.substr(0, exec.find(' '));
        exec = exec.substr(exec.find(' ') + 1, exec.size());
        if (temp[0] != '.')
        {
            err = 1;
            return ;
        }
        if (find(loc.cgi_ex.begin(), loc.cgi_ex.end(), temp) == loc.cgi_ex.end())
            loc.cgi_ex.push_back(temp);
    }
    temp = exec;
    if (temp[0] != '.')
    {
        err = 1;
        return ; 
    }
    if (find(loc.cgi_ex.begin(), loc.cgi_ex.end(), temp) == loc.cgi_ex.end())
        loc.cgi_ex.push_back(temp);
}

void handleReturn(std::string& curLoc, location& loc, int &err)
{
    loc.returning = curLoc;
}

void handleParamLocation(std::string& curLoc, location& loc, int &err)
{
    if (curLoc[0] == ' ')
    {
        curLoc = curLoc.substr(1, curLoc.size());
    }
    if (curLoc.find(';') == std::string::npos)
    {
        err  = 1;
        return ;
    }
    loc.autoindex = true;
    std::string param = curLoc.substr(0, curLoc.find(';'));
    curLoc = curLoc.substr(curLoc.find(';') + 1, curLoc.size());
    if (param.find(' ') == std::string::npos)
    {
        err = 1;
        return ;
    }
    std::string category = param.substr(0, param.find(' '));
    param = param.substr(param.find(' ') + 1);
    std::cout << category;
    if (category == "allowed_methods")
    {
        handleAllowMethods(param, loc, err);
    }
    else if (category == "index")
    {
        handleIndex(param, loc);
    }
    else if (category == "autoindex")
    {
        handleAutoindex(param, loc, err);
    }
    else if (category == "cgi_path")
    {
        handleCgiPath(param, loc, err);
    }
    else if (category == "cgi_ex")
    {
        handleCgiEx(param, loc, err);
    }
    else if (category == "return")
    {
        handleReturn(param, loc, err);
    }
    else
    {
        err = 1;
        return ;
    }
}

void handleLocation(std::string& Location, server& s, int& err)
{
    if (Location[0] == ' ')
    {
        Location = Location.substr(1, Location.size());
    }
    std::string currentLoc = Location.substr(0, Location.find('}') + 1);
    Location = Location.substr(Location.find('}') + 1, Location.size());
    if (currentLoc.find(' ') == std::string::npos)
    {
        err = 1;
        return;
    }
    std::string check = currentLoc.substr(0, currentLoc.find(' '));
    if (check != "location")
    {
        err = 1;
        return ;
    }
    currentLoc = currentLoc.substr(currentLoc.find(' ') + 1, currentLoc.size());
    check = currentLoc.substr(0, currentLoc.find('{'));
    if (check.size() == 0)
    {
        err = 1;
        return ;
    }
    currentLoc = currentLoc.substr(currentLoc.find('{') + 1, currentLoc.size());
    int i = 0;
    location loc;
    loc.root = "";
    loc.url = "";
    loc.returning = "";
    while (currentLoc != "}" && currentLoc != " }")
    {
        handleParamLocation(currentLoc, loc, err);
        if (err)
            return ;
        i++;
        
    }
    if ((loc.returning != "" && i != 1) || loc.root == "" || loc.indexes.size() == 0 || loc.cgi_ex.size() != loc.cgi_path.size())
    {
        err = 1;
        return ;
    }
    s.locations.push_back(loc);
}

void handleNotLocation(std::string& serv, server& s, int &err)
{
    if (serv[0] == ' ')
    {
        serv = serv.substr(1, serv.size());
    }
    std::string temp = serv.substr(0, serv.find(';') + 1);
    
    serv = serv.substr(serv.find(';') + 1, serv.size());

    std::string category = getFistWordAndDelete(temp);
    
    std::cout << category << std::endl;
    if (category == "host")
    {
        handleHost(temp, s, err);
    }
    else if (category == "port")
    {
        handlePort(temp, s, err);
    }
    else if (category == "server_name")
    {
        handleServerName(temp, s, err);
    }
    else if (category == "error_page")
    {
        handleErrorPage(temp, s, err);
    }
    else if (category == "client_max_body_size")
    {
        handleMaxBody(temp, s, err);
    }
    else
    {
        err = 1;
        return;
    }
}

server createServer(std::string& serv, int& err)
{
    server s;
    serv = serv.substr(7, serv.size());
    s.host = get_ip_as_number("127.0.0.1", err);
    s.port = 8080;
    s.servername = "";
    s.client_max_body_size = 1024;
    while (serv[0] != '}')
    {
        if (isLocation(serv))
        {
            handleLocation(serv, s, err);
        }
        else
        {
            handleNotLocation(serv, s, err);
        }
        if (err)
            return s;
    }
    return s;
}

int parse(std::string path)
{
    std::ifstream inputFile(path);
    if (!inputFile)
    {
        std::cerr << "Couldn't open " << path << std::endl;
        return (1);
    }
    std::string output = "";
    std::string line = "";
    int curly = 0;
    while (std::getline(inputFile, line))
    {
        line = update_spaces(line);
        output += line + " ";
    }
    output = update_spaces(output);
    // std::cout << output;
    if (!areParanthesesOk(output))
        return 1;
    std::vector<std::string> servers = getServers(output);
    std::vector<server> s;
    int err = 0;
    for (int i = 0; i < servers.size(); i++)
    {
        seenRoot = 0;
        if (servers[i].substr(0, 7) != "server{")
            return 1;
        s.push_back(createServer(servers[i], err));
        if (err || !seenRoot)
            return 1;
        std::cout << s[s.size() - 1].client_max_body_size << std::endl;
    }
    return 0;
}

int main()
{
    if (parse("/Users/obrittne/Desktop/webSerc/Configs/default.conf"))
    {
        std::cout << "error" << std::endl;
        return (1);
    }
}