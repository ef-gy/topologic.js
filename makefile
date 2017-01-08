-include ef.gy/base.mk include/ef.gy/base.mk

NAME:=topologic.js
VERSION:=11

JQUERY:=https://code.jquery.com/jquery-2.1.1.js
JQUERYMOBILE:=https://code.jquery.com/mobile/1.4.4/jquery.mobile-1.4.4.js
JQUERYMOBILECSS:=https://code.jquery.com/mobile/1.4.4/jquery.mobile-1.4.4.min.css

CXXFLAGS:=$(CFLAGS) -fno-exceptions
EMCFLAGS:=--pre-js src/web/setup.js --post-js src/web/glue.js -O3 --llvm-lto 3 -fno-exceptions --memory-init-file 0 -s NO_FILESYSTEM=1 -s ELIMINATE_DUPLICATE_FUNCTIONS=1 -s OUTLINING_LIMIT=20480 -DNOVAO -DWEBGL -s ASSERTIONS=0 -s NO_EXIT_RUNTIME=1 -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=1 -s PRECISE_I64_MATH=0 -s USE_CLOSURE_COMPILER=1
EMXXFLAGS:=$(EMCFLAGS)

JSFUNCTIONS:=['_main','_interpretDrag','_setActiveDimension','_forceRedraw','_setFlameColouring','_resetColourMap','_setViewportSize','_getJSON','_getSVG','_getArgs','_parseJSON','_parseArgs','_getModels','_initialiseGL']

%.html: src/%.cpp include/*/*.h
	$(EMXX) -std=c++0x -Iinclude/ -D NOLIBRARIES $(EMXXFLAGS) $< $(LDFLAGS) -o $@

$(DOWNLOADS)/jquery.js: $(DOWNLOADS)/.volatile
	$(CURL) '$(JQUERY)' -o $@

$(DOWNLOADS)/jquery.mobile.js: $(DOWNLOADS)/.volatile
	$(CURL) '$(JQUERYMOBILE)' -o $@

$(DOWNLOADS)/jquery.mobile.css: $(DOWNLOADS)/.volatile
	$(CURL) '$(JQUERYMOBILECSS)' -o $@

topologic-web.css: $(DOWNLOADS)/jquery.mobile.css src/web/topologic.css
	cat $^ | $(CSSMIN) > $@

topologic-web.html: src/web/topologic.xhtml xslt/web-prepare.xslt topologic-web.js topologic-web.css
	$(XSLTPROC) -o "$@" xslt/web-prepare.xslt $<

topologic.js: topologic-web.js src/topologic.js
	closure-compiler --js topologic-web.js --js src/topologic.js >$@

src/chrome/topologic.js: topologic-web.js $(DOWNLOADS)/jquery.js $(DOWNLOADS)/jquery.mobile.js
	closure-compiler --js $(DOWNLOADS)/jquery.js --js $(DOWNLOADS)/jquery.mobile.js --js topologic-web.js >$@

src/chrome/topologic.html: src/web/topologic.xhtml xslt/chrome-prepare.xslt src/chrome/topologic.js src/chrome/topologic-web.css
	$(XSLTPROC) -o "$@" xslt/chrome-prepare.xslt $<

src/chrome/topologic-web.css: topologic-web.css
	cp $^ $@

src/chrome/topologic-128.png: $(ICON)
	rsvg -w 128 -a $^ $@ || true

src/chrome/topologic-16.png: $(ICON)
	rsvg -w 16 -a $^ $@ || true

chrome-app.zip: src/chrome/manifest.json src/chrome/topologic.html src/chrome/topologic.js src/chrome/topologic-web.css src/chrome/background.js src/chrome/topologic-128.png src/chrome/topologic-16.png
	zip -9jo $@ $^

