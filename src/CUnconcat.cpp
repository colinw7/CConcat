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
  bool        count    = false;
  int         fileNum  = -1;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if      (argv[i][1] == '-')
        filename = "--";
      else if (argv[i][1] == 't')
        tabulate = true;
      else if (argv[i][1] == 'C')
        count = true;
      else if (argv[i][1] == 'N') {
        ++i;

        if (i < argc)
          fileNum = atoi(argv[i]);
        else
          std::cerr << "Missing value for " << argv[i - 1] << "\n";
      }
      else
        std::cerr << "Invalid option " << argv[i] << "\n";
    }
    else {
      if (filename != "") {
        std::cerr << "Usage - " << argv[0] << " <file>\n";
        exit(1);
      }

      filename = argv[i];
    }
  }

  if (filename == "") {
    fprintf(stderr, "Usage - %s [-t] [-C] [-N <n>] <file>\n", argv[0]);
    exit(1);
  }

  CUnconcat unconcat;

  unconcat.setFilename(filename);
  unconcat.setFileNum (fileNum);
  unconcat.setTabulate(tabulate);
  unconcat.setCount   (count);

  if (! unconcat.exec())
    exit(1);

  return 0;
}

//------

CUnconcat::
CUnconcat()
{
}

bool
CUnconcat::
exec()
{
  FILE *fp;

  if (filename_ == "--")
    fp = stdin;
  else {
    fp = fopen(filename_.c_str(), "rb");

    if (! fp) {
      std::cerr << "Can't Open Input File " << filename() << "\n";
      return false;
    }
  }

  // read concat id line and id
  char buffer[256];

  int no = fread(buffer, 1, 10, fp);

  if (no != 10 || strncmp(buffer, "CONCAT_ID=", 10) != 0) {
    std::cerr << "Invalid Concat File " << filename() << "\n";
    exit(1);
  }

  if (! readId(fp))
    exit(1);

  //---

  // read concat id from filename line
  uint len = id_.size();

  no = fread(buffer, 1, len, fp);

  if (no != len || strncmp(buffer, id_.c_str(), len) != 0) {
    std::cerr << "Invalid Concat File " << filename() << "\n";
    exit(1);
  }

  //---

  int numFiles = 0;

  while (true) {
    // read filename
    std::string output_file;

    int c = fgetc(fp);

    while (c != '\n' && c != EOF) {
      output_file += char(c);

      c = fgetc(fp);
    }

    if (c == EOF) {
      std::cerr << "Invalid Concat File " << filename() << "\n";
      exit(1);
    }

    //---

    bytesWritten_ = 0;

    FILE *fp1 = 0;

    // output filename
    if      (isTabulate()) {
      std::cout << output_file;
    }
    // increment file count
    else if (isCount()) {
      ++numFiles;
    }
    else {
      bool output = true;

      if (fileNum() > 0) {
        ++numFiles;

        output = (fileNum() == numFiles);
      }

      //---

      // open output file if needed
      if (output) {
        if (filename() == output_file) {
          std::cerr << "Input and Output File are the same\n";
          exit(1);
        }

        fp1 = fopen(output_file.c_str(), "w");

        if (! fp1) {
          std::cerr << "Can't Open Output File " << output_file << "\n";
          exit(1);
        }
      }
    }

    // read to end of file (new id/filename line)
    while ((c = fgetc(fp)) != EOF)
      if (check_match(fp1, c))
        break;

    check_match(fp1, EOF);

    // close file
    if (fp1)
      fclose(fp1);

    //---

    // output number of bytes in file for tabulate
    if (isTabulate())
      std::cout << " " << bytesWritten_ << " bytes\n";

    //---

    if (c == EOF)
      break;
  }

  // close input file
  if (fp != stdin)
    fclose(fp);

  //---

  // output file count
  if (isCount())
    std::cout << numFiles << "\n";

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
