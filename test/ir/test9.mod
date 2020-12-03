//
// test9
//
// IR generation
//

module test9;

const
    N: integer = 5 + 2*2 + 1;

var i: integer;
    b: boolean;
    A: integer[N];
    B: boolean[N];

begin
  b := true;
  b := false;
  b := B[0];
  b := A[0] > 1;

  if (b) then
    b := true
  else
    b := false
  end;

  if (B[0]) then
    b := true
  else
    b := false
  end;

  if (A[0] > 1) then
    b := true
  else
    b := false
  end;

  i := 0
end test9.
