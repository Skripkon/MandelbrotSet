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
      : m_fractal(MandelbrotSet(width, height, maxIter, numThreads)),
        m_window(sf::VideoMode(width, height), "Mandelbrot Set",
                 sf::Style::Titlebar | sf::Style::Close) {
    // Initialize the SFML image and texture
    m_image.create(m_fractal.WIDTH, m_fractal.HEIGHT);
    drawFractal();
    m_texture.loadFromImage(m_image);
    m_sprite.setTexture(m_texture);

    // Configure the selection rectangle (to zoom in)
    m_selectionRect.setFillColor(sf::Color(0, 0, 0, 0));
    m_selectionRect.setOutlineColor(sf::Color::Red);
    m_selectionRect.setOutlineThickness(3);

    // Load font and create buttons
    if (!m_font.loadFromFile("TimesNewRoman.ttf")) {
      std::cout << "Please, provide a Font!\n";
    }
    createButton(width - 150, 5, m_changeColorButton, m_changeColorButtonText,
                 "Change color");
    createButton(width - 150, 50, m_resetZoomingButton,
                 m_resetZoomingButtonText, "Reset Zooming");
  }

  void run() {
    while (m_window.isOpen()) {
      sf::Event event;
      while (m_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
          m_window.close();
          return;
        }
        handleEvent(event);
      }
      // Draw elements on the window
      m_window.draw(m_sprite);
      if (m_dragging) {
        m_window.draw(m_selectionRect);
      }
      m_window.draw(m_changeColorButton);
      m_window.draw(m_changeColorButtonText);
      m_window.draw(m_resetZoomingButton);
      m_window.draw(m_resetZoomingButtonText);

      m_window.display();
    }
  }

 private:
  void drawFractal() {
    m_fractal.renderFractal();
    for (int y = 0; y < m_fractal.HEIGHT; ++y) {
      for (int x = 0; x < m_fractal.WIDTH; ++x) {
        int iter = m_fractal.image[y * m_fractal.WIDTH + x];
        sf::Color color = getColor(iter);
        m_image.setPixel(x, y, color);
      }
    }
  }

  sf::Color getColor(int iter) {
    if (iter < m_fractal.MAX_ITER) {
      switch (m_COLOR) {
        case 1:
          return sf::Color(0, 255 * iter / m_fractal.MAX_ITER,
                           255 * iter / m_fractal.MAX_ITER);
        case 2:
          return sf::Color(255 * iter / m_fractal.MAX_ITER, 0,
                           255 * iter / m_fractal.MAX_ITER);
        case 3:
          return sf::Color(255 * iter / m_fractal.MAX_ITER,
                             255 * iter / m_fractal.MAX_ITER, 0);
      }
    }
    return sf::Color::Black;
  }

  void handleEvent(sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
      if (m_changeColorButton.getGlobalBounds().contains(event.mouseButton.x,
                                                         event.mouseButton.y)) {
        changeColorButtonHandler();

      } else if (m_resetZoomingButton.getGlobalBounds().contains(
                     event.mouseButton.x, event.mouseButton.y)) {
        resetZoomingButtonHandler();

      } else {
        // Start rectangle selection
        m_dragging = true;
        m_start = sf::Mouse::getPosition(m_window);
      }
      return;
    }

    // Mouse left-click released after selecting a rectangle (zoom in is needed)
    if (event.type == sf::Event::MouseButtonReleased &&
        event.mouseButton.button == sf::Mouse::Left && m_dragging) {
      handleZoomingIn();
      return;
    }

    if (m_dragging) {
      handleDragging();
      return;
    }

    if (event.type == sf::Event::MouseWheelScrolled &&
        event.mouseWheelScroll.delta != 0) {
      ZoomByScrolling(event.mouseWheelScroll.delta, event.mouseWheelScroll.x,
                      event.mouseWheelScroll.y);
    }
  }

  void changeColorButtonHandler() {
    m_COLOR = (m_COLOR % 3) + 1;
    drawFractal();
    m_texture.loadFromImage(m_image);
    m_sprite.setTexture(m_texture);
  }

  void resetZoomingButtonHandler() {
    m_fractal.setDefaultRegion();
    drawFractal();
    m_texture.loadFromImage(m_image);
    m_sprite.setTexture(m_texture);
  }

  void handleZoomingIn() {
    m_dragging = false;
    m_end = sf::Mouse::getPosition(m_window);
    if (std::abs(m_start.x - m_end.x) < EPS &&
        std::abs(m_start.y - m_end.y) < EPS) {
      return;
    }
    // Ensure aspect ratio is preserved (4:3)
    double widthRatio = 4.0 / 3.0;
    double newWidth = m_end.x - m_start.x;
    double newHeight = newWidth / widthRatio;

    if (newHeight < 0)
      newHeight = -newHeight;

    // Adjust the end point to maintain aspect ratio
    if (m_end.y < m_start.y) {
      m_end.y = m_start.y - newHeight;
    } else {
      m_end.y = m_start.y + newHeight;
    }

    // Specify a new area of the complex plane
    double realMin =
        m_fractal.REAL_MIN + (m_fractal.REAL_MAX - m_fractal.REAL_MIN) *
                                 std::min(m_start.x, m_end.x) / m_fractal.WIDTH;
    double realMax =
        m_fractal.REAL_MIN + (m_fractal.REAL_MAX - m_fractal.REAL_MIN) *
                                 std::max(m_start.x, m_end.x) / m_fractal.WIDTH;
    double imagMin = m_fractal.IMAG_MIN +
                     (m_fractal.IMAG_MAX - m_fractal.IMAG_MIN) *
                         std::min(m_start.y, m_end.y) / m_fractal.HEIGHT;
    double imagMax = m_fractal.IMAG_MIN +
                     (m_fractal.IMAG_MAX - m_fractal.IMAG_MIN) *
                         std::max(m_start.y, m_end.y) / m_fractal.HEIGHT;
    // Clear red rectangle
    m_selectionRect.setSize(sf::Vector2f(0, 0));

    m_fractal.updateRegion(realMin, realMax, imagMin, imagMax);
    drawFractal();
    m_texture.loadFromImage(m_image);
    m_sprite.setTexture(m_texture);
  }

  void handleDragging() {
    // Update selection rectangle during dragging
    m_end = sf::Mouse::getPosition(m_window);

    double newWidth = std::abs(m_end.x - m_start.x);
    double newHeight =
        newWidth * 3.0 / 4.0;  // Adjust the aspect ratio (must be 4:3)

    if (m_end.x > m_start.x && m_end.y < m_start.y) {  // up-right
      m_end.y = m_start.y - newHeight;
    } else if (m_end.x < m_start.x && m_end.y < m_start.y) {  // up-left
      m_end.y = m_start.y - newHeight;
      m_end.x = m_start.x - newWidth;
    } else if (m_end.x < m_start.x && m_end.y > m_start.y) {  // down-left
      m_end.x = m_start.x - newWidth;
    } else {  // down-right
      m_end.y = m_start.y + newHeight;
    }

    m_selectionRect.setPosition(sf::Vector2f(std::min(m_start.x, m_end.x),
                                             std::min(m_start.y, m_end.y)));
    m_selectionRect.setSize(sf::Vector2f(newWidth, newHeight));
  }

  void ZoomByScrolling(float delta, int mouseX, int mouseY) {
    // Calculate zoom factor
    double zoomFactor = (delta > 0) ? 0.9 : 1.1;

    // Calculate the new complex plane coordinates based on mouse position
    double mouseRe =
        m_fractal.REAL_MIN +
        (m_fractal.REAL_MAX - m_fractal.REAL_MIN) * mouseX / m_fractal.WIDTH;
    double mouseIm =
        m_fractal.IMAG_MIN +
        (m_fractal.IMAG_MAX - m_fractal.IMAG_MIN) * mouseY / m_fractal.HEIGHT;

    // Zoom in or out by adjusting the region
    double newWidth = (m_fractal.REAL_MAX - m_fractal.REAL_MIN) * zoomFactor;
    double newHeight = (m_fractal.IMAG_MAX - m_fractal.IMAG_MIN) * zoomFactor;

    // Center the new region around the mouse position
    double newRealMin = mouseRe - (mouseRe - m_fractal.REAL_MIN) * zoomFactor;
    double newRealMax = newRealMin + newWidth;
    double newImagMin = mouseIm - (mouseIm - m_fractal.IMAG_MIN) * zoomFactor;
    double newImagMax = newImagMin + newHeight;

    m_fractal.updateRegion(newRealMin, newRealMax, newImagMin, newImagMax);
    drawFractal();
    m_texture.loadFromImage(m_image);
    m_sprite.setTexture(m_texture);
  }

  void createButton(int x, int y, sf::RectangleShape& button, sf::Text& text,
                    const std::string& title) {
    button = sf::RectangleShape(sf::Vector2f(125, 30));
    button.setFillColor(sf::Color::Black);
    button.setPosition(x, y);
    button.setOutlineThickness(2);
    button.setOutlineColor(sf::Color::White);

    text.setFont(m_font);
    text.setString(title);
    text.setCharacterSize(15);
    text.setFillColor(sf::Color::White);
    text.setPosition(button.getPosition().x + 10, button.getPosition().y + 5);
  }

  MandelbrotSet m_fractal;
  sf::RenderWindow m_window;

  sf::Image m_image;
  sf::Texture m_texture;
  sf::Sprite m_sprite;

  sf::Vector2i m_start;
  sf::Vector2i m_end;
  sf::RectangleShape m_selectionRect;

  sf::RectangleShape m_changeColorButton;
  sf::Text m_changeColorButtonText;
  sf::RectangleShape m_resetZoomingButton;
  sf::Text m_resetZoomingButtonText;
  sf::Font m_font;

  constexpr static const double EPS = 1e-10;
  int m_COLOR = 1;  // Color palette identifier (1, 2, or 3)
  bool m_dragging =
      false;  // Flag to indicate if the user is dragging for selection
};
