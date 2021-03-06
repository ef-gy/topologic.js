/**\ingroup topologic-frontend
 * \defgroup frontend-sdl SDL frontend
 * \brief Topologic frontend using SDL (for emscripten/JS)
 *
 * The main purpose of this frontend is to provide access to Topologic to
 * JavaScript code by means of the emscripten LLVM-to-JS compiler. The code
 * might even work as a primitive SDL frontend, but this is not tested.
 *
 * \{
 */

/**\file
 * \brief Topologic/SDL frontend
 *
 * This is a basic SDL+OpenGL frontend for Topologic, designed for use with
 * emscripten targeted at WebGL.
 *
 * \copyright
 * This file is part of the Topologic project, which is released as open source
 * under the terms of an MIT/X11-style licence, described in the COPYING file.
 *
 * \see Project Documentation: http://ef.gy/documentation/topologic.js
 * \see Project Source Code: https://github.com/ef-gy/topologic.js
 * \see Licence Terms: https://github.com/ef-gy/topologic.js/blob/master/COPYING
 */

/**\defgroup frontend-webgl WebGL frontend
 * \brief WebGL frontend for web browsers
 *
 * The primary purpose of the SDL frontend is to provide the code needed to get
 * the WebGL frontend to work. This is done through the magic of emscripten.
 *
 * \htmlonly
 * <iframe width="100%" height="800" src="https://dee.pe/r" frameborder="0"/>
 * \endhtmlonly
 *
 * \see https://dee.pe/r - current, pre-compiled version of the
 *      WebGL frontend.
 */

/**\defgroup topologic-javascript-exports emscripten/JS Exported Functions
 * \brief Functions which emscripten will export when compiling to JavaScript
 *
 * Some functions are exported by emscripten so that they can be used by other
 * JavaScript code in the same JS context. These functions have an 'export "C"'
 * tag so they can be looked up without any name mangling issues and they're
 * grouped together in the documentation for convenience.
 */

#include <emscripten.h>

#if !defined(NO_OPENGL)
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_opengl.h"
#else
using GLfloat = double;
#endif

#include <iostream>
#include <topologic/parse.h>
#include <topologic/arguments.h>

#if !defined(MAXDEPTH)
/**\brief Maximum render depth
 *
 * This macro is used by some of the frontends to determine the maximum render
 * depth supported by a frontend. The default value is '7', which is plenty for
 * most applications - increasing this value will increase the size of the
 * generated code, so it may be desirable to decrease this value in
 * environments with tighter constraints.
 */
#define MAXDEPTH 7
#endif

/**\brief Global state object
 *
 * Topologic programme state instance for this programme. Has to be global,
 * because we need to provide C-like accessors for emscripten/JavaScript
 * callers.
 */
static topologic::state<GLfloat, MAXDEPTH> topologicState;

// these functions are defined as 'extern "C"' to disable output name mangling;
// when compiling with emscripten these functions are exported so that they can
// be used in foreign JS code.
extern "C" {
#if !defined(NO_OPENGL)
int initialiseGL(void);
void process(void);
void forceRedraw(void);
void setViewportSize(int, int);
#endif
int interpretDrag(double, double, double);
int setActiveDimension(int);
void setFlameColouring(bool);
void resetColourMap(void);
const char *getJSON(void);
const char *getSVG(void);
const char *getArgs(void);
void parseJSON(const char *);
void parseArgs(const char *);
const char *getModels();
}

#if !defined(NO_OPENGL)
/**\brief Is a mouse button currently being pressed?
 *
 * Set to 'true' whenever a mouse button has been pressed; reset to 'false'
 * when a button is no longer being pressed. Used to implement mouse 'drag'
 * input, which rotates the model in the currently active dimension.
 */
static bool buttonDown = false;

/**\brief Should the scene be rendered?
 *
 * Set to 'true' with the forceRedraw() function. The main loop will only draw
 * a new frame if this variable is true, so whenever you change any output
 * parameters you should call the forceRedraw() function to set this flag.
 *
 * The next time the main loop has completed drawing the scene it will reset
 * this flag to 'false'.
 */
static bool doRender = true;

/**\brief Has the scene's dimensions been forced to the current values?
 *
 * Set to 'true' with setViewportSize() so that the main loop will not try to
 * use the SDL information to specify the output dimensions.
 *
 * This is needed with version of emscripten that do not support querying the
 * canvas size.
 */
static bool forcedSize = false;

/**\brief Has SDL been initialised?
 *
 * Set after successfully running initialiseGL(); set to true when a context is
 * available and initialised. Only SVG/JSON exports will work until this is
 * true.
 */
static bool SDLinitialised = false;

/**\ingroup topologic-javascript-exports
 * \brief Render the scene
 *
 * Sets the 'doRender' variable to 'true' to make the main loop draw the
 * current scene.
 */
