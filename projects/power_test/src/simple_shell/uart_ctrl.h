#ifndef CONSOLE_H
#define CONSOLE_H

#ifdef __cplusplus
extern "C" {
#endif



void put_line(char *fmt, ...);
int get_line(char *s, int len);

#ifdef __cplusplus
}
#endif

#endif /* CONSOLE_H */
/*** end of file ***/

