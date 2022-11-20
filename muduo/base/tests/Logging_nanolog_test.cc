//
// Created by 杨丰硕 on 2022/11/20.
//

#include "muduo/base/Logging.h"
#include "muduo/base/LogFile.h"
#include "muduo/base/ThreadPool.h"
#include "muduo/base/TimeZone.h"

#include "muduo/runtime/NanoLogCpp17.h"

#include <stdio.h>
#include <unistd.h>

int g_total;
FILE* g_file;

void bench(const char* type)
{
    NanoLog::setLogFile(type);
    muduo::Timestamp start(muduo::Timestamp::now());
    g_total = 0;

    int n = 1000*1000;
    const bool kLongLog = false;
    muduo::string empty = " ";
    muduo::string longStr(3000, 'X');
    longStr += " ";
    for (int i = 0; i < n; ++i)
    {
        NANO_LOG(NOTICE, "Hello 0123456789 abcdefghijklmnopqrstuvwxyz%s%d",
                 (kLongLog? longStr.c_str() : empty.c_str()), i);
    }
    muduo::Timestamp end(muduo::Timestamp::now());
    double seconds = timeDifference(end, start);
    /*printf("%12s: %f seconds, %d bytes, %10.2f msg/s, %.2f MiB/s\n",
           type, seconds, g_total,
           n / seconds,
           g_total / seconds / (1024 * 1024));*/
    printf("%12s: %f seconds, %10.2f msg/s\n",
           type, seconds,
           n / seconds);
}

void logInThread()
{
    NANO_LOG(NOTICE, "logInThread");
    usleep(1000);
}

int main() {
    getppid();

    NanoLog::setLogFile("/tmp/nanologfile");
    muduo::ThreadPool pool("pool");
    pool.start(5);
    pool.run(logInThread);
    pool.run(logInThread);
    pool.run(logInThread);
    pool.run(logInThread);
    pool.run(logInThread);
    sleep(1);

    bench("nop");

    g_file = fopen("/dev/null", "w");
    bench("/dev/null");
    sleep(1);
    fclose(g_file);

    g_file = fopen("/tmp/log", "w");
    bench("/tmp/log");
    sleep(1);
    fclose(g_file);


    g_file = NULL;
    bench("test_log_st");

    bench("test_log_mt");

}