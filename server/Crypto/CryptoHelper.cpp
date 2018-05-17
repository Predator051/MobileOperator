#include "CryptoHelper.h"
#include <openssl/md5.h>

std::string toHex(const char *pchData, int count)
{
    std::string s;
    for(int i=0; i<count; ++i) {
        unsigned char ch = pchData[i];
        unsigned char lo = ch%16;
        unsigned char hi = ch/16;

        s.push_back((hi<10)?(hi+0x30):(hi+87));
        s.push_back((lo<10)?(lo+0x30):(lo+87));
    }

    return s;
}

void CryptoHelper::md5_hash(const std::string &word, std::string &hash_word)
{
//    unsigned char digest[MD5_DIGEST_LENGTH];

//    MD5(reinterpret_cast<const unsigned char*>(word.c_str()), word.length(), (unsigned char*)&digest);

//    char mdString[33];

//    for(int i = 0; i < 16; i++)
//        sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

//    hash_word = mdString;
    const int hash_length = 16;
    char hash[hash_length];

    MD5_CTX Md5Ctx;

    MD5_Init(&Md5Ctx);
    std::string copyWord = word;
    while(true)
    {
        if(copyWord.length() < hash_length)
        {
            MD5_Update(&Md5Ctx, (unsigned char *)copyWord.c_str(), copyWord.length());
            break;
        }

        std::string substr = copyWord.substr(0, hash_length);
        MD5_Update(&Md5Ctx, (unsigned char *)substr.c_str(), substr.length());
        copyWord.erase(0, hash_length);
    }
    MD5_Final((unsigned char *)hash, &Md5Ctx);

    hash_word = toHex(hash, hash_length);
}

std::string CryptoHelper::gen_random_string(int len) {
    srand(time(NULL));
    std::string s;
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return s;
}
