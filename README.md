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

Original speed (no editing)

| Width | Height | Maximum Number of Iterations |
|-------|--------|------------------------------|
| 800   | 600    | 100                          |

![demo2](https://github.com/user-attachments/assets/327ca5b5-983d-4006-a843-85a0497e2ef3)

https://github.com/user-attachments/assets/ea86bc21-220a-402c-9a40-6546fe1b79cd
