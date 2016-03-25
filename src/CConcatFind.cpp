#include <CConcatFind.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

int
main(int argc, char **argv)
{
  std::string          filename;
  std::string          pattern;
  std::string          root;
  bool                 list = false;
  bool                 number = false;
  bool                 matchFile = false;
  bool                 glob = false;
  CConcatFind::Strings extensions;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if      (argv[i][1] == 'L' || argv[i][1] == 'l')
        list = true;
      else if (argv[i][1] == 'n')
        number = true;
      else if (argv[i][1] == 'e') {
        ++i;

        if (i < argc) {
          std::string exts = argv[i];

          std::string ext;

          for (int j = 0; j < exts.size(); ++j) {
            if (exts[j] == '|') {
              if (ext != "")
                extensions.push_back(ext);

              ext = "";
            }
            else
              ext += exts[j];
          }

          if (ext != "")
            extensions.push_back(ext);
        }
      }
      else if (argv[i][1] == 'f') {
        matchFile = true;
      }
      else if (argv[i][1] == 'R') {
        ++i;

        if (i < argc)
          root = std::string(argv[i]) + "/";
      }
      else if (argv[i][1] == 'g') {
        glob = true;
      }
      else
        std::cerr << "Invalid option " << argv[i] << std::endl;
    }
    else {
      if      (filename == "")
        filename = argv[i];
      else if (pattern == "")
        pattern = argv[i];
      else {
        std::cerr << "Usage - " << argv[0] << " <file>" << std::endl;
        exit(1);
      }
    }
  }

  if (filename == "") {
    fprintf(stderr, "Usage - %s [-t] <file>\n", argv[0]);
    exit(1);
  }

  CConcatFind find;

  find.setFilename  (filename);
  find.setPattern   (pattern);
  find.setList      (list);
  find.setNumber    (number);
  find.setExtensions(extensions);
  find.setMatchFile (matchFile);
  find.setGlob      (glob);
  find.setRoot      (root);

  if (! find.exec())
    exit(1);

  return 0;
}

CConcatFind::
CConcatFind()
{
}

void
CConcatFind::
setPattern(const std::string &s)
{
  pattern_ = s;
  glob_    = CGlob("*" + pattern_ + "*");
}

bool
CConcatFind::
exec()
{
  FILE *fp = fopen(filename().c_str(), "rb");

  if (fp == NULL) {
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
    setCurrentFile("");

    int c = fgetc(fp);

    while (c != '\n' && c != EOF) {
      currentFile_ += char(c);

      c = fgetc(fp);
    }

    if (c != '\n') {
      std::cerr << "Invalid Concat File " << filename() << std::endl;
      exit(1);
    }

    //---

    bool skip = false;

    if (! extensions().empty()) {
      std::string::size_type p = currentFile().rfind('.');

      std::string ext;

      if (p != std::string::npos)
        ext = currentFile().substr(p + 1);

      skip = true;

      for (int j = 0; j < extensions().size(); ++j) {
        if (extensions()[j] == ext) {
          skip = false;
          break;
        }
      }
    }

    //---

    bool found = false;

    if (isMatchFile()) {
      if (checkPattern(currentFile())) {
        std::cout << root() << currentFile() << std::endl;
        found = true;
      }

      skip = true;
    }

    //---

    currentLine_ = 1;

    std::string line;

    bytesWritten_ = 0;

    while ((c = fgetc(fp)) != EOF) {
      if (checkMatch(c))
        break;

      if (c == '\n') {
        if (! skip && ! found) {
          bool found1 = checkLine(line);

          if (isList() && found1) {
            std::cout << root() << currentFile() << std::endl;
            found = true;
          }
        }

        line = "";

        ++currentLine_;
      }
      else
        line += c;
    }

    checkMatch(EOF);

    if (c == EOF)
      break;
  }

  fclose(fp);

  return true;
}

bool
CConcatFind::
checkLine(const std::string &line) const
{
  if (! checkPattern(line))
    return false;

  if (! isList()) {
    if (isNumber())
      std::cout << root() << currentFile() << ":" << currentLine() << ": " << line << std::endl;
    else
      std::cout << root() << currentFile() << ": " << line << std::endl;
  }

  return true;
}

bool
CConcatFind::
checkPattern(const std::string &s) const
{
  if (isGlob())
    return glob_.compare(s);
  else
    return (s.find(pattern()) != std::string::npos);
}

bool
CConcatFind::
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
