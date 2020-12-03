//
// constant declarations and use
//

module const03;

const N : integer = 5;
      M : integer = 5*(N+1);
      F : boolean = false;
      T : boolean = !F;
      S : char[] = "Hello, world!";

var A: integer[N][M];

begin
  WriteStr(S); WriteLn();
  WriteStr("N = "); WriteInt(N); WriteLn();
  WriteStr("M = "); WriteInt(M); WriteLn()
end const03.

