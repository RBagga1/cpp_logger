#include "logger.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

const std::filesystem::path LOG_FILE_PATH = "example.log";

// A simple function that simulates work and logs a message
void doWork(Logger &logger, int task_id)
{
    logger.info("Worker thread " + std::to_string(getThreadID()) + " starting task " + std::to_string(task_id));
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    logger.debug("Worker thread " + std::to_string(getThreadID()) + " finished task " + std::to_string(task_id));
}

int main()
{
    // --- Advanced Logger Configuration ---
    // Use the LoggerBuilder for a fluent and readable setup.
    // This logger will write to both a file and the console.
    std::cout << "Initializing advanced logger..." << std::endl;
    Logger advanced_logger = LoggerBuilder()
                                 .setName("WebApp")
                                 .setLogFilePath(LOG_FILE_PATH)
                                 .setMinimumLogLevel(LogLevel::DEBUG)
                                 .setPrintToConsole(true)
                                 .setLogThreadIDs(true)
                                 .setLogSelfName(true)
                                 .build();

    advanced_logger.critical("This is a critical issue from the 'WebApp' logger.");
    advanced_logger.info("Logger initialized. Starting main application tasks.");

    // --- Main Thread Work ---
    for (int i = 0; i < 3; ++i)
    {
        advanced_logger.info("Main thread is working on task " + std::to_string(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    // --- Multi-threaded Logging ---
    // thread-safe logging from multiple worker threads.
    std::vector<std::thread> threads;
    advanced_logger.info("Spawning 5 worker threads...");
    for (int i = 0; i < 5; ++i)
    {
        // std::ref() is used to pass the logger by reference to the thread.
        threads.emplace_back(doWork, std::ref(advanced_logger), i);
    }

    // Wait for all threads to complete their work.
    for (auto &t : threads)
    {
        t.join();
    }

    advanced_logger.info("All worker threads have finished.");
    std::cout << "Example finished. Check " << LOG_FILE_PATH << " and console for output." << std::endl;

    return 0;
}
