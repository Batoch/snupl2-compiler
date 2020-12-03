//
// array declaration and definitions
//
// require string support not required
//

module array01;

var A : integer[5][5];

procedure foo(a: integer);
var B: integer[5];
begin
  B[7] := a;
end foo;

begin
  A[0][1] := A[2][3]
end array01.
