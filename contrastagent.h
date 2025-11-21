#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>

// Struct to hold contrast agent parameters: name and dose (in mmol/kg)
template <typename T>
struct ContrastAgent {
    std::string agent_name;
    double agent_dose;
};

// Reads contrast agent information from "data/" directory.
// File should have agent name on line 1 and dose (mmol/kg) on line 2.
template <typename T>
ContrastAgent<T> loadContrastInfo(const std::string& filename)
{
    const std::string full_path = "data/" + filename;
    std::ifstream infile(full_path);

    if (!infile.is_open())
        throw std::runtime_error("Cannot open contrast info file: " + full_path);

    ContrastAgent<T> agent_info;
    std::getline(infile, agent_info.agent_name);

    if (infile.fail() || agent_info.agent_name.empty())
        throw std::runtime_error("Failed to read agent name from " + full_path);

    infile >> agent_info.agent_dose;

    if (agent_info.agent_dose <= 0)
        throw std::runtime_error("Invalid contrast dose in file: " + full_path);

    return agent_info;
}
