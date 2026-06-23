# Zombie Arena Game

A fast-paced, 2D top-down shooter game developed using C++ and the Simple and Fast Multimedia Library (SFML). 

This project demonstrates a scalable game architecture built on advanced Object-Oriented Programming (OOP) principles, including inheritance, polymorphism, and abstraction. It features dynamic collision detection, layered views, and sprite sheet animations, all optimized for performance using C++ pointers, references, and the Standard Template Library (STL).

## 🛠️ Tech Stack
* **Language:** C++
* **Framework:** SFML (Simple and Fast Multimedia Library)
* **Concepts:** OOP, Memory Management, Game Loop Pattern, Collision Detection

## 📂 File Structure

The project is modularized into the following core files:

* **`ZombieArena.cpp`**
  The main entry point of the game. It contains the core game loop, handles real-time user input, manages the window views, orchestrates the interactions between the player and the zombies, and renders the graphics and UI to the screen.

* **`player.h`**
  Contains the `Player` class definition. Manages the player character's state, including position, health, movement speed, rotation (facing the mouse cursor), and screen boundaries. 

* **`zombie.h`**
  Contains the `Zombie` class definition. Handles the enemy AI, determining how the zombies spawn, track the player's position, manage their own health, and update their movement frames.

* **`bullet.h`**
  Contains the `Bullet` class definition. Manages the projectiles fired by the player, including their trajectory, speed, active state, and collision boundaries.

## ✨ Key Features
* **Dynamic Rendering:** Utilizes layered views, sprite sheets, and vertex arrays for efficient visual rendering.
* **Robust Mechanics:** Real-time input handling, dynamic collision detection, and custom sound effect integration.
* **Optimized Performance:** Efficient memory management and resource handling utilizing C++ pointers, references, and STL containers.

## 🚀 Getting Started

### Prerequisites
To compile and run this game, you need:
1. A C++ compiler (e.g., GCC, MSVC).
2. The [SFML Library](https://www.sfml-dev.org/download.php) installed and configured on your system.

### Compilation
If you are using a terminal with `g++`, you can compile the game using a command similar to this (ensure your SFML include and lib paths are set up correctly):

```bash
g++ -c ZombieArena.cpp
g++ ZombieArena.o -o ZombieArena -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
