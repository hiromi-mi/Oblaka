// include guard
#pragma once

#ifndef OBLAKA_OBLAKA_H
#define OBLAKA_OBLAKA_H

#define OBLAKA_NOMATCH -1

#define _XOPEN_SOURCE
#include <locale.h>
#include <stdlib.h>
#include <malloc.h>
#include <wchar.h>

typedef enum TokenKind {
   tSTRING,
   tLEFTPARENSIS,
   tRIGHTPARENSIS,
   tSELECT
};

typedef struct {
   TokenKind kind;
   wchar_t *str;
} Token;

typedef struct {
   wchar_t *buf;
   size_t len;
   size_t reserved_len;
} WString;

void *oblaka_safe_malloc(size_t size) {
   return malloc(size);
}

void oblaka_safe_free(void *ptr) {
   free(ptr);
   ptr = NULL;
}

int match(wchar_t *pattern, wchar_t *regexp);
int match_here(wchar_t *pattern, wchar_t *regexp, unsigned int *pat_pos,
               unsigned int *reg_pos);
int match_star(wchar_t *c, wchar_t *pattern, wchar_t *regexp,
               unsigned int *pat_pos, unsigned int *reg_pos);

int match_star(wchar_t *c, wchar_t *pattern, wchar_t *regexp,
               unsigned int *pat_pos, unsigned int *reg_pos) {
   unsigned int c_pos = 0;
   while (pattern[*pat_pos++] == c[c_pos++]) {
      if (c[c_pos] == '\0') {
         break;
      }
   }
   return match_here(pattern, regexp, pat_pos, reg_pos);
}

int match_paren(wchar_t *pattern, wchar_t *regexp, unsigned int *pat_pos,
                unsigned int *reg_pos) {
   int parensis_count = 0;
   int index_id = 0;
   for (unsigned int i = pat_pos;; i++) {
      if (regexp[i] == '\0') {
         break;
      }
      if (regexp[i] == '(' && (i == 0 || regexp[i - 1] != '\\')) {
         parensis_count++;
      }
      if (regexp[i] == ')' && (i == 0 || regexp[i - 1] != '\\')) {
         parensis_count--;
         if (parensis_count <= 0) {
            break;
         }
      }
      if (regexp[i] == '|' && (i == 0 || regexp[i - 1] != '\\')) {
         wchar_t *part_regexp =
             (wchar_t *)malloc(sizeof(wchar_t) * (i - *reg_pos));
         wchar_t *part_pat =
             (wchar_t *)malloc(sizeof(wchar_t) * (wcslen(pattern) - *pat_pos));
         unsigned int part_pat_pos = 0;
         unsigned int part_reg_pos = 0;
         wcsncpy(part_regexp, &regexp[*pat_pos], i - *pat_pos - 1);
         wcsncpy(part_pat, pattern, wcslen(pattern) - *pat_pos - 1);

         int result =
             match_here(part_pat, part_regexp, part_pat_pos, part_reg_pos);
         if (result != 0) {
            match_here();
         }
         free(part_regexp);
         free(part_pat);
      }
      // 最初のpos は ( から.
      return OBLAKA_NOMATCH;
   }
}

int match_here(wchar_t *pattern, wchar_t *regexp, unsigned int *pat_pos,
               unsigned int *reg_pos) {
   if (regexp[*reg_pos] == '\0' || regexp[*reg_pos] == '$') {
      return 1;
   }
   if (pattern[*pat_pos] == '\0') {
      return OBLAKA_NOMATCH;
   }
   if (regexp[*reg_pos] == '*') {
      return match_star(regexp, pattern, regexp + 2, pat_pos, reg_pos);
   }

   if (regexp[*reg_pos] == '\\') {
      reg_pos++;
   }

   /*
   //面倒なので後から
   if (regexp[0] == '(') {
   }
   */

   // マッチ
   if (pattern[*pat_pos] == regexp[*reg_pos]) {
      if (regexp[*reg_pos + 1] == '(' && regexp[*reg_pos] != '\\') {
         // 繰り返しの場合
         ++pat_pos;
         ++reg_pos;
         return match_paren(pattern, regexp, pat_pos, reg_pos);
      } else {
         ++pat_pos;
         ++reg_pos;
         return match_here(pattern, regexp, pat_pos, reg_pos);
      }
   }
   return OBLAKA_NOMATCH;
}

void oblaka_tokenize_regexp(Token* tokens, const wchar_t *regexp) {
}

int match(const wchar_t *pattern, const wchar_t *regexp) {
   /*
   unsigned int pat_pos = 0;
   unsigned int reg_pos = 0;
   if (regexp[reg_pos] == '^') {
      reg_pos++;
      return match_here(pattern, regexp, &pat_pos, &reg_pos);
   }
   do {
      if (match_here(pattern, regexp, &pat_pos, &reg_pos) == 1) {
         return 1;
      }
   } while (pattern[pat_pos] != '\0');
   return OBLAKA_NOMATCH;
   */
   Token *tokens = oblaka_safe_malloc(sizeof(Token) * 128);
   oblaka_tokenize_regexp(tokens, regexp);
   oblaka_safe_free(tokens);
   return OBLAKA_NOMATCH;
}

#endif
