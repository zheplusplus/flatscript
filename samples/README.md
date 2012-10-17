Stekin Features
===============

Spec
----

很抱歉, 语言规范还没有.

Samples
-------

samples 目录下有很多样例可以参考. 其中 samples/errors 目录下的样例包含一些典型的错误, 而 samples/warnings 目录下的样例包含典型的警告.

Features and examples
---------------------

### 缩进

Stekin 中, 缩进作为语法的一部分, 用来区分语句块

* Stekin 要求的缩进比 Python 更加强, 缩进必须是 4 空格的整数倍, 而不可以是制表符, 或者 1-3 个空格
* 缩进不可以参差不齐, 子句必须统一缩进
* 缩进还要紧邻, 如函数体的缩进只可以比函数头的缩进多一级

### 类型推导

* 变量定义无需指定类型 (Stekin 将从初值进行推导)

如

    x: 1
    y: x < 2

定义 `x`, 类型等同于初值 `1` 的类型 (整型), 和 `y`, 类型等同于初值为 `x < 2` 的类型 (布尔型).

* 函数定义无须指定参数与返回值类型 (Stekin 将从实参和 `return` 语句推导)

如

    func fib(x)
        if x < 1
            return 1
        return fib(x - 1) + fib(x - 2)

函数 `fib` 的返回值类型将从 `return 1` 中推导得出为整型.

### 不可修改

* 变量定义后不可以改变值或类型
* `=` 符号的作用是比较两侧操作数相等, 而不是赋值

### 嵌套函数定义

Stekin 允许函数嵌套定义, 并且内层定义的函数可以直接引用外层函数中定义的局部变量.

如

    func fib(x)
        func add()
            return fib(x - 1) + fib(x - 2)
        if x < 1
            return 1
        return add(x)

在函数 `add` 中, 可以直接引用外层函数 `fib` 的参数 `x`.

### `ifnot` 分支

对于条件成立时什么也不用做而条件不成立的时候需要忙活的分支, 如果不希望在条件前面打上碍眼的感叹号, 可以尝试这种分支语句.

如

    func fib(x)
        ifnot x < 1
            return fib(x - 1) + fib(x - 2)
        return 1

### 列表管道

Stekin 将常见的列表遍历行为集成到语言本身, 使用形如 `list | return 表达式` 进行 list mapping 操作. 该操作将会遍历列表, 将每个元素代入 ''表达式'' 中得到结果, 而结果列表则顺序包含这些结果表达式的值. 而使用形如 `list | if 表达式` 进行 list filter 操作. 该操作将会遍历列表, 将每个元素代入 ''表达式'' 中得到一个布尔值, 当且仅当该值为真, 结果列表包含该元素. 在管道中可以使用 `$element` 引用元素, 而使用 `$index` 引用元素的索引 (从 0 开始).

如

    list: [0, 1, 2, 3, 4]
    sqr_list: list | return $element * $element

此时 `sqr_list` 将是 `[0, 1, 4, 9, 16]`, 即将 `list` 中元素依次平方组成的列表.

如

    list: [0, 1, 2, 3, 4]
    smallers: list | if $element < 2

得到的 `smallers` 为 `[0, 1]`, 即 `list` 中所有满足 `$element < 2` 的元素组成的列表.

在管道操作符 `|` 之后的关键字 `return` 或是 `if` 决定了该管道的作用. 如上面例子如果修改为

    list: [0, 1, 2, 3, 4]
    smallers: list | return $element < 2

那么得到的 `smaller` 将为 `[true, true, false, false, false]`.

管道可以任意拼接, 如

    list: [0, 1, 2, 3, 4]
    new_list: list | return $element * $element | if $index % 2 = 0

将会得到 `[0, 4, 16]`, 即先执行 `list | return $element * $element` 的到临时量 `[0, 1, 4, 9, 16]`, 再套接上 `| if $index % 2 = 0` (滤去奇数项), 得到结果.
