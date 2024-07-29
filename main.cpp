#include <complex>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include <SFML/Graphics.hpp>
#include "Application.cpp"  // Include the Application class

int main() {
  // The ratio (width : height) must be 4:3
  int width = 600;
  int height = 450;
  int maxIter = 100;
  int numThreads = std::thread::
      hardware_concurrency();  // Number of threads for multithreading

  // Create an instance of the Application class
  Application application(width, height, maxIter, numThreads);

  // Run the application
  application.run();

  return 0;
}
