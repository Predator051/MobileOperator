#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <libconfig.h++>
#include <string>

class ConfigManager
{
public:
    ConfigManager();

    bool readConfigFIle(const std::string& path);
    void parseConfigs();
private:
    libconfig::Config conf;
};

#endif // CONFIGMANAGER_H
