#ifndef PLASMA_CLASSIC_H
#define PLASMA_CLASSIC_H

#include "screensave/config_api.h"

const char *plasma_classic_canonical_key(const char *key);
unsigned int plasma_classic_preset_count(void);
unsigned int plasma_classic_theme_count(void);
const screensave_preset_descriptor *plasma_find_preset_descriptor(const char *preset_key);
int plasma_classic_is_known_preset_key(const char *preset_key);
int plasma_classic_is_known_theme_key(const char *theme_key);

#endif /* PLASMA_CLASSIC_H */
