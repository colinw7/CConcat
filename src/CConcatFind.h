#ifndef CCONCAT_FIND_H
#define CCONCAT_FIND_H

#include <CConcatBase.h>
#include <CGlob.h>
#include <CRegExp.h>
#include <vector>

class CommentParser;

class CConcatFind : public CConcatBase {
 public:
  typedef std::vector<std::string> Strings;

 public:
  CConcatFind();
 ~CConcatFind();

  //---

  const std::string &filePattern() const { return filePattern_.str; }
  void setFilePattern(const std::string &s);

  //---

  const std::string &pattern() const { return pattern_.str; }
  void setPattern(const std::string &s);

  bool isGlob() const { return pattern_.isGlob; }
  void setGlob(bool b) { pattern_.isGlob = b; }

  bool isRegExp() const { return pattern_.isRegExp; }
  void setRegExp(bool b) { pattern_.isRegExp = b; }

  bool isNoCase() const { return pattern_.noCase; }
  void setNoCase(bool b) { pattern_.noCase = b; }

  bool isMatchWord() const { return pattern_.matchWord; }
  void setMatchWord(bool b) { pattern_.matchWord = b; }

  //---

  bool isList() const { return list_; }
  void setList(bool b) { list_ = b; }

  bool isShowFile() const { return showFile_; }
  void setShowFile(bool b) { showFile_ = b; }

  bool isNumber() const { return number_; }
  void setNumber(bool b) { number_ = b; }

  const Strings &extensions() const { return extensions_; }
  void setExtensions(const Strings &s) { extensions_ = s; }

  bool isMatchFile() const { return matchFile_; }
  void setMatchFile(bool b) { matchFile_ = b; }

  bool isMatchDir() const { return matchDir_; }
  void setMatchDir(bool b) { matchDir_ = b; }

  const std::string &root() const { return root_; }
  void setRoot(const std::string &s) { root_ = s; }

  bool isComment() const { return comment_; }
  void setComment(bool b) { comment_ = b; }

  bool isNoComment() const { return noComment_; }
  void setNoComment(bool b) { noComment_ = b; }

  bool exec();

  bool checkMatch(int c);

  bool checkLine(const std::string &line) const;

  void addLineChar(char c);

  int getChar() const;

 private:
  const std::string &currentFile() const { return currentFile_; }
  void setCurrentFile(const std::string &s) { currentFile_ = s; }

  int currentLine() const { return currentLine_; }
  void setCurrentLine(int i) { currentLine_ = i; }

  bool checkPattern(const std::string &s) const;

  bool checkFilePattern(const std::string &s) const;

  std::string getDirName() const;

  const std::string &lpattern() const { return pattern_.lstr; }

 private:
  struct PatternData {
    std::string str;
    CGlob       glob;
    CRegExp     regexp;
    std::string lstr;
    bool        isGlob    { false };
    bool        isRegExp  { false };
    bool        noCase    { false };
    bool        matchWord { false };
  };

  static bool checkPatternData(const std::string &str, const PatternData &data);

  static bool isWord(const std::string &str, int pos, int len);

  static std::string toLower(const std::string &str);

 private:
  PatternData    filePattern_;
  PatternData    pattern_;
  bool           list_          { false };
  bool           showFile_      { true };
  bool           number_        { false };
  Strings        extensions_;
  bool           matchFile_     { false };
  bool           matchDir_      { false };
  bool           comment_       { false };
  bool           noComment_     { false };
  std::string    root_;
  uint           bytesWritten_  { 0 };
  std::string    checkBuffer_;
  std::string    currentFile_;
  int            currentLine_   { 1 };
  CommentParser* commentParser_ { nullptr };
  FILE*          fp_            { nullptr };
  std::string    line_;
};

#endif
