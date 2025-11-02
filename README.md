# cMusix

A terminal-based music player built in C.  
Made for those who want to jam without the RAM slam.

---

## About

**cMusix** is a minimalistic command-line music player, ensuring that the system 
It's built specifically for Arch-based systems, but the source is portable enough to compile on other *nix systems too.

The idea is simple: you give it a music file, it plays it — no GUI, no menus, no distractions.  
Perfect for anyone who just wants music while staying fully in the terminal.

---

## Features

- Supports common audio formats: `.wav`, `.mp3`, `.ogg`
- Fast and light (Because its in C)
- Works from the terminal – great for tiling WM users
- Open for anyone to use or modify

---

## Requirements

You’ll need SDL2 and SDL2_mixer installed before compiling. The installation script will help you download that, if you dont have. If you are unsure, you may want to see it for yourself

### Installation:
```bash
git  clone https://github.com/pandgey/cMusix.git
cd cMusix/
make install
make
``` 
The program assume that you store your music in Music directory

If you are unsure where is your Music folder, i made an algorithm to check for it as well. Assuming you are in cMusix folder
```bash
cd folderLocator
gcc -o musicFinder musicFinder.c
./musicFinder
``` 

### Tested on
- Arch-Linux

## Contributions
Found anything broken or have a new ideas for this?
Feel free to shoot me a message or just open an issue

## License
~No license headache, just use it.
It’s me, my code, and the beat. You’re free to remix it however you want~ you know what? imma just put license just to protect myself, idk just protection :3
(ps. you dont have to credit me, but always appreciated!)
