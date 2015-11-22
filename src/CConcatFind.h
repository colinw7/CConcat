#ifndef CCONCAT_FIND_H
#define CCONCAT_FIND_H

#include <CConcatBase.h>
#include <vector>

class CConcatFind : public CConcatBase {
 public:
  typedef std::vector<std::string> Strings;

 public:
  CConcatFind();

  void setPattern(const std::string &pattern) { pattern_ = pattern; }

  void setList      (bool list          ) { list_       = list  ; }
  void setNumber    (bool number        ) { number_     = number; }
  void setExtensions(const Strings &strs) { extensions_ = strs  ; }

  bool exec();

  bool check_match(int c);

  bool check_line(const std::string &line) const;

 private:
  std::string pattern_;
  bool        list_;
  bool        number_;
  Strings     extensions_;
  uint        bytes_written_;
  std::string check_buffer_;
  std::string current_file_;
  int         current_line_;
};

#endif
