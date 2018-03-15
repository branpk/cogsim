#ifndef OL_H
#define OL_H


#include <stdint.h>


typedef struct OlBlock OlBlock;
typedef struct OlField OlField;
typedef struct OlValue OlValue;


typedef enum {
  ol_dec = 1,
  ol_hex = 2,
  ol_fp = 4,
  ol_ident = 8,
  ol_str = 16,
  ol_block = 32,
} OlValueType;


struct OlValue {
  OlValueType type;
  union {
    uint64_t dec;
    uint64_t hex;
    double fp;
    char *ident;
    char *str;
    OlBlock *block;
  };
};

struct OlField {
  OlValue *key;
  OlValue *value;
  OlField *next;
};

struct OlBlock {
  OlField *head;
};


OlBlock *ol_parseFile(char *filename);
void ol_free(OlBlock *b);
char *ol_valueStr(OlValue *v);

OlValue *ol_checkField(OlBlock *b, char *ident, OlValueType types);
int ol_checkFieldInt(OlBlock *b, char *ident);
float ol_checkFieldFloat(OlBlock *b, char *ident);
OlBlock *ol_checkFieldArray(OlBlock *b, char *ident, OlValueType types);


#endif