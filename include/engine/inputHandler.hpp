#pragma once

#include <iostream>
#include <thread>
#include <mutex>

class InputHandler {
public:

    InputHandler() = default;

    bool inputRecieved();
    std::string getInput();
    void start();
    void join();


private:

    bool _hasInput{false};
    std::mutex _mutex{};
    std::string _input{};
    std::thread _inputThread{};

    void recieveInput();

};