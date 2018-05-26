#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <iostream>
#include <map>
#include <QVariantMap>

using KeyValueMap = std::map<std::string, std::string>;

class CacheManager
{
public:
    static CacheManager& instance(const std::string& session_id);

    void makeCache(const KeyValueMap& keyValue);
    std::string getValue(const std::string& key);
    KeyValueMap getCache();
    void saveSession(const std::string& session);
    std::string getLastSession();
private:
    CacheManager(const std::string& session);
    ~CacheManager() {}

    CacheManager(CacheManager const&);
    CacheManager& operator= (CacheManager const&);

    std::string session_;

    bool readJsonFile(std::string file_path, QVariantMap& result);

    // writes a QVariantMap to disk
    bool writeJsonFile(const QVariantMap &point_map, const QString &file_path);
};

#endif // CACHEMANAGER_H
