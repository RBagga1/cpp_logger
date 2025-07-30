# Simple Logger
This project is a simple logger, with current support for multi-thread safe logging.

I am using this to brush up my C++ skills, so any feedback is welcome.

## Features
* Logger supports:
  * naming the logger
  * 5 different levels of logging
  * thread id for multi-threaded applications
  * filtering based on a specified minimum log level
* Logger implemented with a queue and it's own worker thread to prevent blocking any other important tasks.
  * Conditional variables and locks to ensure the enqueued messages to log are logged in the correct order, and no collisions happen between anything enqueuing messages into the logger.
* Logger will cleanup on it's own when deconstructed.

#### WIP
* Tests using the GoogleTest Library.
* Support for json logs.
* Customization options such as:
  * outputting to command line instead of a file.
  * color of the line outputted.
  * toggling thread ids in log output.
  * toggling color in log output.

#### Possible Tasks
* Log rotation:
  * Creating a new log file after a certain size or time threshold has been reached.
* Writing configuration files for the logger's behavior.
* Support for OS's besides Linux.

## Getting Started
This section will help you get the project up and running on your local machine for development and testing purposes.

#### Prerequisites
You'll need the following tools installed on your system to build the library:
* **Git**
* **CMake** (version 3.15 or higher)
* A C++ compiler that supports **C++17** (GCC, Clang, MSVC, ...)

#### Building
Follow these steps to clone and compile the project.
1. Clone the repo:
```bash
git clone https://github.com/RBagga1/cpp-logger.git
cd cpp-logger
```

2. Configure the project with CMake:
```bash
cmake -B build
```

3. Compile the code:
```bash
cmake --build build
```

After a successful build, the static library will be located at `build/lib/liblogger.a`, and the example executable at `build/bin/example_logger`.

#### Running the Example
To see the logger work, run the example executable from the project's root directory. This will generate a log file in the project's root directory.

```bash
./build/bin/example_logger
```

## Usage
### An Example of how the logger works:

This will run the example and generate a log file in the project's root directory.
```cpp
#include "Logger.h"

int main() {
    // Create a logger instance
    Logger my_logger("MyApp", "my_app.log");

    // Log messages at different levels
    my_logger.info("This is an informational message.");
    my_logger.critical("This is a critical error!");

    return 0; // Logger cleans up automatically
}
```

### Example Log Output
YYYY-MM-DD HH:MM:SS [ thread: 140132... ] [<MyApp> CRITICAL] - This is a critical error!

## Authors
* **Ram Bagga** - [@rbagga1](https://www.github.com/RBagga1)

## License
This project is licensed under the MIT License.