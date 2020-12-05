//
// test03
//
// scanner test: character and strings constants
//


// these should all pass

'7'
'\0'
'\x00'
'\x77'
'ä'
'\xe4'


"hello, world!"

"hello,\tworld\n"

"she said, \"hello, world!\""

"escaped single-quotes are not required but accepted: \'this is nice,\', he said"
"escaped single-quotes are not required but accepted: 'this is nice,', he said"

"a backslash is a '\\' character"

"Extended Latin1 characters work, but are escaped internally: ö, ä, ü."

"Hex encoded characters in strings: \x80."

""

// these are illegal

'\r'
'\'
'\12'
''

// no NULL character can appear in a string
"null-terminated string: \0"

// string not terminated (i.e., illegal character (newline) in string constant)
"hello, world

// non-printable ascii character
"the following is a backspace character: "

// invalid escape sequence
"hello\, world!"

// end of file
"hello
