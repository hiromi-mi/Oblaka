// include guard
#pragma once

#ifndef OBLAKA_OBLAKA_H
#define OBLAKA_OBLAKA_H

#define OBLAKA_NOMATCH -1

#define _XOPEN_SOURCE
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <wchar.h>

typedef enum {
   tSTRING,
   tLEFTPARENSIS,
   tRIGHTPARENSIS,
   tSELECT,
   tEOF
} TokenKind;

typedef struct {
   wchar_t *buf;
   size_t len;
   size_t reserved_len;
} WString;

typedef struct {
   TokenKind kind;
   WString* str;
} Token;

struct A_Delta {
   char* chdir_chars;
   struct State* new_state;
};
typedef struct State State;

void *oblaka_safe_malloc(size_t size) {
   void* result = malloc(size);
   if (result == NULL) {
      perror("malloc");
   }
   return result;
}

void *oblaka_safe_realloc (void* ptr, size_t size) {
   void* result = realloc(ptr, size);
   if (result == NULL) {
      perror("realloc");
   }
   return result;
}

void oblaka_safe_free(void *ptr) {
   free(ptr);
   ptr = NULL;
}

WString *oblaka_wstring_new(const wchar_t* initial_str) {
   WString *wstr = oblaka_safe_malloc(sizeof(WString));
   wstr->len = 1; // 文字列の消費するバイト長
   while (initial_str[wstr->len-1] != '\0') {
      wstr->len++;
   }
   wstr->reserved_len = wstr->len;
   wstr->buf = oblaka_safe_malloc(sizeof(wchar_t) * wstr->reserved_len);
   wcscpy(wstr->buf, initial_str);
   return wstr;
}

void oblaka_wstring_append_wchar(WString* dest, const wchar_t wchar) {
   if (dest->reserved_len <= dest->len) {
      // メモリ不足. dest->len が増加するからまずい.
      dest->buf = oblaka_safe_realloc(dest->buf, sizeof(wchar_t) * dest->reserved_len + 128);
      dest->reserved_len += 128;
   }
   dest->buf[dest->len-1] = wchar;
   dest->buf[dest->len++] = '\0';
}

void oblaka_wstring_free(WString* wstr) {
   oblaka_safe_free(wstr->buf);
   oblaka_safe_free(wstr);
}

int match(const wchar_t *pattern, const wchar_t *regexp);
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
   for (unsigned int i = *pat_pos;; i++) {
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
             match_here(part_pat, part_regexp, &part_pat_pos, &part_reg_pos);
         free(part_regexp);
         free(part_pat);
      }
      // 最初のpos は ( から.
      return OBLAKA_NOMATCH;
   }
      return OBLAKA_NOMATCH;
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

Token* oblaka_token_new() {
   // str は空
   Token *next_token = oblaka_safe_malloc(sizeof(Token));
   next_token->str = oblaka_wstring_new(L"");
   return next_token;
}

Token** oblaka_tokenize_regexp(const wchar_t *regexp) {
   Token **tokens = oblaka_safe_malloc(sizeof(Token*) * 128);
   Token *next_token = NULL;
   int i=0;
   do {
      if (next_token == NULL) {
         next_token = oblaka_token_new();
      }
      switch(*regexp) {
         case '(':
            next_token->kind = tLEFTPARENSIS;
push_token:
            regexp++;
            tokens[i++] = next_token;
            next_token = NULL;
            break;
         case ')':
            next_token->kind = tRIGHTPARENSIS;
            goto push_token;
         case '|':
            next_token->kind = tSELECT;
            goto push_token;
         case '\0':
            next_token->kind = tEOF;
            goto push_token;
         case '\\':
            // Treat the next character as local character.
            // go to default:
            regexp++;
            if (*regexp == 's') {
               // \s. 文字列クラス
               // 以下こんな感じ
            }
         default:
            {
               oblaka_wstring_append_wchar(next_token->str, *regexp);
               regexp++;
               switch (*regexp) {
                  case '(':
                  case ')':
                  case '|':
                  case '\0':
                     goto push_token;
               }
            }
            break;
      }
   } while (*regexp != '\0');
   return tokens;
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
   Token** tokens = oblaka_tokenize_regexp(regexp);
   //oblaka_get_nfa(tokens);
   oblaka_safe_free(tokens);
   return OBLAKA_NOMATCH;
}

#endif
