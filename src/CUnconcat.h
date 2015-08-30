#ifndef CUNCONCAT_H
#define CUNCONCAT_H

#include <CConcatBase.h>

class CUnconcat : public CConcatBase {
 public:
  CUnconcat();

  void setTabulate(bool tabulate) { tabulate_ = tabulate; }

  bool exec();

  bool check_match(FILE *fp, int c);

 private:
  uint        bytes_written_;
  std::string check_buffer_;
  bool        tabulate_;
};

#endif
