#include "inputHandler.h"

std::string InputHandler::getInput() {
    join();
    start();
    _hasInput = false;
    return _input;
}

void InputHandler::start() {
    _inputThread = std::thread(&InputHandler::recieveInput, this);
}

void InputHandler::join() {
    if(_inputThread.joinable()) _inputThread.join();
}

void InputHandler::recieveInput() {
    _hasInput = false;
    std::getline(std::cin, _input);
    _hasInput = true;
}
