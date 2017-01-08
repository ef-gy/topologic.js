goog.provide('topologic');
goog.provide('topologic.Topologic');

// This depends on how the library is compiled; 7 is the default, so if this
// were to change in the future, it'd also need to be changed here.
topologic.MAX_DEPTH = 7;

// This is technically currently a singleton, because of limitations in the
// JS-to-C++ interface. I'm sure that could be fixed, but not sure yet if it'd
// be worthwhile.
topologic.Topologic = function() {
  this.activeDimension = 3;
  this.flameColouring = false;
}

topologic.Topologic.prototype.setActiveDimension = Module.cwrap(
  'setActiveDimension', 'number', ['number']);
topologic.Topologic.prototype.setFlameColouring = Module.cwrap(
  'setFlameColouring', null, ['number']);
topologic.Topologic.prototype.setViewportSize = Module.cwrap(
  'setViewportSize', null, ['number','number']);
topologic.Topologic.prototype.resetColourMap = Module.cwrap(
  'resetColourMap', null, []);
topologic.Topologic.prototype.getJSON = Module.cwrap(
  'getJSON', 'string', []);
topologic.Topologic.prototype.getSVG = Module.cwrap(
  'getSVG', 'string', []);
topologic.Topologic.prototype.getArgs = Module.cwrap(
  'getArgs', 'string', []);
topologic.Topologic.prototype.parseJSON = Module.cwrap(
  'parseJSON', null, ['string']);
topologic.Topologic.prototype.parseArgs = Module.cwrap(
  'parseArgs', null, ['string']);
topologic.Topologic.prototype.interpretDrag = Module.cwrap(
  'interpretDrag', 'number', ['number','number','number']);
topologic.Topologic.prototype.getModels = Module.cwrap(
  'getModels', 'string', []);

// These two are GL-specific; it may make more sense to put them in a separate
// module.
topologic.Topologic.prototype.forceRedraw = Module.cwrap(
  'forceRedraw', null, []);
topologic.Topologic.prototype.initialiseGL = Module.cwrap(
  'initialiseGL', 'number', []);

topologic.Topologic.prototype.useSettings = function(settings) {
  this.parseJSON(JSON.stringify(settings));
};
