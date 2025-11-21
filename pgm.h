#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <format>

#include "image.h"
#include "debug.h"

// Loads a PGM image (ASCII, P2 format) from the "data/" directory and returns an Image object.
// This function supports only max pixel values of 255 (8-bit grayscale).
template <typename T>
Image<T> load_pgm(const std::string& filename)
{
  const std::string full_path = "data/" + filename;
  debug::log("Attempting to read PGM file: \"" + full_path + "\"");

  std::ifstream pgm_file(full_path);
  if (!pgm_file.is_open())
    throw std::runtime_error("Cannot open file: \"" + full_path + "\"");

  std::string header;
  pgm_file >> header;
  if (header != "P2")
    throw std::runtime_error("Unsupported PGM format in file: " + full_path);

  int width = 0, height = 0;
  T max_pixel_value;
  pgm_file >> width >> height >> max_pixel_value;

  debug::log(std::format(
    "PGM header read: dimensions={}x{}, max pixel value={}",
    width, height, max_pixel_value
  ));

  if (max_pixel_value != 255)
    throw std::runtime_error("Unsupported max value in \"" + filename + "\". Expected 255.");

  std::vector<T> pixel_data;
  T pixel;
  while (pgm_file >> pixel)
    pixel_data.push_back(pixel);

  if (static_cast<int>(pixel_data.size()) != width * height)
    throw std::runtime_error(std::format(
      "Mismatch in pixel count: expected {} but got {} in file \"{}\"",
      width * height, pixel_data.size(), filename
    ));

  return Image<T>(width, height, pixel_data);
}
