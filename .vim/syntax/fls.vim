if version <= 508
  command -nargs=+ HiLink hi link <args>
else
  command -nargs=+ HiLink hi def link <args>
endif

HiLink FlsKeyword Statement
HiLink FlsConditional Conditional
HiLink FlsExcKeyword Tag
HiLink FlsClassKeyword StorageClass
HiLink FlsPipeItem StorageClass
HiLink FlsOp Tag
HiLink FlsInclude Include
HiLink FlsComment  Comment
HiLink FlsTodo Todo
HiLink FlsLiteral Number
HiLink FlsBuiltin Function
HiLink FlsSpaceError Error

delcommand HiLink

syn keyword FlsKeyword func return
syn keyword FlsKeyword for break continue
syn keyword FlsKeyword extern enum
syn keyword FlsKeyword delete
syn keyword FlsConditional else if ifnot
syn keyword FlsExcKeyword try catch throw
syn keyword FlsInclude include export
syn keyword FlsClassKeyword class this ctor super
syn match FlsClassKeyword ":"
syn match FlsPipeItem "$[a-z]*"
syn keyword FlsOp owns instanceof typeof
syn match FlsOp "[-+*/%<>=!\|\&]"
syn match FlsOp "|:"
syn match FlsOp "|?"

syn match FlsComment "#.*$" contains=FlsTodo
syn keyword FlsTodo FIXME TODO

syn keyword FlsLiteral true false null undefined NaN Infinity
syn match FlsLiteral "\<\%([1-9]\d*\|0\)\=\>"
syn region FlsLiteral start=+\z(['"]\)+ end="\z1" skip="\\\\\|\\\z1"
syn region FlsLiteral start=+\z('''\|"""\)+ end="\z1" keepend
syn region FlsLiteral start=+[rR]\z(['"]\)+ end="\z1" skip="\\\\\|\\\z1"
syn region FlsLiteral start=+[rR]\z('''\|"""\)+ end="\z1" keepend
syn region FlsLiteral start=+/[^/*]+me=e-1 skip=+\\\\\|\\/+ end=+/[gim]\{0,2\}\s*$+ end=+/[gim]\{0,2\}\s*[;.,)\]}]+me=e-1 contains=@htmlPreproc oneline

syn keyword FlsBuiltin console eval setTimeout setInterval clearTimeout
syn keyword FlsBuiltin Number Array Object Function JSON RegExp Math String Date
syn keyword FlsBuiltin parseInt parseFloat isFinite isNaN escape unescape
syn keyword FlsBuiltin encodeURI decodeURI encodeURIComponent decodeURIComponent
syn keyword FlsBuiltin Error EvalError RangeError ReferenceError URIError
syn keyword FlsBuiltin SyntaxError TypeError

syn match FlsSpaceError display excludenl "\s\+$"
syn match FlsSpaceError display " \+\t"
syn match FlsSpaceError display "\t\+ "

let b:current_syntax = "fls"

" vim:set sw=2 sts=2 ts=8 noet:
