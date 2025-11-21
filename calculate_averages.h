#pragma once

#include <vector>

#include "image.h"
#include "debug.h"

// Function to calculate the average signal intensity from each image slice
// Assumes each image has a 5x5 mask region applied beforehand
template<typename T>
std::vector<double> average_Sd(const std::vector<Image<T>>& filtered_images) {
    std::vector<double> S_d;

    for (const auto& slice : filtered_images) {
        double sum = 0.0;
        const int num_pixels = 5 * 5;
        for (int x = 0; x < slice.width(); ++x) {
            for (int y = 0; y < slice.height(); ++y) {
                sum += slice(x, y);
            }
        }
        double average;
        if (num_pixels > 0) {
            average = sum / num_pixels;
        } else {
            average = 0.0;
        }

        S_d.push_back(average);
    }
    return S_d;
}