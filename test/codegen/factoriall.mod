//
// factorial - longint version
//
// factorial computation
//

module factoriall;

var l : longint;

function fact(n: longint): longint;
begin
  if (n <= 0L) then
    return 0L
  else
    if (n <= 1L) then
      return n
    else
      return n*fact(n-1L)
    end
  end
end fact;

function ReadNumber(str: char[]): longint;
var l: longint;
begin
  WriteStr(str);
  l := ReadLong();
  return l
end ReadNumber;

begin
  WriteStr("Factorials (longint)"); WriteLn();
  WriteLn();

  l := ReadNumber("Enter a number (0 to exit): ");

  while (l > 0L) do
    WriteStr("factorial("); WriteLong(l); WriteStr(") = ");
    WriteLong(fact(l)); WriteLn();

    l := ReadNumber("Enter a number (0 to exit): ")
  end
end factoriall.
