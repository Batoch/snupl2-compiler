//
// test13
//
// simple computations with longints
//
// expected output: 11242-5 (no newline)
//

module test13;

var a : longint;
    b : longint;
    c : longint;

begin
  a := 3L;
  b := 8L;

  c := a+b;
  WriteLong(c);

  c := a*b;
  WriteLong(c);

  c := b / a;
  WriteLong(c);

  c := -b+a;
  WriteLong(c)
end test13.
