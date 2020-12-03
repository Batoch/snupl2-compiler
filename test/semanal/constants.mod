//
// constants.mod
//
// expression evaluation of constants
//

module constants;

const
  M : integer = 5;
  N : integer = 5 + 5 * 3;
  O : integer = M + N + 1;

  B : boolean = M > O;
  T : boolean = true;

  C : char = 'x';

  // If the array bounds are specified, the size of the string including
  // the terminating \0 character must match the size of the character array.
  S : char[15] = "Hello, world!\n";

  // If no bounds are specified, the compiler computes the bounds.
  R : char[] = "Hello, world!\n";

var
  A : integer[M][N][O];

begin
end constants.

