import os, re, sys, string

def ToCAsciiArray(lines):
  result = []
  for chr in lines:
    value = ord(chr)
    assert value < 128
    result.append(hex(value))
  return ", ".join(result)



def RemoveCommentsAndTrailingWhitespace(lines):
  lines = re.sub(r'//.*\n', '\n', lines) # end-of-line comments
  lines = re.sub(re.compile(r'/\*.*?\*/', re.DOTALL), '', lines) # comments.
  lines = re.sub(r'\s+\n+', '\n', lines) # trailing whitespace
  return lines


def ReadFile(filename):
  file = open(filename, "rt")
  try:
    lines = file.read()
  finally:
    file.close()
  return lines



def main():
  source_files = sys.argv[2:]
  output = open(sys.argv[1], 'w')
  for source_file in source_files:
    lines = ReadFile(source_file)
    # lines = RemoveCommentsAndTrailingWhitespace(lines)
    var_name = "embedded_" + re.sub(r'[./\\]', '_', source_file)
    o = "const char " + var_name + "[] = {\n" + ToCAsciiArray(lines + "\0") + "\n};\n"

    output.write(o)
  output.close()


if __name__ == "__main__":
  main()


# cat input_file | ( echo "unsigned char xxx[] = {"; xxd -i; echo "};" ) > output_file.c

