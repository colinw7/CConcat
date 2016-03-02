#include <CConcatBase.h>
#include <iostream>
#include <cstdio>

CConcatBase::
CConcatBase() :
 checkPos_(0)
{
}

bool
CConcatBase::
readId(FILE *fp)
{
  id_ = "";

  int c = fgetc(fp);

  while (c != '\n' && c != EOF) {
    id_ += c;

    c = fgetc(fp);
  }

  if (c == EOF) {
    std::cerr << "Invalid Concat File " << filename() << std::endl;
    return false;
  }

  int len = id_.size();

  if (len == 0) {
    fprintf(stderr, "Null Concat Id\n");
    return false;
  }

  return true;
}

const std::string &
CConcatBase::
getDefId() const
{
  static std::string id = "##concat##";

  return id;
}
