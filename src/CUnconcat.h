#ifndef CUNCONCAT_H
#define CUNCONCAT_H

#include <sys/types.h>
#include <string>

class CUnconcat {
 public:
  CUnconcat();

  void setFileName(const std::string &filename) { filename_ = filename; }

  void setTabulate(bool tabulate) { tabulate_ = tabulate; }

  bool exec();

  bool check_match(FILE *fp, int c);

  const std::string &getDefId();

 private:
  char        id_[256];
  std::string filename_;
  uint        bytes_written_;
  std::string check_buffer_;
  uint        check_pos_;
  bool        tabulate_;
};

#endif
