
###
### Make to manipulate webpages.
### Note that code relies on MinifyAll extension to first minify the raw source

all:include/pages/CineMatrix.css.h \
	include/pages/update.html.h \
	include/pages/captivePortal.html.h \
	include/pages/networkSetup.html.h \
	include/pages/networkConfigPage.js.h \
	include/pages/configDone.html.h \
	include/pages/WebLogin.html.h 
	@exec true 



include/pages/WebLogin.html.h: WEBSources/WebLogin.html 
	@./ProduceDefine.py $< text/html serverscope > $@


include/pages/test.html.h: WEBSources/test.html 
	@./ProduceDefine.py $< text/html\;charset=UTF-8  > $@

include/pages/CineMatrix.css.h: WEBsources/CineMatrix-min.css
	@./ProduceDefine.py $< text/css  > $@

include/pages/update.html.h: WEBsources/update.html
	@./ProduceDefine.py $< text/html\;charset=UTF-8 > $@


include/pages/captivePortal.html.h: WEBSources/captivePortal.html
	@./ProduceDefine.py $< text/html\;charset=UTF-8 > $@

include/pages/networkSetup.html.h: WEBsources/networkSetup.html
	@./ProduceDefine.py $< text/html\;charset=UTF-8 > $@

include/pages/networkConfigPage.js.h: WEBSources/networkConfigPage-min.js
	@./ProduceDefine.py $< text/javascript  > $@


include/pages/configDone.html.h: WEBsources/configDone.html
	@./ProduceDefine.py $< text/html\;charset=UTF-8 > $@

include/pages/CineMatrix.css.h: WEBsources/CineMatrix-min.css
	@./ProduceDefine.py $< text/css serverscope  > $@


clean:
	rm include/pages/*

