#include <SFML/Graphics.hpp>
#include <complex>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include "MandelbrotSet.cpp"

class Application {
 public:
  Application(int width, int height, int maxIter, int numThreads)
      : fractal(MandelbrotSet(width, height, maxIter, numThreads)),
        window(sf::VideoMode(width, height), "Mandelbrot Set",
               sf::Style::Titlebar | sf::Style::Close) {
    // Initialize the SFML image and texture
    image.create(fractal.WIDTH, fractal.HEIGHT);
    drawFractal();
    texture.loadFromImage(image);
    sprite.setTexture(texture);

    // Configure the selection rectangle (to zoom in)
    selectionRect.setFillColor(sf::Color(0, 0, 0, 0));
    selectionRect.setOutlineColor(sf::Color::Red);
    selectionRect.setOutlineThickness(3);

    // Load font and create buttons
    if (!font.loadFromFile("TimesNewRoman.ttf")) {
      std::cout << "Please, provide a Font!\n";
    }
    createButton(width - 150, 5, changeColorButton, changeColorButtonText,
                 "Change color");
    createButton(width - 150, 50, resetZoomingButton, resetZoomingButtonText,
                 "Reset Zooming");
  }

  void run() {
    while (window.isOpen()) {
      sf::Event event;
      while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
          window.close();
          return;
        }
        handleEvent(event);
      }
      // Draw elements on the window
      window.draw(sprite);
      if (dragging) {
        window.draw(selectionRect);
      }
      window.draw(changeColorButton);
      window.draw(changeColorButtonText);
      window.draw(resetZoomingButton);
      window.draw(resetZoomingButtonText);

