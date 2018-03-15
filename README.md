
## TTC Cog Pedro Spot Simulator

This program takes the state of Mario and the lower Pedro spot cog as well as a sequence of RNG result values, and computes
the maximum speed that can be achieved in the spot.

It also includes a visualization tool in case you want to see what's going on.


### Running the program

If you're on Windows, you can just download the .exe from builds/. It's portable, so you can run it from anywhere without
installation. If you're not on Windows, you can modify one of the build scripts to fit your system.

You should run the program from the command line:

```<executable location>/cogsim inputfile.txt```

`inputfile.txt` is a file specifying the initial state, described below.

There are two additional optional arguments:

```-o output.csv``` generates a spreadsheet showing what happens on each frame.

```-v``` runs the program in visual mode (see below).


### Input format

Here is an example input:

```
setting = 2

mario = {
  x = 1420
  z = -1139.4
  yaw = -0x40C
  hSpeed = 33
}

cog = {
  yaw = 0
  speed = 0
  speedTarget = 0
}

rng = {
  1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 2 -1 1 -2 -1 1 1 -1 1 -1 1 -1 1 -1 1 -1 1 -1
  1 -1 1 -1 1 -1
}
```

`setting` is the internal TTC speed setting:

```
0 = slow (3)
1 = fast (9)
2 = random (6)
3 = still (12)
```

The rest of the variables should be self-explanatory.

The input format is pretty lenient. White space doesn't matter (including new lines), and you can separate the values
with commas if you want.

Integer variables (yaw) can be specified in decimal or hexadecimal.

You can specify a floating point number either in decimal or by its IEEE-754 representation in hexadecimal. This
is the best way to guarantee perfect accuracy with the game.


### Visual mode

Running the program with the `-v` argument opens a window to let you watch the cog and Mario.

While in visual mode:

- Pressing the up and down keys zooms in and out, respectively.

- Pressing the U key toggles the rendering mode. The default is wireframe, which outlines the cog floor triangles.
Pressing U switches to unit square mode, which instead colors all unit squares white if they are considered to be
located above the floor. This can slow down the rendering a bit if you're zoomed out.
