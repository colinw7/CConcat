#ifndef CUNCONCAT_H
#define CUNCONCAT_H

#include <CConcatBase.h>

class CUnconcat : public CConcatBase {
 public:
  CUnconcat();

  bool isTabulate() const { return tabulate_; }
  void setTabulate(bool b) { tabulate_ = b; }

  bool exec();

  bool check_match(FILE *fp, int c);

 private:
  uint        bytesWritten_ { 0 };
  std::string checkBuffer_;
  bool        tabulate_     { false };
};

#endif
