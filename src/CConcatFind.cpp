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
    std::cerr << "  CConcatFind [-l|L] [-n] [-i] [-e] [-f] [-R] [-g] [-w] <file> <pattern>\n";
    std::cerr << "\n";
    std::cerr << "    -l|L          : list files containing pattern\n";
    std::cerr << "    -n            : show line number\n";
    std::cerr << "    -i            : no case matching\n";
    std::cerr << "    -e <exts>     : only match files with specified extensions\n";
    std::cerr << "    -f            : match only filename\n";
    std::cerr << "    -fp <pattern> : file pattern\n";
    std::cerr << "    -d            : match only directory\n";
    std::cerr << "    -R <root>     : root directory of file\n";
    std::cerr << "    -g            : glob match\n";
    std::cerr << "    -x            : regexp match\n";
    std::cerr << "    -w            : word match\n";
    std::cerr << "    -comment      : match comments only\n";
    std::cerr << "    -nocomment    : match no comments only\n";
    std::cerr << "    -h|--help     : help for usage\n";
  };

  std::string  filename;
  std::string  filePattern;

  std::string pattern;
  bool        glob      = false;
  bool        regexp    = false;
  bool        nocase    = false;
  bool        matchWord = false;

  std::string          root;
  CConcatFind::Strings extensions;

  bool list       = false;
  bool showFile   = true;
  bool number     = false;
  bool matchFile  = false;
  bool matchDir   = false;
  bool comment    = false;
  bool nocomment  = false;
  bool parse_args = true;

  for (int i = 1; i < argc; i++) {
    if (parse_args && argv[i][0] == '-') {
      std::string arg = &argv[i][1];

      if      (arg == "comment") {
        comment = true;
      }
      else if (arg == "nocomment" || arg == "no_comment") {
        nocomment = true;
      }
      else if (arg == "nofile" || arg == "no_file") {
        showFile = false;
      }
      else if (arg == "L" || arg == "l")
        list = true;
      else if (arg == "n")
        number = true;
      else if (arg == "i")
        nocase = true;
      else if (arg == "e") {
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
      else if (arg == "f") {
        matchFile = true;
      }
      else if (arg == "fp") {
        ++i;

        if (i < argc) {
          filePattern = argv[i];
        }
      }
      else if (arg == "d") {
        matchDir = true;
      }
      else if (arg == "R") {
        ++i;

        if (i < argc)
          root = std::string(argv[i]) + "/";
      }
      else if (arg == "g") {
        glob = true;
      }
      else if (arg == "x") {
        regexp = true;
      }
      else if (arg == "w") {
        matchWord = true;
      }
      else if (arg == "h" || arg == "-help") {
        showUsage();
        exit(0);
      }
      else if (arg == "-") {
        parse_args = false;
      }
      else {
        std::cerr << "Invalid option " << argv[i] << std::endl;
        exit(1);
      }
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

  find.setFilename(filename);

  find.setFilePattern(filePattern);

  find.setPattern  (pattern);
  find.setRegExp   (regexp);
  find.setGlob     (glob);
  find.setNoCase   (nocase);
  find.setMatchWord(matchWord);

  find.setList       (list);
  find.setShowFile   (showFile);
  find.setNumber     (number);
  find.setExtensions (extensions);
  find.setMatchFile  (matchFile);
  find.setMatchDir   (matchDir);
  find.setRoot       (root);
  find.setComment    (comment);
  find.setNoComment  (nocomment);

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
setFilePattern(const std::string &s)
{
  filePattern_.str = s;

  if (filePattern_.str != "") {
    filePattern_.glob    = CGlob("*" + filePattern_.str + "*");
    filePattern_.regexp  = CRegExp(".*" + filePattern_.str + ".*");
  }
  else {
    filePattern_.glob    = CGlob();
    filePattern_.regexp  = CRegExp();
  }

  filePattern_.lstr = filePattern_.str;
}

void
CConcatFind::
setPattern(const std::string &s)
{
  pattern_.str    = s;
  pattern_.glob   = CGlob("*" + pattern_.str + "*");
  pattern_.regexp = CRegExp(".*" + pattern_.str + ".*");

  if (isNoCase())
    pattern_.lstr = toLower(pattern_.str);
  else
    pattern_.lstr = pattern_.str;
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

    // check file name match
    if (filePattern_.str != "") {
      if (! checkFilePattern(currentFile()))
        skip = true;
    }

    //---

    // check file match
    bool found = false;

    if (isMatchFile()) {
      if (checkPattern(currentFile())) {
        if (! isMatchDir())
          std::cout << root() << currentFile() << std::endl;
        else
          std::cout << root() << getDirName() << std::endl;

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
            if (! isMatchDir())
              std::cout << root() << currentFile() << std::endl;
            else
              std::cout << root() << getDirName() << std::endl;

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

std::string
CConcatFind::
getDirName() const
{
  auto filename = currentFile();

  auto p = filename.rfind('/');

  if (p != std::string::npos)
    return filename.substr(0, p);

  return filename;
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
    if (isShowFile())
      std::cout << root() << currentFile() << ":";

    if (isNumber())
      std::cout << currentLine() << ": " << line << std::endl;
    else
      std::cout << line << std::endl;
  }

  return true;
}

bool
CConcatFind::
checkPattern(const std::string &str) const
{
  return checkPatternData(str, pattern_);
}

bool
CConcatFind::
checkFilePattern(const std::string &str) const
{
  return checkPatternData(str, filePattern_);
}

bool
CConcatFind::
checkPatternData(const std::string &str, const PatternData &data)
{
  if (data.isGlob)
    return data.glob.compare(str);

  if (data.isRegExp)
    return data.regexp.find(str);

  if (! data.noCase) {
    auto p = str.find(data.str);

    if (p == std::string::npos)
      return false;

    if (data.matchWord) {
      if (! isWord(str, p, data.str.size()))
        return false;
    }
  }
  else {
    std::string lstr = toLower(str);

    auto p = lstr.find(data.lstr);

    if (p == std::string::npos)
      return false;

    if (data.matchWord) {
      if (! isWord(lstr, p, data.lstr.size()))
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
toLower(const std::string &str)
{
  std::string lstr = str;

  for (size_t i = 0; i < str.size(); ++i)
    lstr[i] = tolower(lstr[i]);

  return lstr;
}

bool
CConcatFind::
isWord(const std::string &str, int p, int len)
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