void forceRedraw(void) { doRender = true; }

/**\ingroup topologic-javascript-exports
 * \brief Resize viewport
 *
 * Call this function after resizing the viewport in the frontend; this is most
 * useful for the WebGL client, where SDL is not exactly in control of the
 * canvas size and the user might resize the browser window at any time.
 *
 * \param[in] width  The viewport's new width
 * \param[in] height The viewport's new height
 *
 * \note Setting the viewport to (0,0) will reset the viewport to be queried via
 *       SDL on the next frame render cycle.
 */
void setViewportSize(int width, int height) {
  if ((width == 0) && (height == 0)) {
    forcedSize = false;
    topologicState.width = 1280;
    topologicState.height = 720;
  } else {
    forcedSize = true;
    topologicState.width = width;
    topologicState.height = height;
  }
  doRender = true;
}

/**\ingroup topologic-javascript-exports
 * \brief Main loop iteration
 *
 * Will try to process any pending SDL events in a loop and then return control
 * to the caller. This function is designed to be called directly in the
 * programme's main loop - either in main() itself, or, in an emscripten/JS
 * envionment, in whatever place emscripten_set_main_loop() saw fit to have it
 * run in.
 */
void process(void) {
  if (!SDLinitialised) {
    return;
  }

  const SDL_VideoInfo *info = SDL_GetVideoInfo();

  if (info && !forcedSize) {
    topologicState.width = info->current_w;
    topologicState.height = info->current_h;
  }

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_MOUSEBUTTONDOWN:
      switch (event.button.button) {
      case 4:
        topologicState.interpretDrag(0, 0, 30);
        doRender = true;
        break;
      case 5:
        topologicState.interpretDrag(0, 0, -30);
        doRender = true;
        break;
      default:
        buttonDown = true;
      }
      break;
    case SDL_MOUSEBUTTONUP:
      switch (event.button.button) {
      case 4:
      case 5:
        break;
      default:
        buttonDown = false;
      }
      break;
    case SDL_MOUSEMOTION:
      if (buttonDown) {
        topologicState.interpretDrag(event.motion.xrel, event.motion.yrel, 0);
        doRender = true;
      }
      break;
    }
  }

  if (doRender) {
    doRender = false;

    if (topologicState.model) {
      topologicState.model->opengl(true);
    }

    SDL_GL_SwapBuffers();
  }
}

/**\ingroup topologic-javascript-exports
 * \brief Initialise SDL and the GL context
 *
 * Call this function to set up the OpenGL context using standard SDL calls.
 * Until you do so, no rendering can take place - except for SVGs, which do not
 * rely on OpenGL.
 *
 * \returns 0 on success, nonzero otherwise.
 */
int initialiseGL(void) {
  SDL_Surface *screen;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Unable to initialize SDL: " << SDL_GetError() << "\n";
    return 1;
  }

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // *new*

  screen = SDL_SetVideoMode(1280, 720, 16, SDL_OPENGL | SDL_RESIZABLE);
  if (!screen) {
    std::cerr << "Unable to set video mode: " << SDL_GetError() << "\n";
    return 2;
  }

  glClearDepth(1.0f);

  glEnable(GL_BLEND);

  glDisable(GL_CULL_FACE);

  SDLinitialised = true;

  return 0;
}
#endif

/**\ingroup topologic-javascript-exports
 * \brief Genere new colour map
 *
 * Creates a new colour map for the fractal flame rendering algorithm. The
 * colours in that map are chosen at random.
 */
void resetColourMap(void) {
#if !defined(NO_OPENGL)
  topologicState.opengl.setColourMap();
  doRender = true;
#else
#warning "resetColourMap() needs an implementation."
#endif
}

/**\ingroup topologic-javascript-exports
 * \brief Topologic/SDL main function
 *
 * Initialise the global state to a basic 4-cube, then have emscripten take over
 * by setting a main function and returning.
 *
 * The command line arguments are unused.
 *
 * \returns 0 on success, nonzero otherwise.
 */
int main(int, char **) {
  efgy::geometry::with<GLfloat, topologic::updateModel, MAXDEPTH>(
      topologicState, "cartesian", "cube", 4, 4);

#if !defined(NO_OPENGL)
  emscripten_set_main_loop(process, 30, 0);
#endif

  return 0;
}

/**\ingroup topologic-javascript-exports
 * \brief Interpret trackball/mouse drag events
 *
 * Call this functin after detecting mouse drag or scroll events. This is a
 * simple wrapper for topologic::state::interpretDrag(), so see that function's
 * documentation for further details.
 *
 * \returns '0' if things went smoothly, nonzero otherwise. This method doesn't
 *          do anything that could fail, however, so it'll always return '0'.
 */
