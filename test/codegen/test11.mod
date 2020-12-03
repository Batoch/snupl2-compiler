//
// test11
//
// variable alignment
//
// expected output: all variables aligned without superfluous
// padding, both on the stack and in the global data section.
//

module test11;

var A: integer;
    B: boolean;
    C: longint;
    D: char;
    E: integer;
    F: boolean;
    G: integer[10];
    H: boolean;
    I: char[7];
    J: longint;
    K: integer;
    L: longint;

procedure foo();
var a: integer;
    b: boolean;
    c: longint;
    d: char;
    e: integer;
    f: boolean;
    g: integer[10];
    h: boolean;
    i: char[7];
    j: longint;
    k: integer;
    l: longint;
begin
end foo;


begin
end test11.
