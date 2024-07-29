# Setup (Linux)

## Install SFML (lib for graphics)

```bash
sudo apt-get install libsfml-dev
```

## Compile

```bash
g++ main.cpp \
    Application.cpp \
    MandelbrotSet.cpp \
    -o mandelbrot \
    -lsfml-graphics \
    -lsfml-window \
    -lsfml-system \
    -pthread
```

## Run

```bash
./mandelbrot
```

# Demo

*to be added*