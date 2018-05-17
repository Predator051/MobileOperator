#ifndef DEFINE
#define DEFINE

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#define WORKER_THREAD_COUNT 4
#define BUFFER_MAX_SIZE 65535

#define LOG_INFO(message) std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" [INF]"<<message<<std::endl;
#define LOG_ERR(message) std::cout<<__FILE__<<":"<<__FUNCTION__<<":"<<__LINE__<<" [ERR]"<<message<<std::endl;

typedef std::vector<uint8_t> ByteBuffer;
typedef std::shared_ptr<ByteBuffer> ByteBufferPtr;

enum class PostgresRole{
    CLIENT = 0,
    ADMIN = 1,
    AUTH = 2
};

enum class ResponseCode {
    status_internal_error = 0,
    status_success = 1,
    status_bad_request = 2,
    status_unknown_command = 3
};

#endif // DEFINE