      window.display();
    }
  }

 private:
  void handleEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
      if (changeColorButton.getGlobalBounds().contains(event.mouseButton.x,
                                                       event.mouseButton.y)) {
        // Change color button clicked
        COLOR = (COLOR % 3) + 1;
        drawFractal();
        texture.loadFromImage(image);
        sprite.setTexture(texture);
      } else if (resetZoomingButton.getGlobalBounds().contains(
                     event.mouseButton.x, event.mouseButton.y)) {
        // Reset zooming button clicked
        fractal.setDefaultRegion();
        drawFractal();
        texture.loadFromImage(image);
        sprite.setTexture(texture);
      } else {
        // Start rectangle selection
        dragging = true;
        start = sf::Mouse::getPosition(window);
      }
    }

    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left) {
      // Mouse left-click released
      if (dragging) {
        dragging = false;
        end = sf::Mouse::getPosition(window);
        if (std::abs(start.x - end.x) < EPS &&
            std::abs(start.y - end.y) < EPS) {
          return;
        }
        // Ensure aspect ratio is preserved (4:3)
        double widthRatio = 4.0 / 3.0;
        double newWidth = end.x - start.x;
        double newHeight = newWidth / widthRatio;

        if (newHeight < 0)
          newHeight = -newHeight;

        // Adjust the end point to maintain aspect ratio
        if (end.y < start.y) {
          end.y = start.y - newHeight;
        } else {
          end.y = start.y + newHeight;
        }

        // Specify a new area of the complex plane
        double realMin =
            fractal.REAL_MIN + (fractal.REAL_MAX - fractal.REAL_MIN) *
                                   std::min(start.x, end.x) / fractal.WIDTH;
        double realMax =
            fractal.REAL_MIN + (fractal.REAL_MAX - fractal.REAL_MIN) *
                                   std::max(start.x, end.x) / fractal.WIDTH;
        double imagMin =
            fractal.IMAG_MIN + (fractal.IMAG_MAX - fractal.IMAG_MIN) *
                                   std::min(start.y, end.y) / fractal.HEIGHT;
        double imagMax =
            fractal.IMAG_MIN + (fractal.IMAG_MAX - fractal.IMAG_MIN) *
                                   std::max(start.y, end.y) / fractal.HEIGHT;

        fractal.updateRegion(realMin, realMax, imagMin, imagMax);
        drawFractal();
        texture.loadFromImage(image);
        sprite.setTexture(texture);
      }
    }

    if (dragging) {
      // Update selection rectangle during dragging
      end = sf::Mouse::getPosition(window);

      double newWidth = std::abs(end.x - start.x);
      double newHeight =
          newWidth * 3.0 / 4.0;  // Adjust the aspect ratio (must be 4:3)

      if (end.x > start.x && end.y < start.y) {  // up-right
        end.y = start.y - newHeight;
      } else if (end.x < start.x && end.y < start.y) {  // up-left
        end.y = start.y - newHeight;
        end.x = start.x - newWidth;
      } else if (end.x < start.x && end.y > start.y) {  // down-left
        end.x = start.x - newWidth;
      } else {  // down-right
        end.y = start.y + newHeight;
      }

      selectionRect.setPosition(
          sf::Vector2f(std::min(start.x, end.x), std::min(start.y, end.y)));
      selectionRect.setSize(sf::Vector2f(newWidth, newHeight));
    }

    if (event.type == sf::Event::MouseWheelScrolled) {
      if (event.mouseWheelScroll.delta != 0) {
        handleZoom(event.mouseWheelScroll.delta, event.mouseWheelScroll.x,
                   event.mouseWheelScroll.y);
      }
    }
  }

  void handleZoom(float delta, int mouseX, int mouseY) {
    // Calculate zoom factor
    double zoomFactor = (delta > 0) ? 0.9 : 1.1;

    // Calculate the new complex plane coordinates based on mouse position
    double mouseRe = fractal.REAL_MIN + (fractal.REAL_MAX - fractal.REAL_MIN) *
                                            mouseX / fractal.WIDTH;
    double mouseIm = fractal.IMAG_MIN + (fractal.IMAG_MAX - fractal.IMAG_MIN) *
                                            mouseY / fractal.HEIGHT;

    // Zoom in or out by adjusting the region
    double newWidth = (fractal.REAL_MAX - fractal.REAL_MIN) * zoomFactor;
    double newHeight = (fractal.IMAG_MAX - fractal.IMAG_MIN) * zoomFactor;

    // Center the new region around the mouse position
    double newRealMin = mouseRe - (mouseRe - fractal.REAL_MIN) * zoomFactor;
    double newRealMax = newRealMin + newWidth;
    double newImagMin = mouseIm - (mouseIm - fractal.IMAG_MIN) * zoomFactor;
    double newImagMax = newImagMin + newHeight;

    fractal.updateRegion(newRealMin, newRealMax, newImagMin, newImagMax);
    drawFractal();
    texture.loadFromImage(image);
    sprite.setTexture(texture);
  }

  void drawFractal() {
    fractal.renderFractal();
    for (int y = 0; y < fractal.HEIGHT; ++y) {
      for (int x = 0; x < fractal.WIDTH; ++x) {
        int iter = fractal.image[y * fractal.WIDTH + x];
        sf::Color color = sf::Color::Black;
        if (iter < fractal.MAX_ITER) {
          switch (COLOR) {
            case 1:
              color = sf::Color(0, 255 * iter / fractal.MAX_ITER,
                                255 * iter / fractal.MAX_ITER);
              break;
            case 2:
              color = sf::Color(255 * iter / fractal.MAX_ITER, 0,
                                255 * iter / fractal.MAX_ITER);
              break;
            case 3:
              color = sf::Color(255 * iter / fractal.MAX_ITER,
                                255 * iter / fractal.MAX_ITER, 0);
              break;
          }
        }
        image.setPixel(x, y, color);
      }
    }
  }

  void createButton(int x, int y, sf::RectangleShape& button, sf::Text& text,
                    const std::string& title) {
    button = sf::RectangleShape(sf::Vector2f(125, 30));
    button.setFillColor(sf::Color::Black);
    button.setPosition(x, y);
    button.setOutlineThickness(2);
    button.setOutlineColor(sf::Color::White);

    text.setFont(font);
    text.setString(title);
    text.setCharacterSize(15);
    text.setFillColor(sf::Color::White);
    text.setPosition(button.getPosition().x + 10, button.getPosition().y + 5);
  }

  // Members
  MandelbrotSet fractal;
  sf::Image image;
  sf::RenderWindow window;
  sf::Texture texture;
  sf::Sprite sprite;
  sf::Vector2i start, end;
  sf::RectangleShape selectionRect;
  sf::RectangleShape changeColorButton;
  sf::Text changeColorButtonText;
  sf::RectangleShape resetZoomingButton;
  sf::Text resetZoomingButtonText;
  sf::Font font;

  constexpr static const double EPS = 1e-10;
  int COLOR = 1;  // Color palette identifier (1, 2, or 3)
  bool dragging =
      false;  // Flag to indicate if the user is dragging for selection
};
