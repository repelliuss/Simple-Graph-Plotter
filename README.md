<p align="center">
  &nbsp;<img width="200" height="200" src="https://b.allthepics.net/rpsLogo.png">
</p>

# Simple Graph Plotter

<p align="center">
  <img width="470" height="380" src="https://b.allthepics.net/55e6da8e31fda4f34.png">
</p>

## Table of contents
* [General Info](#general-info)
* [Screenshots](#screenshots)
* [Code Example](#code-example)
* [Features](#features)
* [Setup](#setup)
* [Demo](#demo)

## General Info
&nbsp;&nbsp;&nbsp;&nbsp;A library for plotting 2D graphs of mathematical equations and some curved geometric shapes using only vectors. This is not an advanced library but it has some cool [features](#features) and easy-to-use if your need is simple.

&nbsp;&nbsp;&nbsp;&nbsp;This was a university project in 2018-2019 spring semester. Challenge of the project was using only vectors so that's why this library implemented using just vectors.

## Screenshots
<p align="center"><img src="https://b.allthepics.net/1b7594a4f68dafe65.png" width="302" height="255" />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="https://b.allthepics.net/3eeaec92b0aa5f155.png" width="302" height="255" /></p>
<p align="center"><img src="https://b.allthepics.net/2fa22a417b1e0f585.png" width="302" height="255" />&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<img src="https://b.allthepics.net/4c7b8d534b034ea58.png" width="302" height="255" /></p>

## Code Example
```c
Figure *fig;
Color c = {.red = 80, .green = 20, .blue = 0};

fig = start_figure(10, 10);
set_thickness_resolution(fig, 0.05, 1000);
set_color(fig, c);

draw_fx(fig, your_function, -10, 10);
export_eps(fig, "filename.eps");
```

## Features
* Capable of drawing equations with one input
* Draws ellipse, circle, polyline
* Resizable paper size
* Resolution, color and thickness settings
* Takes domain of a function
* Scaling
* Resizing
* Appending
* Exporting to SVG and EPS files
* Importing from SVG files

## Setup
1. Clone this repository to your project.
2. Give `-Iinclude` option to your compiler.
3. `#include <RPS/libvector.h>` in your file.
4. Compile `src/libvector.c` and give object file to your linker.

## Demo
&nbsp;&nbsp;&nbsp;&nbsp;You can follow comments in file `test/driver.c` for demo instructions.
* To run demo, type `make` in the main folder where you cloned this repository.
* A folder will be created named `demo` where you can see exported graphs.
* Type `make clean` if you want to remove generated demo files.
