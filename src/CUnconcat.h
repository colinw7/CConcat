#ifndef CUNCONCAT_H
#define CUNCONCAT_H

class CUnconcat {
 private:
  char   id_[256];
  string filename_;
  uint   bytes_written_;
  string check_buffer_;
  uint   check_pos_;
  bool   tabulate_;

 public:
  CUnconcat();

  void setFileName(const string &filename) { filename_ = filename; }

  void setTabulate(bool tabulate) { tabulate_ = tabulate; }

  bool exec();

  bool check_match(FILE *fp, int c);

  const string &getDefId();
};

#endif
