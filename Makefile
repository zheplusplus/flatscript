COMPILE_FLAGS=-e require:process -p flsc
TEST_FLAGS=-e require:process:describe:it

_build = $(1) -i main.fls -o $(2)

all:
	$(call _build,flsc $(COMPILE_FLAGS),flsc.js)

recursive:all
	$(call _build,node ./flsc.js $(COMPILE_FLAGS),flsc-recur.js)

recursivecheck:recursive
	$(call _build,node ./flsc-recur.js $(COMPILE_FLAGS),flsc-recur-recur.js)
	sha1sum flsc-recur.js flsc-recur-recur.js

_unittest = $(1) $(TEST_FLAGS) -i test.fls -o test.js && mocha --bail test.js
_sampletest = $(1) -e require -i test/sample-test.fls | node

runtest:all
	$(call _unittest,flsc)
	$(call _sampletest,flsc)

recursivetest:recursive
	$(call _unittest,flsc)
	$(call _unittest,node ./flsc.js)
	$(call _unittest,node ./flsc-recur.js)
	$(call _sampletest,flsc)
	$(call _sampletest,node ./flsc.js)
	$(call _sampletest,node ./flsc-recur.js)

unittest:all
	$(call _unittest,flsc)

sample-test:all
	$(call _sampletest,flsc)
	$(call _sampletest,node ./flsc.js)

npm:recursive
	node ./flsc-recur.js $(COMPILE_FLAGS) -i npm.fls -o npm.js
	echo "#!/usr/bin/env node" > flsc
	echo "require('./flsc-recur')" >> flsc
	node npm.js > package.json

clean:
	find -type f -name "*.js" -exec rm {} \;
	rm -f package.json
	rm -f flsc