int interpretDrag(double x, double y, double z) {
  topologicState.interpretDrag(GLfloat(x), GLfloat(y), GLfloat(z));
  return 0;
}

/**\ingroup topologic-javascript-exports
 * \brief Set active dimension
 *
 * Sets the currently 'active' dimension. Each dimension of the global state
 * object has an 'active' flag, which determins whether certain operations,
 * such as interpreting a mouse drag event, apply to them; this function sets
 * that flag.
 *
 * \returns '0' if things went smoothly, nonzero otherwise. This method doesn't
 *          do anything that could fail, however, so it'll always return '0'.
 */
int setActiveDimension(int dim) {
  topologicState.setActive(dim);
  return 0;
}

/**\ingroup topologic-javascript-exports
 * \brief Enable or disable fractal flame colouring
 *
 * Set the 'fractal flame colouring algorithm' flag of the global topologic
 * state object.
 *
 * \param[in] flameColouring Whether to enable the fractal flame colouring
 *                           algorithm.
 */
void setFlameColouring(bool flameColouring) {
  topologicState.fractalFlameColouring = flameColouring;
}

/**\ingroup topologic-javascript-exports
 * \brief Obtain JSON metadata
 *
 * Creates JSON metadata, stores it locally and returns a pointer to it so it
 * may be reused later.
 *
 * \returns A pointer to a C-style string containing JSON model metadata.
 */
const char *getJSON(void) {
  std::ostringstream os("");
  static std::string str;

  os << efgy::json::tag() << topologicState;

  str = os.str();

  return str.c_str();
}

/**\ingroup topologic-javascript-exports
 * \brief Obtain SVG render
 *
 * Creates an SVG render, stores it locally and returns a pointer to a C-style
 * string that contains this SVG data.
 *
 * \returns A pointer to a C-style string containing an SVG render of the
 *          currently active model.
 */
const char *getSVG(void) {
  std::ostringstream os("");
  static std::string str;

  os << efgy::svg::tag() << topologicState;

  str = os.str();

  return str.c_str();
}

/**\ingroup topologic-javascript-exports
 * \brief Obtain argument string
 *
 * Creates a semicolon-separated list of command line arguments, which could be
 * used to reconstruct the current state object.
 *
 * \returns A pointer to a C-style string containing an arguments of currently
 *          active model.
 */
const char *getArgs(void) {
  std::ostringstream os("");
  static std::string str;
  bool first = true;
  std::vector<std::string> v;

  for (const auto &arg : topologicState.args(v)) {
    if (!first) {
      os << ";";
    } else {
      first = false;
    }
    os << arg;
  }

  str = os.str();

  return str.c_str();
}

/**\ingroup topologic-javascript-exports
 * \brief Parse JSON export
 *
 * Takes a JSON string, parses it and applies it to the current global state
 * context.
 *
 * \param[in] json The JSON string to parse.
 */
void parseJSON(const char *json) {
  std::string s(json);
  efgy::json::value<> v;
  s >> v;
  topologic::parse(topologicState, v);
  topologic::parseModel<GLfloat, MAXDEPTH, topologic::updateModel>(
      topologicState, v);
}

/**\ingroup topologic-javascript-exports
 * \brief Parse CLI arguments
 *
 * Takes a semicolon-separated list of CLI arguments and applies them to the
 * current global state context.
 *
 * \param[in] pArgs The argument string to parse.
 */
void parseArgs(const char *pArgs) {
  std::istringstream s(pArgs);
  std::string arg;
  std::vector<std::string> args;

  while (std::getline(s, arg, ';')) {
    args.push_back(arg);
  }

  topologic::parse(topologicState, args, false);
}

/**\ingroup topologic-javascript-exports
 * \brief Get JSON list of models and formats
 *
 * Similar to the --version flag of the proper binary; this returns a JSON
 * string with all the available models and formats.
 *
 * \returns JSON string of all the available models and formats.
 */
const char *getModels() {
  std::ostringstream os("");
  static std::string str;

  efgy::json::value<> modelSet;
  modelSet.toArray();
  efgy::json::value<> formatSet;
  formatSet.toArray();

  std::set<const char *> models;
  for (const char *m :
       efgy::geometry::with<GLfloat, efgy::geometry::functor::models, MAXDEPTH>(
           models, "*", 0, 0)) {
    modelSet.push(m);
  }
  std::set<const char *> formats;
  for (
      const char *f :
      efgy::geometry::with<GLfloat, efgy::geometry::functor::formats, MAXDEPTH>(
          formats, "*", "*", 0, 0)) {
    formatSet.push(f);
  }

  efgy::json::value<> modelData;
  modelData.toObject();

  modelData("models") = modelSet;
  modelData("formats") = formatSet;

  os << efgy::json::tag() << modelData;

  str = os.str();

  return str.c_str();
}

/** \} */
