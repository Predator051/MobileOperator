#ifndef CRYPTOHELPER_H
#define CRYPTOHELPER_H

#include <string>
#include <random>

class CryptoHelper
{
public:
    static void md5_hash(const std::string& word, std::string& hash_word);
    static std::string gen_random_string(int len);
};

#endif // CRYPTOHELPER_H
