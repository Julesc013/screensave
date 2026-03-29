#ifndef SCREENSAVE_VERSION_H
#define SCREENSAVE_VERSION_H

#define SCREENSAVE_VERSION_MAJOR 0
#define SCREENSAVE_VERSION_MINOR 15
#define SCREENSAVE_VERSION_PATCH 0
#define SCREENSAVE_VERSION_TEXT "0.15.0"
#define SCREENSAVE_VERSION_SERIES "Series 15 Advanced GL Capability Path and Gallery"

typedef struct screensave_version_info_tag {
    unsigned short major;
    unsigned short minor;
    unsigned short patch;
    const char *version_text;
    const char *series_text;
} screensave_version_info;

const screensave_version_info *screensave_version_get_info(void);
const char *screensave_version_get_text(void);

#endif /* SCREENSAVE_VERSION_H */
