#include "CUnconcat.h"

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
  bool   tabulate = false;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (argv[i][1] == 't')
        tabulate = true;
      else
        cerr << "Invalid option " << argv[i] << endl;
    }
    else {
      if (filename != "") {
        cerr << "Usage - " << argv[0] << " <file>" << endl;
        exit(1);
      }

      filename = argv[i];
    }
  }

  if (filename == "") {
    fprintf(stderr, "Usage - %s [-t] <file>\n", argv[0]);
    exit(1);
  }

  CUnconcat unconcat;

  unconcat.setFileName(filename);
  unconcat.setTabulate(tabulate);

  if (! unconcat.exec())
    exit(1);

  return 0;
}

CUnconcat::
CUnconcat() :
 tabulate_(false), check_pos_(0)
{
  memset(id_, 0, sizeof(id_));
}

bool
CUnconcat::
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
    string output_file;

    c = fgetc(fp);

    while (c != '\n' && c != EOF) {
      output_file += char(c);

      c = fgetc(fp);
    }

    if (c == EOF) {
      cerr << "Invalid Concat File " << filename_ << endl;
      exit(1);
    }

    bytes_written_ = 0;

    FILE *fp1 = NULL;

    if (tabulate_)
      cout << output_file;
    else {
      if (filename_ == output_file) {
        cerr << "Input and Output File are the same" << endl;
        exit(1);
      }

      fp1 = fopen(output_file.c_str(), "w");

      if (fp1 == NULL) {
        cerr << "Can't Open Output File " << output_file << endl;
        exit(1);
      }
    }

    while ((c = fgetc(fp)) != EOF)
      if (check_match(fp1, c))
        break;

    check_match(fp1, EOF);

    if (fp1 != NULL)
      fclose(fp1);

    if (tabulate_)
      cout << " " << bytes_written_ << " bytes" << endl;

    if (c == EOF)
      break;
  }

  fclose(fp);

  return true;
}

bool
CUnconcat::
check_match(FILE *fp, int c)
{
  if (c != id_[check_pos_]) {
    if (fp != NULL) {
      uint len1 = check_buffer_.size();

      for (uint i = 0; i < check_pos_; i++)
        fputc(check_buffer_[i], fp);

      if (c != EOF)
        fputc(c, fp);
    }

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
CUnconcat::
getDefId()
{
  static string id = "##concat##";

  return id;
}
