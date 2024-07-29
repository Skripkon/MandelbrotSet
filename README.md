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

| Width | Height | Maximum Number of Iterations |
|-------|--------|------------------------------|
| 800   | 600    | 100                          |

https://github.com/user-attachments/assets/ea86bc21-220a-402c-9a40-6546fe1b79cd

https://github.com/user-attachments/assets/a3779584-85c6-4143-bc1e-248ed817cbcd
