#pragma once

#include <vector>

#include "pgm.h"
#include "image.h"
#include "debug.h"
#include "mask.h"

// Computes the average pixel signal in each image of a filtered dataset (e.g. within LVBP region)
template <typename T>
std::vector<double> compute_average_signal(const std::vector<Image<T>>& filtered_images) {
    std::vector<double> signal_averages;

    for (const auto& frame : filtered_images) {
        double sum = 0.0;
        int count = 0;

        for (int x = 0; x < frame.width(); ++x) {
            for (int y = 0; y < frame.height(); ++y) {
                T value = frame(x, y);
                sum += value;
                count++;
            }
        }

        double avg = (count > 0) ? (sum / count) : 0.0;
        signal_averages.push_back(avg);
    }

    return signal_averages;
}

// Stores the peak contrast value and corresponding time frame
struct PeakContrast {
    int time_frame;
    double signal_intensity;
};

// Computes the time frame and value of the maximum signal intensity
PeakContrast find_peak_contrast(const std::vector<double>& averages);

// Stores information about contrast arrival including gradients
struct ContrastArrival {
    int time_frame;
    double signal_intensity;
    std::vector<double> gradients;
};

// Calculates the first noticeable contrast increase in the signal curve
ContrastArrival find_contrast_arrival(const std::vector<double>& averages);

// Computes the temporal gradient between arrival and peak contrast points
double temporal_gradient(const double& S_peak, const double& S_arrival, const int& d_peak, const int& d_arrival);
