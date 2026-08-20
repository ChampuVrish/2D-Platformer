# Custom 2D Engine
(Can't Even come up with a good name........yet)

*A handcrafted 2D action-platformer engine built with ****SDL3**** and ****C++****, inspired by gothic horror games and designed from scratch as a learning project that grew into a reusable game engine.*

---

## Overview

This Custom 2D Engine is a custom-built 2D platformer engine written in modern C++ using SDL3. It started as a simple platformer tutorial but has evolved into an engine capable of handling animated characters, enemies, projectiles, parallax backgrounds, collision detection, camera movement, and future boss fights.

The goal is not just to make one game—it is to build a reusable 2D engine for creating gothic action platformers.

---

## Features

### Player System

* 8-frame idle breathing animation.
* Running animation.
* Sliding animation.
* Jump and fall states.
* Shooting while idle, running, jumping, and sliding.
* Sprite flipping based on movement direction.

### Combat

* Projectile shooting system.
* Bullet collision detection.
* Enemy animation support.
* Independent player and enemy animation systems.

### Engine Systems

* Custom `GameObject` architecture.
* Animation manager (`Animation.h`).
* Texture management through a resource system.
* Tile-based collision detection.
* Camera with viewport tracking.
* Debug collision rendering.
* VSync support.

### World Rendering

* 16:9 logical rendering (`640 × 360`).
* High-resolution assets rendered at game scale.
* Multi-layer parallax background system.
* Gothic environment assets.

---

## Tech Stack

* **Language:** C++20
* **Graphics:** SDL3
* **Image Loading:** SDL3_image
* **Build System:** CMake
* **Compiler:** MinGW / Visual Studio

---

## Project Structure

```text
2D-Platformer/
├── Assets/
│   ├── Player/
│   ├── Enemies/
│   ├── Backgrounds/
│   ├── Bullets/
│   └── Tiles/
│
├── Animation.h
├── GameObject.h
├── Resources.h
├── 2D.cpp
├── CMakeLists.txt
└── README.md
```

---

## Current Gameplay

* Smooth player movement.
* Platform collision.
* Camera follows the player.
* Enemy idle animation.
* Projectile firing.
* Sliding mechanics.
* Debug collision visualization.

---

## Engine Architecture

Every object in the game is represented by a `GameObject`.

```cpp
GameObject
├── Texture
├── Animation
├── Position
├── Velocity
├── Collider
├── Object Type
└── Player / Enemy Data
```

The renderer works independently of gameplay logic, allowing different sprite-sheet sizes to render at fixed in-game sizes.

Examples:

| Sprite Sheet           | Rendered Size |
| ---------------------- | ------------- |
| Player (147×145/frame) |     32×32     |
| Enemy (512×624/frame)  |     50×50     |
| Bullet (32×32/frame)   |     2×2       |

---

## Roadmap

### Environment

* Parallax castle layers.
* Animated fog layer.
* Dynamic rain.
* Lightning effects.
* Moonlit gothic sky.

### Gameplay

* Health bar.
* Enemy AI.
* Melee and ranged enemies.
* Boss fights.
* Damage system.
* Pickups and collectibles.

### Engine Improvements

* Render size stored per object.
* Animation state machine.
* Particle system.
* Sound manager.
* Scene management.
* UI/HUD framework.

---

## Future Vision

This engine is being developed as the foundation for a **Dark Horror-inspired gothic 2D action platformer** featuring:

* Dark Victorian environments.
* Fast-paced combat.
* Unique enemy encounters.
* Multi-stage boss battles.
* Atmospheric weather and lighting.
* Rich parallax world design.

---

## Screenshots

> Coming soon: Gameplay, parallax backgrounds, enemy encounters, and boss arena previews.

---
## Controls

| Key                 | Action                                     |
| ------------------- | ------------------------------------------ |
| **W**               | Move Up *(future interactions / ladders)*  |
| **A**               | Move Left                                  |
| **S**               | Move Down *(future interactions / crouch)* |
| **D**               | Move Right                                 |
| **K**               | Jump                                       |
| **J**               | Shoot                                      |

### Debug Mode

Press **Backslash (`\`)** at any time to enable or disable the engine's debug overlay.

When enabled, the engine displays:

* 🟥 Collision bounding boxes.
* 🟩 Ground and object colliders.
* 🔍 Debug visualization for gameplay physics.


## Author

**AyushMsrv**

Building a custom 2D game engine in C++ and SDL3 while documenting the journey from tutorial code to a fully featured gothic action-platformer engine.
