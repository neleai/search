#ifndef PATTERN
#define PATTERN(...)
#endif
PATTERN(name)
PATTERN(path)
PATTERN(pathname)
PATTERN(mime)
PATTERN(type)
PATTERN(contain)
PATTERN(rawcontain)
#undef PATTERN
#ifndef DATES
#define DATES(...)
#endif
DATES(time)
DATES(mtime)
DATES(atime)
DATES(ctime)
#undef DATES
#ifndef UGR
#define UGR(...)
#endif
UGR(user)
UGR(group)
#undef UGR
