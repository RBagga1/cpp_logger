#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include "logger.h"

void doWork(Logger &logger)
{
    logger.debug(std::to_string(getThreadID()) + " - Doing work");
}

int main()
{
    std::cout << "Main thread id: " << getThreadID() << std::endl;

    const std::string LOGFILE_PATH = "./log.log";
    Logger logger("CVLogger", LOGFILE_PATH);
    logger.info("Logger initialized");
    logger.debug("Debugging information");
    logger.warning("This is a warning");
    logger.error("An error occurred");
    logger.critical("Critical issue encountered");
    std::cout << "Logging completed." << std::endl;

    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++)
    {
        threads.emplace_back(doWork, std::ref(logger));
    }

    for (int i = 0; i < 5; i++)
    {
        threads[i].join();
    }

    FILE *logFile = fopen(LOGFILE_PATH.c_str(), "r");
    if (!logFile)
    {
        std::cerr << "Failed to open log file: " << LOGFILE_PATH << std::endl;
        return 1;
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), logFile))
    {
        std::cout << buffer;
    }

    if (logFile)
    {
        fclose(logFile);
    }
    return 0;
}
