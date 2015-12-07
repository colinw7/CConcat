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
  CConcatFind::Strings extensions;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if      (argv[i][1] == 'L')
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

  find.setFileName  (filename);
  find.setPattern   (pattern);
  find.setList      (list);
  find.setNumber    (number);
  find.setExtensions(extensions);
  find.setMatchFile (matchFile);
  find.setRoot      (root);

  if (! find.exec())
    exit(1);

  return 0;
}

CConcatFind::
CConcatFind() :
 list_(false), number_(false), matchFile_(false), current_line_(1)
{
}

bool
CConcatFind::
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
    current_file_ = "";

    int c = fgetc(fp);

    while (c != '\n' && c != EOF) {
      current_file_ += char(c);

      c = fgetc(fp);
    }

    if (c != '\n') {
      std::cerr << "Invalid Concat File " << filename_ << std::endl;
      exit(1);
    }

    //---

    bool skip = false;

    if (! extensions_.empty()) {
      std::string::size_type p = current_file_.rfind('.');

      std::string ext;

      if (p != std::string::npos)
        ext = current_file_.substr(p + 1);

      skip = true;

      for (int j = 0; j < extensions_.size(); ++j) {
        if (extensions_[j] == ext) {
          skip = false;
          break;
        }
      }
    }

    //---

    bool found = false;

    if (matchFile_) {
      std::string::size_type p = current_file_.find(pattern_);

      if (p != std::string::npos) {
        std::cout << root_ << current_file_ << std::endl;
        found = true;
      }

      skip = true;
    }

    //---

    current_line_ = 1;

    std::string line;

    bytes_written_ = 0;

    while ((c = fgetc(fp)) != EOF) {
      if (check_match(c))
        break;

      if (c == '\n') {
        if (! skip && ! found) {
          bool found1 = check_line(line);

          if (list_ && found1) {
            std::cout << root_ << current_file_ << std::endl;
            found = true;
          }
        }

        line = "";

        ++current_line_;
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
check_line(const std::string &line) const
{
  std::string::size_type p = line.find(pattern_);

  if (p == std::string::npos)
    return false;

  if (! list_) {
    if (number_)
      std::cout << root_ << current_file_ << ":" << current_line_ << ": " << line << std::endl;
    else
      std::cout << root_ << current_file_ << ": " << line << std::endl;
  }

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

  uint len = id_.size();

  if (check_pos_ >= len) {
    check_pos_ = 0;

    return true;
  }

  return false;
}