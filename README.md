# Visualization of the Mandelbrot Set using C++ and SFML

My videos about fractals and this code: [Part 1](https://youtu.be/KPTZsSQ8DcE?si=kp0P_gmDtlRbwkn7) & [Part 2](https://youtu.be/aSPB0Ftv-2A?si=iduV0tESrp9AiqxF)

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

# Demo (quality lost)

| Width | Height | Maximum Number of Iterations | Speed |
|-------|--------|------------------------------|-------|
| 600   | 450    | 100                          | 1x    |

![demo2](https://github.com/user-attachments/assets/327ca5b5-983d-4006-a843-85a0497e2ef3)

![demo1](https://github.com/user-attachments/assets/d767de3e-61c8-47f3-9b75-574e2868d5f7)


