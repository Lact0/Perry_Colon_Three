#pragma once

#include <iostream>
#include <thread>

class InputHandler {
public:

    InputHandler() = default;

    bool inputRecieved() {return _hasInput;}
    std::string getInput();
    void start();
    void join();


private:

    bool _hasInput{false};
    std::string _input{};
    std::thread _inputThread{};

    void recieveInput();

};