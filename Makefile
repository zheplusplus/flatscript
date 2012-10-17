COMPILE_FLAGS=-e require:process -p flsc
TEST_FLAGS=-e require:describe:it
FLSCT=flsc $(TEST_FLAGS)

build = $(1) -i main.fls > $(2)

all:
	$(call build,flsc $(COMPILE_FLAGS),flsc.js)

recursive:all
	$(call build,node ./flsc.js $(COMPILE_FLAGS),flsc-recur.js)

recursivecheck:recursive
	$(call build,node ./flsc-recur.js $(COMPILE_FLAGS),flsc-recur-recur.js)
	sha1sum flsc-recur.js flsc-recur-recur.js

unittest = $(1) $(TEST_FLAGS) -i test.fls > test.js && mocha --bail test.js
sampletest = $(1) -e require -i test/sample-test.fls | node

runtest:all
	$(call unittest,flsc)
	$(call sampletest,flsc)

recursivetest:recursive
	$(call unittest,flsc)
	$(call unittest,node ./flsc.js)
	$(call unittest,node ./flsc-recur.js)
	$(call sampletest,flsc)
	$(call sampletest,node ./flsc.js)
	$(call sampletest,node ./flsc-recur.js)

sample-test:all
	$(call sampletest,flsc)
	$(call sampletest,node ./flsc.js)

npm:recursive
	node ./flsc-recur.js $(COMPILE_FLAGS) -i npm.fls -o npm.js
	echo "#!/usr/bin/env node" > flsc
	echo "require('./flsc-recur')" >> flsc
	node npm.js > package.json

clean:
	find -type f -name "*.js" -exec rm {} \;
	rm -f package.json
