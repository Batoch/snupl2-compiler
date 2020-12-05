//
// test04
//
// computations with boolean arrays
//
// expected output: 0001111111 (no newline)
//
// if you get an endless loop, make sure you store
// booleans as 1-byte values.
//

module test04;

const
    N : integer = 2*2*2 + 2;

var a : boolean[N];
    i : integer;

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
end test04.
