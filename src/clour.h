#ifndef __CLOUR_H
#define __CLOUR_H

#define CLOUR_BEGIN  "\033[%dm"
#define CLOUR_END    "\033[0m"

typedef  int CR;

extern CR bg_red;
extern CR bg_green;
extern CR bg_yellow;

extern CR font_red;
extern CR font_green;
extern CR font_yellow;

extern CR hightlight;
extern CR underline;
extern CR flash;
extern CR exchange;
extern CR blank;

#endif
