/* This file is part of Spica, released under MIT license.
 * See LICENSE for details.
 */

/* Author: Piotr Polesiuk, 2022 */

#ifndef __SPICA_SEQUENCE_H__
#define __SPICA_SEQUENCE_H__

typedef enum SImageSourceType {
  SIst_Pointer,
  SIst_File
} SImageSourceType_t;

typedef enum SImageFileType {
  SFtp_PNG,
  SFtp_SIWW
} SImageFileType_t;

typedef struct SImageSource {
  SImageSourceType_t type;
  union {
    SImage_t *ptr;
    struct {
      const char *fname;
      SFileType_t fileType;
    };
  }
} SImageSource_t;

typedef struct SSequenceElem {
  SImageSource_t image;
} SSequenceElem_t;

typedef struct SSequence {
  size_t length;
  size_t capacity;
  SSequenceElem_t *data;
} SSequence_t;

#endif /* __SPICA_SEQUENCE_H__ */
