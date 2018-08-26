#include <CConcatReplace.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

int
main(int argc, char **argv)
{
  auto showUsage = []() {
    std::cerr << "Usage:";
    std::cerr << "  CConcatReplace [-from <patterns>] [-to <patterns>] file\n";
    std::cerr << "\n";
    std::cerr << "    -from <patterns> : from patterns\n";
    std::cerr << "    -to <patterns>   : to patterns\n";
    std::cerr << "    -filename        : replace in filename\n";
    std::cerr << "    -contents        : replace in file contents\n";
    std::cerr << "    -h|--help        : help for usage\n";
  };

  std::string             filename;
  CConcatReplace::Strings fromPatterns;
  CConcatReplace::Strings toPatterns;
  bool                    replaceFilename = false;
  bool                    replaceContents = false;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if      (strcmp(&argv[i][1], "from") == 0) {
        ++i;

        if (i < argc)
          fromPatterns.push_back(argv[i]);
      }
      else if (strcmp(&argv[i][1], "to") == 0) {
        ++i;

        if (i < argc)
          toPatterns.push_back(argv[i]);
      }
      else if (strcmp(&argv[i][1], "filename") == 0) {
        replaceFilename = true;
      }
      else if (strcmp(&argv[i][1], "contents") == 0) {
        replaceContents = true;
      }
      else if (argv[i][1] == 'h' || strcmp(&argv[i][1], "-help") == 0) {
        showUsage();
        exit(0);
      }
      else
        std::cerr << "Invalid option " << argv[i] << std::endl;
    }
    else {
      if      (filename == "")
        filename = argv[i];
      else {
        showUsage();
        exit(1);
      }
    }
  }

  if (filename == "") {
    showUsage();
    exit(1);
  }

  if (! replaceFilename && ! replaceContents)
    replaceContents = true;

  CConcatReplace replace;

  replace.setFromPatterns   (fromPatterns);
  replace.setToPatterns     (toPatterns);
  replace.setFilename       (filename);
  replace.setReplaceFilename(replaceFilename);
  replace.setReplaceContents(replaceContents);

  if (! replace.exec())
    exit(1);

  return 0;
}

CConcatReplace::
CConcatReplace()
{
}

CConcatReplace::
~CConcatReplace()
{
}

void
CConcatReplace::
setFromPatterns(const Strings &strs)
{
  fromPatterns_ = strs;
}

void
CConcatReplace::
setToPatterns(const Strings &strs)
{
  toPatterns_ = strs;
}

bool
CConcatReplace::
exec()
{
  // open file
  fp_ = fopen(filename().c_str(), "rb");

  if (! fp_) {
    std::cerr << "Can't Open Input File " << filename() << std::endl;
    return false;
  }

  //---

  // read concat id line
  char buffer[256];

  int no = fread(buffer, 1, 10, fp_);

  if (no != 10 || strncmp(buffer, "CONCAT_ID=", 10) != 0) {
    std::cerr << "Invalid Concat File " << filename() << std::endl;
    exit(1);
  }

  if (! readId(fp_))
    exit(1);

  //---

  // read id
  uint len = id_.size();

  no = fread(buffer, 1, len, fp_);

  if (no != len || strncmp(buffer, id_.c_str(), len) != 0) {
    std::cerr << "Invalid Concat File " << filename() << std::endl;
    exit(1);
  }

  std::cout << "CONCAT_ID=" << id_ << "\n";

  //---

  // read file chars
  while (true) {
    // get file name

    setCurrentFile("");

    int c = getChar();

    while (c != '\n' && c != EOF) {
      currentFile_ += char(c);

      c = getChar();
    }

    if (c != '\n') {
      std::cerr << "Invalid Concat File " << filename() << std::endl;
      exit(1);
    }

    if (replaceFilename()) {
      processLine(currentFile_);
    }

    std::cout << id_ << currentFile_ << "\n";

    //---

    // process lines
    currentLine_ = 1;

    line_ = "";

    bytesWritten_ = 0;

    while ((c = getChar()) != EOF) {
      if (checkMatch(c))
        break;

      if (c == '\n') {
        processLine(line_);

        std::cout << line_ << "\n";

        line_ = "";

        ++currentLine_;
      }
      else {
        addLineChar(c);
      }
    }

    checkMatch(EOF);

    if (c == EOF)
      break;
  }

  fclose(fp_);

  return true;
}

void
CConcatReplace::
addLineChar(char c)
{
  if (c == '\n')
    c = ' ';

  line_ += c;
}

int
CConcatReplace::
getChar() const
{
  return fgetc(fp_);
}

bool
CConcatReplace::
checkMatch(int c)
{
  if (c != id_[checkPos_]) {
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

bool
CConcatReplace::
processLine(std::string &line) const
{
  int nf = fromPatterns_.size();
  int nt = toPatterns_  .size();

  for (int i = 0; i < std::min(nt, nf); ++i) {
    const std::string &fromPattern = fromPatterns_[i];

    auto pos = line.find(fromPattern);

    if (pos != std::string::npos) {
      const std::string &toPattern = toPatterns_[i];

      line = line.substr(0, pos) + toPattern + line.substr(pos + fromPattern.size());
    }
  }

  return true;
}
