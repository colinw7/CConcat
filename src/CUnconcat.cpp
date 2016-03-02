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

  unconcat.setFilename(filename);
  unconcat.setTabulate(tabulate);

  if (! unconcat.exec())
    exit(1);

  return 0;
}

CUnconcat::
CUnconcat()
{
}

bool
CUnconcat::
exec()
{
  FILE *fp = fopen(filename_.c_str(), "rb");

  if (fp == 0) {
    std::cerr << "Can't Open Input File " << filename() << std::endl;
    return false;
  }

  char buffer[256];

  int no = fread(buffer, 1, 10, fp);

  if (no != 10 || strncmp(buffer, "CONCAT_ID=", 10) != 0) {
    std::cerr << "Invalid Concat File " << filename() << std::endl;
    exit(1);
  }

  if (! readId(fp))
    exit(1);

  uint len = id_.size();

  no = fread(buffer, 1, len, fp);

  if (no != len || strncmp(buffer, id_.c_str(), len) != 0) {
    std::cerr << "Invalid Concat File " << filename() << std::endl;
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
      std::cerr << "Invalid Concat File " << filename() << std::endl;
      exit(1);
    }

    bytesWritten_ = 0;

    FILE *fp1 = 0;

    if (isTabulate())
      std::cout << output_file;
    else {
      if (filename() == output_file) {
        std::cerr << "Input and Output File are the same" << std::endl;
        exit(1);
      }

      fp1 = fopen(output_file.c_str(), "w");

      if (fp1 == 0) {
        std::cerr << "Can't Open Output File " << output_file << std::endl;
        exit(1);
      }
    }

    while ((c = fgetc(fp)) != EOF)
      if (check_match(fp1, c))
        break;

    check_match(fp1, EOF);

    if (fp1 != 0)
      fclose(fp1);

    if (isTabulate())
      std::cout << " " << bytesWritten_ << " bytes" << std::endl;

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
  if (c != id_[checkPos_]) {
    if (fp != 0) {
      uint len1 = checkBuffer_.size();

      for (uint i = 0; i < checkPos_; i++)
        fputc(checkBuffer_[i], fp);

      if (c != EOF)
        fputc(c, fp);
    }

    bytesWritten_ += checkPos_;

    if (c != EOF)
      ++bytesWritten_;

    checkPos_    = 0;
    checkBuffer_ = "";

    return false;
  }

  checkBuffer_ += char(c);

  ++checkPos_;

  uint len = id_.size();

  if (checkPos_ >= len) {
    checkPos_ = 0;

    return true;
  }

  return false;
}
