# 🎶 cMusix

A no-frills, terminal-based music player built in C using SDL2 and SDL2_mixer.  
Made for those who want to jam without the RAM slam.

---

## 🧠 About

**cMusix** is a minimalistic command-line music player designed with performance in mind.  
It's built specifically for Arch-based systems, but the source is portable enough to compile on other *nix systems too.

The idea is simple: you give it a music file, it plays it — no GUI, no menus, no distractions.  
Perfect for anyone who just wants music while staying fully in the terminal.

---

## ✨ Features

- Supports common audio formats: `.wav`, `.mp3`, `.ogg`
- SDL2-powered audio playback
- Fast and light – built entirely in C
- Works from the terminal – great for tiling WM users or minimalists
- Open for anyone to use or modify

---

## ⚙️ Requirements

You’ll need SDL2 and SDL2_mixer installed before compiling.

### 🧪 Arch-based systems:
```bash
sudo pacman -S sdl2 sdl2_mixer
