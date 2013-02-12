Stekinscript
============

A Javascript generator that makes it easy to write asynchronous code, in a synchronous way.

[Preview Version]

Starring Features
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

### Write asynchronous calls in a synchronous way, thus getting rid of annoying nested callbacks

If a callback is an anonymous function with parameters like `(error, result)` it is called a **regular asynchronous callback** in Stekinscript. It is good to use token `%%` to replace the callback **as an argument** in a call, and the rest statements in the block will become a part of the body of that anonymous function.

Code sample I

    content: fs.read('some-file', %%).toString()
    console.log(content)

Output

    var $c_content;
    fs.read("some-file", (function ($cb_err, $ar_0) {
        if ($cb_err) throw $cb_err;
        $c_content = $ar_0.toString();
        console.log($c_content);
    }));

Code sample II

    content: fs.read('some-file-a', %%).toString() +
             fs.read('some-file-b', %%).toString()
    console.log(content)

Output

    var $c_content;
    fs.read("some-file-a", (function ($cb_err, $ar_0) {
        if ($cb_err) throw $cb_err;
        fs.read("some-file-b", (function ($cb_err, $ar_1) {
            if ($cb_err) throw $cb_err;
            $c_content = ($ar_0.toString() + $ar_1.toString());
            console.log($c_content);
        }));
    }));

### Change the way to handle errors in regular asynchronous callback

It is a bad idea to throw an exception in asynchronous environment. Instead the error should also be propagated to some callback. In Stekinscript, if a function is defined with one of whose parameters is `%%`, this parameter is considered as the callback that consumes errors along with result.

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

Stekinscript will always read source code from stdin, and output Javascript via stdout. The ordinary way to compile files is like

    stekin < source.stkn > output.js

Or pipe the program to node

    stekin < source.stkn | node

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
