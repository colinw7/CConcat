#ifndef CCONCAT_REPLACE_H
#define CCONCAT_REPLACE_H

#include <CConcatBase.h>
#include <vector>

class CConcatReplace : public CConcatBase {
 public:
  typedef std::vector<std::string> Strings;

 public:
  CConcatReplace();
 ~CConcatReplace();

  const Strings &fromPatterns() const { return fromPatterns_; }
  void setFromPatterns(const Strings &strs);

  const Strings &toPatterns() const { return toPatterns_; }
  void setToPatterns(const Strings &strs);

  bool replaceFilename() const { return replaceFilename_; }
  void setReplaceFilename(bool b) { replaceFilename_ = b; }

  bool replaceContents() const { return replaceContents_; }
  void setReplaceContents(bool b) { replaceContents_ = b; }

  bool exec();

  bool processLine(std::string &line) const;

  void addLineChar(char c);

  int getChar() const;

 private:
  const std::string &currentFile() const { return currentFile_; }
  void setCurrentFile(const std::string &s) { currentFile_ = s; }

  int currentLine() const { return currentLine_; }
  void setCurrentLine(int i) { currentLine_ = i; }

  bool checkMatch(int c);

 private:
  Strings        fromPatterns_;
  Strings        toPatterns_;
  bool           replaceFilename_ { false };
  bool           replaceContents_ { false };
  uint           bytesWritten_    { 0 };
  std::string    checkBuffer_;
  std::string    currentFile_;
  int            currentLine_     { 1 };
  FILE*          fp_              { nullptr };
  std::string    line_;
};

#endif
