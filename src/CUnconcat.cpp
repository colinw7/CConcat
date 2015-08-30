#include <CUnconcat.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

int
main(int argc, char **argv)
{
  std::string filename;
  bool        tabulate = false;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if (argv[i][1] == 't')
        tabulate = true;
      else
        std::cerr << "Invalid option " << argv[i] << std::endl;
    }
    else {
      if (filename != "") {
        std::cerr << "Usage - " << argv[0] << " <file>" << std::endl;
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
 tabulate_(false)
{
}

bool
CUnconcat::
exec()
{
  FILE *fp = fopen(filename_.c_str(), "rb");

  if (fp == NULL) {
    std::cerr << "Can't Open Input File " << filename_ << std::endl;
    return false;
  }

  char buffer[256];

  int no = fread(buffer, 1, 10, fp);

  if (no != 10 || strncmp(buffer, "CONCAT_ID=", 10) != 0) {
    std::cerr << "Invalid Concat File " << filename_ << std::endl;
    exit(1);
  }

  if (! readId(fp))
    exit(1);

  uint len = id_.size();

  no = fread(buffer, 1, len, fp);

  if (no != len || strncmp(buffer, id_.c_str(), len) != 0) {
    std::cerr << "Invalid Concat File " << filename_ << std::endl;
    exit(1);
  }

  while (true) {
    std::string output_file;

    int c = fgetc(fp);

    while (c != '\n' && c != EOF) {
      output_file += char(c);

      c = fgetc(fp);
    }

    if (c == EOF) {
      std::cerr << "Invalid Concat File " << filename_ << std::endl;
      exit(1);
    }

    bytes_written_ = 0;

    FILE *fp1 = NULL;

    if (tabulate_)
      std::cout << output_file;
    else {
      if (filename_ == output_file) {
        std::cerr << "Input and Output File are the same" << std::endl;
        exit(1);
      }

      fp1 = fopen(output_file.c_str(), "w");

      if (fp1 == NULL) {
        std::cerr << "Can't Open Output File " << output_file << std::endl;
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
      std::cout << " " << bytes_written_ << " bytes" << std::endl;

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

  uint len = id_.size();

  if (check_pos_ >= len) {
    check_pos_ = 0;

    return true;
  }

  return false;
}
