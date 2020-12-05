//
// test06
//
// computations with boolean arrays
//
// expected output: 0001111111 (no newline)
//

module test06;

const
    N : integer = 10;

var a : boolean[N];

procedure test(a: boolean[]);
var i: integer;
begin
  i := 0;
  while (i < N) do
    a[i] := i > 2;
    i := i+1
  end;

  i := 0;
  while (i < N) do
    if (a[i]) then WriteInt(1)
    else WriteInt(0)
    end;
   i := i+1
  end
end test;

begin
  test(a)
end test06.
