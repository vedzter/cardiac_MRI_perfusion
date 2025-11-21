#include <vector>
#include <stdexcept>

#include "calculation.h"
#include "debug.h"

// Function to find the time frame containing the peak contrast concentration
PeakContrast find_peak_contrast(const std::vector<double>& averages) {
    if (averages.empty()) {
        throw std::runtime_error("find_peak_contrast: Input vector 'averages' is empty.");
    }

    debug::log("Identifying peak contrast concentration...");

    PeakContrast peak;
    peak.signal_intensity = 0.0;
    peak.time_frame = -1;

    // Identify the largest average value
    for (size_t i = 0; i < averages.size(); ++i) {
        double avg = averages[i];
        if (avg > peak.signal_intensity) {
            peak.signal_intensity = avg;
            peak.time_frame = static_cast<int>(i);
        }
    }

    if (peak.time_frame == -1) {
        throw std::runtime_error("find_peak_contrast: Peak contrast frame not found.");
    }

    return peak;
}

// Function to identify the time frame when contrast agent first arrives
ContrastArrival find_contrast_arrival(const std::vector<double>& averages) {
    if (averages.empty()) {
        throw std::runtime_error("find_contrast_arrival: Input vector 'averages' is empty.");
    }

    debug::log("Detecting contrast arrival...");

    ContrastArrival arrival;
    arrival.signal_intensity = 0.0;
    arrival.time_frame = -1;

    // Compute the temporal gradient between consecutive time points
    for (size_t d = 0; d + 1 < averages.size(); ++d) {
        double gradient = averages[d + 1] - averages[d];
        arrival.gradients.push_back(gradient);
    }

    // Append 0.0 for the last frame to match the original logic
    arrival.gradients.push_back(0.0);

    // Find the first time frame where the gradient exceeds a threshold (e.g. 10)
    for (size_t i = 0; i < arrival.gradients.size(); ++i) {
        if (arrival.gradients[i] > 10.0) {
            arrival.signal_intensity = averages[i];
            arrival.time_frame = static_cast<int>(i);
            break;
        }
    }

    if (arrival.time_frame == -1) {
        throw std::runtime_error("find_contrast_arrival: No significant contrast arrival detected.");
    }

    return arrival;
}

// Function to compute the temporal gradient between arrival and peak frames
double temporal_gradient(const double& S_peak, const double& S_arrival, const int& d_peak, const int& d_arrival) {
    if (d_peak == d_arrival) {
        throw std::runtime_error("temporal_gradient: Time frames for peak and arrival are identical.");
    }

    debug::log("Calculating temporal gradient...");

    return (S_peak - S_arrival) / static_cast<double>(d_peak - d_arrival);
}