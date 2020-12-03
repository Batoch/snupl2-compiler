//
// test08
//
// local integer arrays
//
// expected output: 1987654321 (no newline)
//

module test08;

const
  N: integer = 10;

procedure test();
var a: integer[N];
    i: integer;
begin
  a[0] := 1;

  i := 1;
  while (i < N) do
    a[i] := N-i;
    i := i+1
  end;

  i := 0;
  while (i < N) do
    WriteInt(a[i]);
    i := i+1
  end
end test;

begin
  test()
end test08.
