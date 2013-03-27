Stekinscript
============

A Javascript generator that makes it easy to write asynchronous code in a synchronous way. [Alpha Version]

A Simple Example of Starring Features
-----------------

Steknscript code:

    fs: require('fs')
    try
        console.log(fs.readFile('a.txt', %%) + fs.readFile('b.txt', %%))
    catch
        console.error($e)
    console.log('end')

Program would first read "a.txt", then read "b.txt", concatenate their content successively, and output to console, or report to stderr if any error occurs. And a message "end" would get printed in the end.

Though in this piece of Stekinscript code there isn't any *asynchronous* part like callbacks, but actually Stekinscript compile it into Javascript like (formatted by other tools)

    (function() {
        var $c_fs;
        function $anf_0xa753c0($exception) {
            console.error($exception);
            $anf_0xa75480();
        }
        function $anf_0xa75480() {
            console.log("end");
        }

        $c_fs = require("fs");
        try {
            $c_fs.readFile("a.txt", (function($cb_err, $ar_0xa74740) {
                if ($cb_err) return $anf_0xa753c0($cb_err);
                try {
                    $c_fs.readFile("b.txt", (function($cb_err, $ar_0xa75bc0) {
                        if ($cb_err) return $anf_0xa753c0($cb_err);
                        try {
                            console.log(($ar_0xa74740 + $ar_0xa75bc0));
                            $anf_0xa75480();
                        } catch ($exception) {
                            $anf_0xa753c0($exception);
                        }
                    }));
                } catch ($exception) {
                    $anf_0xa753c0($exception);
                }
            }));
        } catch ($exception) {
            $anf_0xa753c0($exception);
        }
    })();

Let me explain the generated code. Function `$anf_0xa753c0` is actually from the `catch` block in the original code, the name of which is randomly generated. But as we know in asynchronous context a try-catch wouldn't help if the exception is generated from a callback, so in Stekinscript the `catch` blockes would be translated into a function, and if an exception is thrown in a callback, the function would be called. The name `$anf_0xa753c0` would be used several times, as in each actual `catch` block it would be called in case any exception raised outside a callback. And it is also used when a callback to each `fs.readFile` is called with error.

The happy path is remarkable, too. When write in Stekinscript two `fs.readFile` calls are side by side, just like function calls whose results are get added together and passed to `console.log`. But in target JS code it becomes somehow sophisticated that the are replaced by two corresponding callback parameters.

Another function `$anf_0xa75480` I shall introduce is what contains codes that should be executed after the try-catch. The calls to this function is properly inserted to ensure `console.log('end')` runs at the last.

Other Features
-----------------

### Indentation-indicated syntax

Code samples

    # define a function that calculates fibonacci number
    func fib(n)
        if n <= 1
            return 1
        return fib(n - 1) + fib(n - 2)

Ouput

    function $c_fib($c_n) {
        if (($c_n <= 1)) {
            return 1;
        }
        return ($c_fib(($c_n - 1)) + $c_fib(($c_n - 2)));
    }

Stekinscript will also do some name mangling.

It is easy to break a long line into shorter ones, by hitting return after proper tokens. Code samples

    ['this', 'is', 'a',
        'long', 'list']

    callFunction('with', 'several'
            , 'arguments')

    x: a +
        b

### Anonymous function in an easy way

Anonymous functions are written in this way without any keywords

    (parameters):
        function-body

Code samples

    fs.read('some-file', (error, content):
            console.log(content.toString())
        )

Output

    fs.read("some-file", (function ($c_error, $c_content) {
        console.log($c_content.toString());
    }));

### Change the way to handle errors in regular asynchronous callback

Along with the try-catch mode at the first, if a function is defined with one of whose parameters is `%%`, this parameter is considered as the callback that consumes errors along with result.

Code sample

    func read(fileA, fileB, %%)
        return fs.read(fileA, %%) + fs.read(fileB, %%)

Output

    function $c_read($c_fileA, $c_fileB, $c_$_racb) {
        fs.read($c_fileA, (function ($cb_err, $ar_0) {
            if ($cb_err) return $c_$_racb($cb_err);
            fs.read($c_fileB, (function ($cb_err, $ar_1) {
                if ($cb_err) return $c_$_racb($cb_err);
                return $c_$_racb(null, ($ar_0 + $ar_1));
            }));
        }));
    }

### Pipe a list into a result

It uses a pipeline to iterate over a list. Pipeline operators are `|:` and `|?`. The former represents a mapping operation while the latter represents a filtering. Within a pipeline, use `$` to reference the element, and `$i` for the index.

Code sample

    x: [1, 1, 2, 3, 5, 8, 13]
    console.log(x |: $ * $)
    console.log(x |? $ % 3 = 1)
    console.log(x |: $i % 2 = 0)
    console.log(x |? $i % 2 = 0)

Results

    [1, 1, 4, 9, 25, 64, 169]
    [1, 1, 13]
    [true, false, true, false, true, false, true]
    [1, 2, 5, 13]

Pipeline could be used along with regular asynchronous calls.

Code sample

    func readFiles(fileList, %%)
        fileContent: fileList |: fs.read($, %%)
        return fileContent.join('')

Output

    function $c_readFiles($c_fileList, $c_$_racb) {
        var $c_fileContent;
        var $ar_0 = (function ($list) {
            if (!($list) || $list.length === undefined)
                return $c_$_racb('not iterable');
            function $next($index, $result) {
                var $key = null;
                if ($index === $list.length) {
                    $c_fileContent = $result;
                    return $c_$_racb(null, $c_fileContent.join(""));
                } else {
                    var $element = $list[$index];
                    fs.read($element, (function ($cb_err, $ar_1) {
                        if ($cb_err) return $c_$_racb($cb_err);
                        $result.push($ar_1);
                        return $next($index + 1, $result);
                    }));
                }
            }
            $next(0, []);
        })($c_fileList);
    }

Use the Compiler
----------------

### Build

* C++ compiler with C++11 features with lambda, move semantic, `nullptr`, decltype support. Suggested: g++ 4.6 or later clang++ 3.2 or later
* flex (the lexical parser) 2.5+
* bison 2.4+
* GMP lib 5+
* Python 2.7.x (to generate some code)
* make

Just run

    make

in the source directory and executable `stekin` would be generated. Clang is used by default. To specify another compiler, try

    make COMPILER=g++

In cygwin g++ is preferred.

### Run

Stekinscript will always read source code from stdin, and output Javascript via stdout. The ordinary way to compile files is like

    stekin < source.stkn > output.js

Or pipe the program to node

    stekin < source.stkn | node

FAQ
---

### Why the compiler complains name 'require'/'exports'/'document'/'window' not defined?

Stekinscript checks name definition at compile time, and it is not possible to use any name that is not defined or imported.

You could pre-import names via `-i` option, like

    stekin -i document -i window < client/source.stkn > client/output.js
    stekin -i require -i exports < server/source.stkn > server/output.js

### How might I use jQuery in Stekinscript?

Use `jQuery` the identifier instead of `$` because `$` represents list elements in pipeline context, like

    buttons: jQuery('.btn')

For More Information
--------------------

Please check the [wiki pages](https://github.com/neuront/stekinscript/wiki/_pages). (Chinese version only)
