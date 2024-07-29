#include <complex>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class MandelbrotSet {
 public:
  std::vector<int> image;  // Stores the number of iterations for each pixel
  int WIDTH;               // Width of the image
  int HEIGHT;              // Height of the image

  double REAL_MIN = -2;    // Minimum real value
  double REAL_MAX = 2;     // Maximum real value
  double IMAG_MIN = -1.5;  // Minimum imaginary value
  double IMAG_MAX = 1.5;   // Maximum imaginary value

  const int MAX_ITER;  // Maximum number of iterations

  MandelbrotSet(int width, int height, int maxIter, int numThreads)
      : WIDTH(width),
        HEIGHT(height),
        MAX_ITER(maxIter),
        NUM_THREADS(numThreads) {
    image.resize(WIDTH * HEIGHT);
  }

  // Renders the entire fractal using multithreading
  void renderFractal() {
    std::vector<std::thread> threads;
    int rowsPerThread = HEIGHT / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; ++i) {
      int startRow = i * rowsPerThread;
      int endRow = (i == NUM_THREADS - 1) ? HEIGHT : (i + 1) * rowsPerThread;
      threads.push_back(std::thread(&MandelbrotSet::renderFractalPart, this,
                                    startRow, endRow));
    }

    for (auto& thread : threads) {
      thread.join();
    }
  }

  // Updates the region of the complex plane to render
  void updateRegion(double realMin, double realMax, double imagMin,
                    double imagMax) {
    REAL_MIN = realMin;
    REAL_MAX = realMax;
    IMAG_MIN = imagMin;
    IMAG_MAX = imagMax;
  }

  // Resets the region to the default values
  void setDefaultRegion() {
    REAL_MIN = -2;
    REAL_MAX = 2;
    IMAG_MIN = -1.5;
    IMAG_MAX = 1.5;
  }

 private:
  int NUM_THREADS;  // Number of threads for rendering

  // Renders a portion of the fractal (from startRow to endRow)
  void renderFractalPart(int startRow, int endRow) {
    for (int y = startRow; y < endRow; ++y) {
      for (int x = 0; x < WIDTH; ++x) {
        double realPart = REAL_MIN + (REAL_MAX - REAL_MIN) * x / WIDTH;
        double imagPart = IMAG_MIN + (IMAG_MAX - IMAG_MIN) * y / HEIGHT;
        std::complex<double> c(realPart, imagPart);
        std::complex<double> z = 0;
        int iter = 0;

        // Iterate to determine the color of the pixel
        while (std::abs(z) <= 2 && iter < MAX_ITER) {
          z = z * z + c;
          iter++;
        }

        image[y * WIDTH + x] = iter;  // Store the iteration count
      }
    }
  }
};
