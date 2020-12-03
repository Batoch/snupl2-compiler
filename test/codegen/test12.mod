//
// test12
//
// function return values
//
// expected output: returned values are stored according to 
// their size
//

module test12;

function Longint(): longint;
var l: longint;
begin
  l := 19750307L;
  return l
end Longint;

function Integer(): integer;
var i: integer;
begin
  i := 75;
  return i
end Integer;

function Char(): char;
var c: char;
begin
  c := 'B';
  return c
end Char;

function Boolean(): boolean;
var b: boolean;
begin
  b := true;
  return b
end Boolean;

procedure Test();
var b: boolean;
    c: char;
    i: integer;
    l: longint;
begin
  WriteStr("Function return values:\n");

  b := Boolean();
  WriteStr("  b = ");
  if (b = true) then WriteStr("true") else WriteStr("false") end;
  WriteLn();

  c := Char();
  WriteStr("  c = "); WriteChar(c); WriteLn();

  i := Integer();
  WriteStr("  i = "); WriteInt(i); WriteLn();

  l := Longint();
  WriteStr("  l = "); WriteLong(l); WriteLn();

  WriteStr("Done.\n")
end Test;

begin
  Test()
end test12.
