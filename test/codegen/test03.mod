//
// test03
//
// simple computations with booleans
// alignment of global variables
//
// expected output: 010 (no newline)
//

module test03;

var a : boolean;
    b : boolean;
    i : integer;
    c : boolean;

begin
  a := true;
  b := false;

  c := a && b;
  if (c) then WriteInt(1)
  else WriteInt(0)
  end;

  c := a || b;
  if (c) then WriteInt(1)
  else WriteInt(0)
  end;

  c := !(a || b);
  if (c) then WriteInt(1)
  else WriteInt(0)
  end
end test03.
