# 🔫 Beam of Madness

A fast-paced 3D aim trainer built with **C++ and OpenGL (FreeGLUT)**, featuring a Roblox-style character armed with an AK-47. Destroy all targets before the timer runs out!

---

## 🎮 Gameplay

- Shoot targets to score points
- Two levels with different difficulty
- 30 seconds per level — be fast!
- Secret cheat code for MADNESS MODE 🤫

---

## 🕹️ Controls

| Key | Action |
|-----|--------|
| `W` `A` `S` `D` | Move & Turn |
| `Space` | Jump |
| `Left Click` | Shoot beam |
| `F` | First person view |
| `G` | Third person view |
| `ESC` | Pause / Back to menu |
| `boom` (type it) | 🤯 Madness Mode |

---

## 📋 Levels

| Level | Target Size | Description |
|-------|------------|-------------|
| Level 1 | Large | Easy — good for warming up |
| Level 2 | Small | Hard — targets are tiny and fast |

---

## 🛠️ Built With

- **C++**
- **OpenGL / FreeGLUT**
- **stb_image** — for texture loading
- **Windows MCI** — for music playback
- **PlaySound** — for sound effects

---

## 📁 Project Structure

```
├── main.cpp          # Entry point, input handling
├── GameManager.cpp   # Core game logic, rendering
├── GameManager.h
├── Player.cpp        # Roblox-style character + AK-47
├── Player.h
├── Target.cpp        # Target spawning and drawing
├── Target.h
├── stb_image.h       # Texture loader
├── grassfield.jpg    # Ground texture
├── brickwall.jpg     # Sky/wall texture
├── boxing_bell.wav   # Shoot sound
└── ayay.wav          # Madness mode music
```

---

## 🚀 How to Build

### Requirements
- Windows OS
- Code::Blocks with MinGW
- FreeGLUT installed

### Steps
1. Clone the repo
   ```bash
   git clone https://github.com/Sorrow01001000/AimLabGameCodeBlocks.git
   ```
2. Open the `.cbp` project file in **Code::Blocks**
3. Make sure FreeGLUT is linked in your compiler settings
4. Hit **Build → Rebuild** then **Run**

> ⚠️ Update the texture file paths in `GameManager.cpp` `init()` to match your local machine!

---

## ✨ Features

- 🧱 Roblox-style blocky character with walk animation
- 🔫 Detailed AK-47 model made from cubes
- 🌿 Grass ground + brick sky textures
- 🌲 Decorated world with trees
- 🎯 Moving targets that slide and bob
- 🕺 Jump mechanic with gravity
- 📷 Switchable first/third person camera
- 🌈 Madness Mode with chaos and music

---

## 👾 Screenshots

> *(Add your screenshots here!)*

---

## 📜 License

This project was made for a graphics course. Feel free to use or modify it!
