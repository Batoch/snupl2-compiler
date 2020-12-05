//
// test02
//
// scanner test: contains valid and invalid tokens
//
// note that the scanner will stop scanning at the first invalid character 
// encoding. This will lead to unexpected output for invalid character/string
// encodings, but is the desired behavior (our compiler will stop parsing at
// the first occurrence of an error).
//

// scannable input
<===>=#:=:::==>;;;if(then)else;a:=-1.1;a::==0x5;
1a2b
"hello world"

// invalid input
{
}
?
^
\
'\r'
'\12'
'hello world'
"Hello, \0world"
