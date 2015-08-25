#include <CConcatFind.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using std::string;
using std::cout;
using std::cerr;
using std::endl;

int
main(int argc, char **argv)
{
  string filename;
  string pattern;
  bool   list = false;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (argv[i][1] == 'L')
        list = true;
      else
        cerr << "Invalid option " << argv[i] << endl;
    }
    else {
      if      (filename == "")
        filename = argv[i];
      else if (pattern == "")
        pattern = argv[i];
      else {
        cerr << "Usage - " << argv[0] << " <file>" << endl;
        exit(1);
      }

    }
  }

  if (filename == "") {
    fprintf(stderr, "Usage - %s [-t] <file>\n", argv[0]);
    exit(1);
  }

  CConcatFind unconcat;

  unconcat.setFileName(filename);
  unconcat.setPattern (pattern);
  unconcat.setList    (list);

  if (! unconcat.exec())
    exit(1);

  return 0;
}

CConcatFind::
CConcatFind() :
 list_(false), check_pos_(0)
{
  memset(id_, 0, sizeof(id_));
}

bool
CConcatFind::
exec()
{
  FILE *fp = fopen(filename_.c_str(), "rb");

  if (fp == NULL) {
    cerr << "Can't Open Input File " << filename_ << endl;
    return false;
  }

  char buffer[256];

  int no = fread(buffer, 1, 10, fp);

  if (no != 10 || strncmp(buffer, "CONCAT_ID=", 10) != 0) {
    cerr << "Invalid Concat File " << filename_ << endl;
    exit(1);
  }

  int i = 0;

  int c = fgetc(fp);

  while (c != '\n' && c != EOF) {
    id_[i++] = c;

    c = fgetc(fp);
  }

  id_[i] = '\0';

  if (c == EOF) {
    cerr << "Invalid Concat File " << filename_ << endl;
    exit(1);
  }

  int len = strlen(id_);

  if (len == 0) {
    fprintf(stderr, "Null Concat Id\n");
    exit(1);
  }

  no = fread(buffer, 1, len, fp);

  if (no != len || strncmp(buffer, id_, len) != 0) {
    cerr << "Invalid Concat File " << filename_ << endl;
    exit(1);
  }

  while (true) {
    current_file_ = "";

    c = fgetc(fp);

    while (c != '\n' && c != EOF) {
      current_file_ += char(c);

      c = fgetc(fp);
    }

    if (c == EOF) {
      cerr << "Invalid Concat File " << filename_ << endl;
      exit(1);
    }

    string line;
    bool   found = false;

    bytes_written_ = 0;

    while ((c = fgetc(fp)) != EOF) {
      if (check_match(c))
        break;

      if (c == '\n') {
        if (! found) {
          bool found1 = check_line(line);

          if (list_ && found1) {
            std::cerr << current_file_ << std::endl;
            found = true;
          }
        }

        line = "";
      }
      else
        line += c;
    }

    check_match(EOF);

    if (c == EOF)
      break;
  }

  fclose(fp);

  return true;
}

bool
CConcatFind::
check_line(const string &line) const
{
  string::size_type p = line.find(pattern_);

  if (p == string::npos)
    return false;

  if (! list_)
    std::cerr << current_file_ << ": " << line << std::endl;

  return true;
}

bool
CConcatFind::
check_match(int c)
{
  if (c != id_[check_pos_]) {
    bytes_written_ += check_pos_;

    if (c != EOF)
      ++bytes_written_;

    check_pos_    = 0;
    check_buffer_ = "";

    return false;
  }

  check_buffer_ += char(c);

  ++check_pos_;

  uint len = strlen(id_);

  if (check_pos_ >= len) {
    check_pos_ = 0;

    return true;
  }

  return false;
}

const string &
CConcatFind::
getDefId()
{
  static string id = "##concat##";

  return id;
}
