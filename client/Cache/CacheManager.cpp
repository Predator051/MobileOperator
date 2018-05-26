#include "CacheManager.h"
#include <QTextStream>
#include <QString>
#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "define.h"

CacheManager &CacheManager::instance(const std::string &session_id)
{
    static CacheManager cache("");
    cache.session_ = session_id;

    return cache;
}

void CacheManager::makeCache(const KeyValueMap &keyValue)
{
    QDir dir("./");

    dir.mkpath("AppCache/" + QString::fromStdString(session_));

    KeyValueMap lastCache = getCache();

    QVariantMap map;

    for(const std::pair<std::string, std::string>& pair: lastCache)
    {
        map.insert(QString::fromStdString(pair.first), QString::fromStdString(pair.second));
    }


    for(const std::pair<std::string, std::string>& pair: keyValue)
    {
        map.insert(QString::fromStdString(pair.first), QString::fromStdString(pair.second));
    }


    writeJsonFile(map, "AppCache/" + QString::fromStdString(session_) + "/cache.json");
}

std::string CacheManager::getValue(const std::string &key)
{

}

KeyValueMap CacheManager::getCache()
{
    KeyValueMap result;
    QVariantMap cache;

    if(!readJsonFile("AppCache/" + session_ + "/cache.json", cache))
    {
        return result;
    }

    QMapIterator<QString, QVariant> i(cache);
    while(i.hasNext())
    {
        i.next();
        result[i.key().toStdString()] = i.value().toString().toStdString();
    }

    return result;
}

void CacheManager::saveSession(const std::string &session)
{
    QDir dir("./");

    dir.mkpath("AppCache");
    QVariantMap map;
    map.insert(QString("lastSession"), QString::fromStdString(session));

    writeJsonFile(map, "AppCache/session.json");
}

std::string CacheManager::getLastSession()
{
    QVariantMap map;
    readJsonFile("AppCache/session.json", map);

    return map["lastSession"].toString().toStdString();
}

CacheManager::CacheManager(const std::string &session)
{
    session_ = session;
}

bool CacheManager::readJsonFile(std::string file_path, QVariantMap &result)
{
    // step 1
    QFile file_obj(QString::fromStdString(file_path));
    if (!file_obj.open(QIODevice::ReadOnly)) {
        std::cout << "Failed to open " << file_path << std::endl;
        return false;
    }

    // step 2
    QTextStream file_text(&file_obj);
    QString json_string;
    json_string = file_text.readAll();
    file_obj.close();
    QByteArray json_bytes = json_string.toLocal8Bit();

    // step 3
    auto json_doc = QJsonDocument::fromJson(json_bytes);

    if (json_doc.isNull()) {
        std::cout << "Failed to create JSON doc." << std::endl;
        return false;
    }
    if (!json_doc.isObject()) {
        std::cout << "JSON is not an object." << std::endl;
        return false;
    }

    QJsonObject json_obj = json_doc.object();

    if (json_obj.isEmpty()) {
        std::cout << "JSON object is empty." << std::endl;
        return false;
    }

    // step 4
    result = json_obj.toVariantMap();
    return true;
}

bool CacheManager::writeJsonFile(const QVariantMap& point_map, const QString& file_path)
{
    QJsonObject json_obj = QJsonObject::fromVariantMap(point_map);
    QJsonDocument json_doc(json_obj);
    QString json_string = json_doc.toJson();

    QFile save_file(file_path);
    !save_file.open(QIODevice::WriteOnly);

    save_file.write(json_string.toLocal8Bit());
    save_file.close();
    return true;
}
