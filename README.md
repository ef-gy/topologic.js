# Topologic-11 ###############################################################

Topologic is a programme that visualises basic primitives and fractals in
arbitrary dimensional spaces. Primitives include simplices, cubes and spheres,
and fractals include basic affine IFSs as well as fractal flames.

Supported output methods include simple SVGs and OpenGL 3.2 and up. The actual
calculations as well as most of the render tasks are performed by libefgy, a
header-only C++ template library for maths.

## PRECOMPILED WEBGL CLIENT ##################################################

Since compiling tends to be a pain in the neck, you might want to consider
using the precompiled WebGL client instead. A ready-to-run version that'll run
straight in your browser without any need to install anything whatsoever - that
is, if you happen to have a WebGL-enabled browser - is available at:

https://dee.pe/r

Just type that in your browser's address bar and enjoy :3.

## OBTAINING TOPOLOGIC #######################################################

The Free/Open Source Software version of this programme is available free of
charge via git at https://github.com/ef-gy/topologic.git - to check
out a local copy of that repository, use the git command line tool like this:

    $ git clone --recursive https://github.com/ef-gy/topologic.js.git

### THE WEBGL FRONTEND #######################################################

If you'd like to compile the WebGL frontend yourself instead of using the
publicly available one at https://dee.pe/r you'll need to install emscripten
and run the following on a command line:

    $ make topologic-web.html

The WebGL client is based on a simple SDL client. Once emscripten has finished
compiling you can run the client by opening the file topologic-web.html in your
favourite WebGL-enabled browser.

## LICENCE ###################################################################

Topologic is distributed under an MIT/X style licence. For all practical intents
and purposes that means that you can do pretty much whatever you want with the
code, except claim that you wrote it yourself, as long as you point out
somewhere in your documentation that you used Topologic.

See the file COPYING in the repository for the exact licence terms.
