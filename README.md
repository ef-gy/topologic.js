# Topologic-10 ###############################################################

Topologic is a programme that visualises basic primitives and fractals in
arbitrary dimensional spaces. Primitives include simplices, cubes and spheres,
and fractals include basic affine IFSs as well as fractal flames.

Supported output methods include simple SVGs and OpenGL 3.2 and up. The actual
calculations as well as most of the render tasks are performed by libefgy, a
header-only C++ template library for maths.

## WEBGL CLIENT ##############################################################

Since compiling tends to be a pain in the neck, you might want to consider
using the WebGL client instead. A ready-to-run version that'll run straight
in your browser without any need to install anything whatsoever - that is, if
you happen to have a WebGL-enabled browser - is available at:

http://ef.gy/webgl/topologic.html

Just type that in your browser's address bar and enjoy :3.

## OBTAINING TOPOLOGIC #######################################################

The Free/Open Source Software version of this programme is available free of
charge via git at http://github.com/ef-gy/topologic.git - to check
out a local copy of that repository, use the git command line tool like this:

    $ git clone git://github.com/ef-gy/topologic.git

Additionally there's a paid version available on the Mac App Store at
http://itunes.apple.com/us/app/topologic/id558300160 - this version is
functionally more or less identical to the F/OSS version, but it provides a
proper Cocoa GUI instead of the CLI/GLUT frontends. This App is rather cheap,
however, so if you happen to have a Mac and you do like the programme then I'd
really appreciate it if you got a copy ;).

Oh, and if you're not on a Mac but you happen to have an iPod/iPhone/iPad,
there's an app for those, too:
https://itunes.apple.com/us/app/topologic/id737310913 - I've really tried to
make good use of the touch controls on those devices, promise :).

Since version 9, the sources for these two frontends are also available in the
repository if you're fine with compiling things yourself on your Mac. See
below for instructions on how to do that.

Topologic is designed so that almost all of the code is template-y C++ in
header files, much like libefgy. The API documentation for this is available
at: http://ef.gy/documentation/topologic - the documentation may not always
be quite up to date, so, if push comes to shove just read the sources.

## OBTAINING LIBEFGY #########################################################

This library is distributed free of charge under a very permissive licence.
The public GIT repository is at http://github.com/ef-gy/libefgy.git -
see the documentation at http://ef.gy/documentation/libefgy for more details.

## INSTALLATION ##############################################################

You'll need to have libxml2 with development files and a recent version of
clang++ available on your system.

Furthermore, you'll need a recent version of libefgy - a header-only C++
template library for maths. If you don't want to install the library
system-wide, you could instead just create a symlink at include/ef.gy that
would point to the directory of the same name in libefgy's sources.

To compile the binary, run:

    $ make

To install the programme, run:

    # make PREFIX=/usr install

Adjust PREFIX as appropriate.

### THE GLUT FRONTEND ########################################################

If all you wanted were pretty SVGs, then the regular CLI programme is all you
needed; but if - instead - you'd want to play with things a bit, then you'll
want to compile the GLUT version as well. To do so, you'll also need GLEW,
FreeGLUT, OpenGL headers and a graphics card that supports OpenGL 3.2. Once
you've verified that you meet all of these prerequisites, run:

    $ make gl

... to compile the GL binaries, then to install them:

    # make PREFIX=/usr install-gl

To install the 'topologic-glut' binary. Again, adjust PREFIX as appropriate
for your system.

### THE WEBGL FRONTEND #######################################################

If you'd like to compile the WebGL frontend yourself instead of using the
publicly available one at http://ef.gy/webgl/topologic.html you'll need to
install emscripten instead of GLUT and run the following on a command line:

    $ make topologic-sdl.js

As the name suggests, the WebGL client is based on a simple SDL client. Once
emscripten has finished compiling you can run the client by opening the file
topologic.html in your favourite WebGL-enabled browser.

### THE COCOA/OSX FRONTEND ###################################################

Since version 9, Topologic's formerly closed-source Cocoa frontend is now
included in the source bundle. I'm not entirely sure why I used to keep that
closed-source, but now that it's open you can just open up the
"Topologic.xcodeproj" project in Xcode on your Mac and compile the frontend
like any other Xcode project.

You should set up a symlink to ef.gy's include directory before trying to
compile the Xcode project. To do so, make sure you check out both Topologic
and libefgy side-by-side in your favourite projects directory, like so:

    $ git clone https://github.com/ef-gy/libefgy.git
    $ git clone https://github.com/ef-gy/topologic.git

... then create the symlink:

    $ cd topologic/include
    $ ln -s ../../libefgy/include/ef.gy

This isn't strictly necessary, but it helps since it avoids having to install
the libefgy headers globally or copying things around manually.

### THE COCOA TOUCH/iOS FRONTEND #############################################

The "Topologic.xcodeproj" also includes the Cocoa Touch client for iOS. To
compile this, use the same procedure as for the Cocoa frontend, but choose an
iOS target in Xcode.

On iOS, Topologic uses futuretap's InAppSettingsKit to provide a smooth
interface to choose model and rendering options. You will need to clone the
InAppSettingsKit repository at https://github.com/futuretap/InAppSettingsKit
and then copy or link the "InAppSettingsKit" directory from that repository
to Topologic's root directory. The process is analogous to setting up the
libefgy header symlink described in the Cocoa section.

## COMPILE-TIME OPTIONS ######################################################

The rather unusual design of this programme allows you to set an arbitrary
limit to the number of dimensions supported by the projection code. This is
set at compile time with the MAXDEPTH constant. The default value of this
constant - if not set otherwise - is 7, meaning that the default topologic
binary will be able to generate, process and render primitives in up to 7
spatial dimensions.

This limit applies to both the model depth as well as the render depth. I.e.
with MAXDEPTH set to 7, then the deepest cube you can render is a 7-cube, in
7-space. But at the same time you couldn't render a 7-sphere, because that
would have to be rendered in 8-space - but a 6-sphere in 7-space will work.

To set a compile-time constant, use the -D flag in your CFLAGS, like so:

    $ make "CFLAGS=-g -D MAXDEPTH=8"

To confirm the number of dimensions that a topologic binary supports, use the
--version flag:

    $ ./topologic --version
    Topologic CLI; Version 5
    Maximum render depth of this binary is 8 dimensions.

## LICENCE ###################################################################

The F/OSS part of Topologic is distributed under an MIT/X style licence. For
all practical intents and purposes that means that you can do pretty much
whatever you want with the code, except claim that you wrote it yourself, as
long as you point out somewhere in your documentation that you used Topologic.

See the file COPYING in the repository for the exact licence terms.
