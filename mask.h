#pragma once

#include <iostream>
#include <vector>

#include "debug.h"
#include "image.h"
#include "dataset.h"

// Class to define and apply a binary mask over a dataset of images.
// The mask is centered around a given coordinate and spans a kernel of defined width and height.
template <typename T>
class MaskFilter : public Image<T> {
public:
    // Constructor
    MaskFilter(int width, int height, int kernel_w = 5, int kernel_h = 5, int center_x = 74, int center_y = 90)
        : Image<T>(width, height),
          kernel_width(kernel_w),
          kernel_height(kernel_h),
          center_x(center_x),
          center_y(center_y) {}

    // Generates a binary mask with a square region centered on (center_x, center_y)
    Image<T> create_binary_mask() const;

    // Applies the given binary mask to each image in the dataset
    std::vector<Image<T>> apply_mask(const Dataset<T>& dataset, const Image<T>& mask) const;

private:
    int kernel_width, kernel_height;
    int center_x, center_y;
};

template <typename T>
Image<T> MaskFilter<T>::create_binary_mask() const {
    debug::log("Creating binary mask:");

    Image<T> mask(this->width(), this->height());

    int half_w = kernel_width / 2;
    int half_h = kernel_height / 2;

    for (int y = center_y - half_h; y <= center_y + half_h; ++y) {
        for (int x = center_x - half_w; x <= center_x + half_w; ++x) {
            mask(x, y) = 1;
        }
    }

    return mask;
}

template <typename T>
std::vector<Image<T>> MaskFilter<T>::apply_mask(const Dataset<T>& dataset, const Image<T>& mask) const {
    std::vector<Image<T>> result;

    for (const auto& frame : dataset) {
        Image<T> masked(frame.width(), frame.height());

        for (int x = 0; x < frame.width(); ++x) {
            for (int y = 0; y < frame.height(); ++y) {
                masked(x, y) = frame(x, y) * mask(x, y);
            }
        }

        result.push_back(masked);
    }

    return result;
}
