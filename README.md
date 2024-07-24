This is a sandpit area to see how making simple games just for fun
steps to setup

# insatll dependancies libraries
inspired and initially all taken from from https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux

The original code is in C, again just for fun moved to a C++ style to experiment

to install the dependacies
```bash
sudo apt install libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev libwayland-dev libxkbcommon-dev
```

to build and run

cd into game directory (say asteroids)

cmake -B build
cmake --build build
cd build
./asteroids
