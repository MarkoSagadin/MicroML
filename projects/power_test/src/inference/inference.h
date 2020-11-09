#ifndef INFERENECE_H
#define INFERENECE_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

bool inference_setup();
bool inference_exe();
void get_inference_results(char * buf, uint16_t max_len);

#ifdef __cplusplus
}
#endif

#endif /* INFERENECE_H */
/*** end of file ***/
