#ifndef SCREENSAVE_VERSION_H
#define SCREENSAVE_VERSION_H

#define SCREENSAVE_VERSION_MAJOR 0
#define SCREENSAVE_VERSION_MINOR 5
#define SCREENSAVE_VERSION_PATCH 0
#define SCREENSAVE_VERSION_TEXT "0.5.0"
#define SCREENSAVE_VERSION_SERIES "Series 05 mandatory GDI renderer"

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
