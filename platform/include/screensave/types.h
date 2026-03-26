#ifndef SCREENSAVE_TYPES_H
#define SCREENSAVE_TYPES_H

typedef struct screensave_pointi_tag {
    int x;
    int y;
} screensave_pointi;

typedef struct screensave_sizei_tag {
    int width;
    int height;
} screensave_sizei;

typedef struct screensave_recti_tag {
    int x;
    int y;
    int width;
    int height;
} screensave_recti;

typedef struct screensave_color_tag {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} screensave_color;

#endif /* SCREENSAVE_TYPES_H */
