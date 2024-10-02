# Qt Application with Item Movement and Animation

This project is a simple Qt-based application that demonstrates the use of a `QGraphicsScene` and `QGraphicsView` to display a grid of interactive items. Each item can be moved with animations, and the movement is triggered by mouse events. The program supports random item placement, item swapping, and matching logic for game-like functionality.

## Features

- **Item Grid**: A grid of images is displayed using `QGraphicsScene` and `QGraphicsView`.
- **Item Interaction**: Items can be dragged and dropped to swap places with adjacent items.
- **Movement Animation**: Items move smoothly across the grid using `QGraphicsItemAnimation`.
- **Matching Logic**: The system checks for matching adjacent items and removes them from the grid when they match.
- **Automatic Grid Refresh**: Once items are removed, the grid automatically refreshes and fills in with new random items.

## Project Structure

- **main.cpp**: Sets up the `QGraphicsScene`, `QGraphicsView`, and initializes the `Board` where items are displayed.
- **Board.cpp**: Handles the core game logic, including adding and removing items, swapping items, and checking for matches.
- **Item.cpp**: Implements the individual item behavior, including movement, mouse interaction, and animation.
- **Consts.h**: Contains constants like board size, animation speed, and image paths.

## Dependencies

- **Qt Framework**: This project uses Qt for the graphical interface, including classes like `QGraphicsScene`, `QGraphicsView`, and `QGraphicsPixmapItem`.
  
  Ensure you have the following Qt modules installed:
  - `QtWidgets`

## Build Instructions

1. Clone the repository:
   ```bash

