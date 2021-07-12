#include "ConfigHandler.h"

#include <iostream>

int main()
{
    cfg::ConfigHandler handler;

    handler.createDefaultConfig();

    std::cout << "created Config" << std::endl;

    cfg::CalibrationConfig data = handler.readConfig();

    std::cout << data << std::endl;
}