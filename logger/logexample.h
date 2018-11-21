#include <thread>
#include <chrono>
#include "logger.h"

using namespace std;


void task1()
{
    for (int i=10; i>0; --i) {
        this_thread::sleep_for (std::chrono::milliseconds(210));
        Logger::Write(Logger::DEBUG, "This message comes from task1");
    }
}
void task2()
{
    for (int i=10; i>0; --i) {
        this_thread::sleep_for (std::chrono::milliseconds(320));
        Logger::Write(Logger::WARNING, "This message comes from task2");
    }
}
void task3()
{
    for (int i=10; i>0; --i) {
        this_thread::sleep_for (std::chrono::milliseconds(230));
        Logger::Write(Logger::ERROR, "This message comes from task3");
    }
}

int main(int argc, char *argv[])
{
Logger::Start(Logger::DEBUG, "/Users/d042762/a.log");
try
{
    thread t1(task1);
    thread t2(task2);
    thread t3(task3);

    t1.join();
    t2.join();
    t3.join();

 }
catch (const string& str)
{
    Logger::Write(Logger::ERROR, "exception caught: " + str);
    exit(EXIT_FAILURE);
}

    Logger::Write(Logger::INFO, "End of example programm");
    Logger::Stop();
    return 0;
}
