#ifndef CUNCONCAT_H
#define CUNCONCAT_H

#include <CConcatBase.h>

class CUnconcat : public CConcatBase {
 public:
  CUnconcat();

  bool isTabulate() const { return tabulate_; }
  void setTabulate(bool b) { tabulate_ = b; }

  bool isCount() const { return count_; }
  void setCount(bool b) { count_ = b; }

  int fileNum() const { return fileNum_; }
  void setFileNum(int i) { fileNum_ = i; }

  bool exec();

  bool check_match(FILE *fp, int c);

 private:
  uint        bytesWritten_ { 0 };
  std::string checkBuffer_;
  int         fileNum_      { -1 };
  bool        tabulate_     { false };
  bool        count_        { false };
};

#endif
