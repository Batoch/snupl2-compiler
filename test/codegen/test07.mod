//
// test07
//
// strings
//
// expected output: "Hello, world 1", "Hello, world 2"
//

module test07;

const
  HelloWorld : char[] = "Hello, world 2";

begin
  WriteStr("Hello, world 1"); WriteLn();
  WriteStr(HelloWorld); WriteLn()
end test07.
