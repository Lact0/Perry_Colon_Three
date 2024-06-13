#include "inputHandler.h"

bool InputHandler::inputRecieved() {
    bool hasInput;
    
    _mutex.lock();
    hasInput = _hasInput;
    _mutex.unlock();

    return hasInput;
}

std::string InputHandler::getInput() {
    std::string input = _input;
    
    _mutex.lock();
    _hasInput = false;
    _mutex.unlock();

    join();
    start();
    return input;
}

void InputHandler::start() {
    _inputThread = std::thread(&InputHandler::recieveInput, this);
}

void InputHandler::join() {
    if(_inputThread.joinable()) _inputThread.join();
}

void InputHandler::recieveInput() {
    _mutex.lock();
    _hasInput = false;
    _mutex.unlock();
    
    std::getline(std::cin, _input);

    _mutex.lock();
    _hasInput = true;
    _mutex.unlock();
}
