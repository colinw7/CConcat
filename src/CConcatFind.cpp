#include <CConcatFind.h>
#include <CCommentParser.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

class CommentParser : public CCommentParser {
 public:
  CommentParser(CConcatFind *find) :
   find_(find) {
  }

  void put_comment(char c) override {
    if (find_->isComment())
      find_->addLineChar(c);
  }

  void put_normal(char c) override {
    if (find_->isNoComment())
      find_->addLineChar(c);
  }

  int getChar() const override {
    return find_->getChar();
  }

 private:
  CConcatFind *find_ { nullptr };
};

//---

int
main(int argc, char **argv)
{
  auto showUsage = []() {
    std::cerr << "Usage:";
    std::cerr << "  CConcatFind [-l|L] [-n] [-i] [-e] [-f] [-R] [-g][ -w] <file> <pattern>\n";
    std::cerr << "\n";
    std::cerr << "    -l|L       : list files containing pattern\n";
    std::cerr << "    -n         : show line number\n";
    std::cerr << "    -i         : no case matching\n";
    std::cerr << "    -e <exts>  : only match files with specified extensions\n";
    std::cerr << "    -f         : match only filename\n";
    std::cerr << "    -R <root>  : root directory of file\n";
    std::cerr << "    -g         : glob match\n";
    std::cerr << "    -w         : word match\n";
    std::cerr << "    -comment   : match comments only\n";
    std::cerr << "    -nocomment : match no comments only\n";
    std::cerr << "    -h|--help  : help for usage\n";
  };

  std::string          filename;
  std::string          pattern;
  std::string          root;
  CConcatFind::Strings extensions;

  bool list      = false;
  bool number    = false;
  bool nocase    = false;
  bool matchFile = false;
  bool matchWord = false;
  bool comment   = false;
  bool nocomment = false;
  bool glob      = false;

  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      if      (strcmp(&argv[i][1], "comment") == 0) {
        comment = true;
      }
      else if (strcmp(&argv[i][1], "nocomment") == 0 ||
               strcmp(&argv[i][1], "no_comment") == 0) {
        nocomment = true;
      }
      else if (argv[i][1] == 'L' || argv[i][1] == 'l')
        list = true;
      else if (argv[i][1] == 'n')
        number = true;
      else if (argv[i][1] == 'i')
        nocase = true;
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
      else if (argv[i][1] == 'w') {
        matchWord = true;
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
      else if (pattern == "")
        pattern = argv[i];
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

  CConcatFind find;

  find.setNoCase    (nocase);
  find.setFilename  (filename);
  find.setPattern   (pattern);
  find.setList      (list);
  find.setNumber    (number);
  find.setExtensions(extensions);
  find.setMatchFile (matchFile);
  find.setMatchWord (matchWord);
  find.setGlob      (glob);
  find.setRoot      (root);
  find.setComment   (comment);
  find.setNoComment (nocomment);

  if (! find.exec())
    exit(1);

  return 0;
}

CConcatFind::
CConcatFind()
{
}

CConcatFind::
~CConcatFind()
{
  delete commentParser_;
}

void
CConcatFind::
setPattern(const std::string &s)
{
  pattern_ = s;
  glob_    = CGlob("*" + pattern_ + "*");

  if (isNoCase())
    lpattern_ = toLower(pattern_);
  else
    lpattern_ = pattern_;
}

bool
CConcatFind::
exec()
{
  if (isComment() || isNoComment())
    commentParser_ = new CommentParser(this);

  //---

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

    //---

    // check extensions
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

    // check file match
    bool found = false;

    if (isMatchFile()) {
      if (checkPattern(currentFile())) {
        std::cout << root() << currentFile() << std::endl;
        found = true;
      }

      skip = true;
    }

    //---

    // process lines
    currentLine_ = 1;

    line_ = "";

    bytesWritten_ = 0;

    while ((c = getChar()) != EOF) {
      if (checkMatch(c))
        break;

      if (c == '\n') {
        if (! skip && ! found) {
          bool found1 = checkLine(line_);

          if (isList() && found1) {
            std::cout << root() << currentFile() << std::endl;
            found = true;
          }
        }

        if (isComment() || isNoComment())
          commentParser_->processCChar(c);

        line_ = "";

        ++currentLine_;
      }
      else {
        if (isComment() || isNoComment())
          commentParser_->processCChar(c);
        else
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
CConcatFind::
addLineChar(char c)
{
  if (c == '\n')
    c = ' ';

  line_ += c;
}

int
CConcatFind::
getChar() const
{
  return fgetc(fp_);
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
checkPattern(const std::string &str) const
{
  if (isGlob())
    return glob_.compare(str);

  if (! isNoCase()) {
    auto p = str.find(pattern());

    if (p == std::string::npos)
      return false;

    if (matchWord_) {
      if (! isWord(str, p, pattern().size()))
        return false;
    }
  }
  else {
    std::string lstr = toLower(str);

    auto p = lstr.find(lpattern());

    if (p == std::string::npos)
      return false;

    if (matchWord_) {
      if (! isWord(lstr, p, lpattern().size()))
        return false;
    }
  }

  return true;
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

std::string
CConcatFind::
toLower(const std::string &str) const
{
  std::string lstr = str;

  for (size_t i = 0; i < str.size(); ++i)
    lstr[i] = tolower(lstr[i]);

  return lstr;
}

bool
CConcatFind::
isWord(const std::string &str, int p, int len) const
{
  // word characters [A-Z],[a-z],[0-9]_

  int pl = p - 1;

  if (pl > 0 && isalnum(str[pl]) || str[pl] == '_')
    return false;

  int pr = p + len;

  if (str[pr] != '\0' && isalnum(str[pr]) || str[pr] == '_')
    return false;

  return true;
}
