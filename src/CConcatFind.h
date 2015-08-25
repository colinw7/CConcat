#ifndef CCONCAT_FIND_H
#define CCONCAT_FIND_H

#include <sys/types.h>
#include <string>

class CConcatFind {
 public:
  CConcatFind();

  void setFileName(const std::string &filename) { filename_ = filename; }
  void setPattern (const std::string &pattern ) { pattern_  = pattern ; }

  void setList(bool list) { list_ = list; }

  bool exec();

  bool check_match(int c);

  bool check_line(const std::string &line) const;

  const std::string &getDefId();

 private:
  char        id_[256];
  std::string filename_;
  std::string pattern_;
  uint        bytes_written_;
  std::string check_buffer_;
  uint        check_pos_;
  bool        list_;
  std::string current_file_;
};

#endif
