//
// test04
//
// scanner test: valid SnuPL/1 program
//

module test04;

const
  N : integer = 2*(2+2);
  ProgramName : char[] = "Matrix\t Adder\n\n";

var
  A, B, C : integer[N][N];

procedure minit(m: integer[][]);
var x,y,v: integer;
begin
  v := 1;
  while (y < N) do
    x := 0;
    while (x < N) do
      m[y][x] := v;
      v := v+1;
      if (v = 10) then v := 0 end;
      x := x+1
    end;
    y := y+1
  end
end minit;

procedure madd(sum: integer[N][N]; a,b: integer[N][N]);
var x,y: integer;
begin
  y := 0;
  while (y < N) do
    x := 0;
    while (x < N) do
      sum[y][x] := a[y][x] + b[y][x];
      x := x+1
    end;
    y := y+1
  end
end madd;

procedure mprint(m: integer[][]; title: char[]);
const MStr : char[] = ". Matrix ";
var N,M,x,y: integer;
begin
  M := DIM(m, 1);
  N := DIM(m, 2);

  WriteStr(title); WriteStr(MStr); WriteInt(M); WriteChar('x'); WriteInt(N); 
  WriteLn();
  WriteStr("[\n");

  while (y < M) do
    WriteStr("  "); WriteInt(y); WriteStr(":  [   ");

    x := 0;
    while (x < N) do
      WriteInt(m[y][x]); WriteStr("   ");
      x := x+1
    end;

    WriteStr("]\n");
    y := y+1
  end;

  WriteStr("]\n\n")
end mprint;

begin
  WriteStr(ProgramName); WriteLn();

  minit(A);
  minit(B);
  minit(C);

  mprint(A, "A");
  mprint(B, "B");

  madd(C, A, B);

  mprint(C, "C = A+B")
end test04.
