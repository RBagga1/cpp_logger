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
    Logger logger = LoggerBuilder()
                        .setName("CVLogger")
                        .setLogFilePath(LOGFILE_PATH)
                        .setPrintToConsole(true)
                        .build();
    logger.info("Logger initialized");
    logger.debug("Debugging information");
    logger.warning("This is a warning");
    logger.error("An error occurred");
    logger.critical("Critical issue encountered");
    std::cout << "Logging completed." << std::endl;

    for (int i = 0; i < 5; i++)
    {
        logger.info("Main thread is working on task " + std::to_string(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++)
    {
        threads.emplace_back(doWork, std::ref(logger));
    }

    for (int i = 0; i < 5; i++)
    {
        threads[i].join();
    }

    return 0;
}
