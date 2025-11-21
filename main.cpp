#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>

#include "debug.h"
#include "contrastagent.h"
#include "terminal_graphics.h"
#include "pgm.h"
#include "calculation.h"
#include "mask.h"
#include "dataset.h"
#include "calculate_averages.h"

void run(std::vector<std::string>& args) {
    debug::verbose = std::erase(args, "-v");

    // Initialisation
    std::string contrast_file;
    std::vector<std::string> image_files;
    
    int regionSize = 5;         // LVBP mask size
    int centerX = 74, centerY = 90;  // Mask center

    // Load in arguments
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i].ends_with(".txt")) {
            contrast_file = args[i];
        } else if (args[i].ends_with(".pgm")) {
            image_files.push_back(args[i]);
        } else if (args[i] == "mask_width" || args[i] == "mask_height") {
            if (i + 1 < args.size()) {
                regionSize = std::stoi(args[++i]);
            }
        } else if (args[i] == "mask_center_x" && i + 1 < args.size()) {
            centerX = std::stoi(args[++i]);
        } else if (args[i] == "mask_center_y" && i + 1 < args.size()) {
            centerY = std::stoi(args[++i]);
        }
    }

    if (contrast_file.empty()) {
        throw std::runtime_error("Missing contrast info file (.txt)");
    }

    if (image_files.empty()) {
        throw std::runtime_error("No PGM image files provided.");
    }

    constexpr int term_width = 900;
    constexpr int term_height = 500;

    // Load and preprocess data
    ContrastAgent<double> contrast_info = loadContrastInfo<double>(contrast_file);
    Dataset<int> images(image_files);

    MaskFilter<int> filter(
        images[0].width(), images[0].height(),
        regionSize, regionSize,
        centerX, centerY
    );

    Image<int> mask = filter.create_binary_mask();
    
    std::vector<Image<int>> filtered_images = filter.apply_mask(images, mask);
    std::vector<double> S_d = average_Sd(filtered_images);

    if (S_d.empty()) {
        throw std::runtime_error("Signal data (S_d) is empty. Check image and mask dimensions.");
    }

    // Compute contrast characteristics
    PeakContrast peak_contrast = find_peak_contrast(S_d);
    ContrastArrival contrast_arrival = find_contrast_arrival(S_d);

    if (contrast_arrival.time_frame < 0 || peak_contrast.time_frame < 0) {
        throw std::runtime_error("Failed to compute contrast arrival or peak.");
    }

    double gradient = temporal_gradient(
        peak_contrast.signal_intensity,
        contrast_arrival.signal_intensity,
        peak_contrast.time_frame,
        contrast_arrival.time_frame
    );

    // Output results
    std::cout << "\nImage at peak contrast concentration:\n";
    TG::imshow(TG::magnify(images[peak_contrast.time_frame], 4), 0, 255);

    std::cout << "\nSignal timecourse within ROI:\n";
    TG::plot(term_width, term_height)
        .add_line(S_d);

    std::cout << "\nGradient of signal timecourse within ROI:\n";
    TG::plot(term_width, term_height)
        .add_line(contrast_arrival.gradients, 3);

    std::cout << "\n--- Contrast Analysis ---\n";
    std::cout << "Contrast agent: " << contrast_info.agent_name
              << ", dose: " << contrast_info.agent_dose << " mmol/kg\n";
    std::cout << "Contrast arrival at frame " << contrast_arrival.time_frame
              << ", intensity: " << contrast_arrival.signal_intensity << "\n";
    std::cout << "Peak contrast at frame " << peak_contrast.time_frame
              << ", intensity: " << peak_contrast.signal_intensity << "\n";
    std::cout << "Temporal gradient during contrast uptake: " << gradient << "\n\n";
}

int main(int argc, char* argv[]) {
    try {
        std::vector<std::string> args(argv, argv + argc);
        run(args);
    }
    catch (const std::exception& exc) {
        std::cerr << "ERROR: " << exc.what() << " - aborting\n";
    }
    catch (...) {
        std::cerr << "ERROR: Unknown exception - aborting\n";
        return 1;
    }

    return 0;
}