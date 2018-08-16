// include guard
#pragma once

#ifndef OBLAKA_OBLAKA_H
#define OBLAKA_OBLAKA_H

#define _XOPEN_SOURCE
#include <locale.h>
#include <wchar.h>
#include <stdlib.h>

int match(wchar_t *pattern, wchar_t *regexp);
int match_here(wchar_t *pattern, wchar_t *regexp);
int match_str(wchar_t c, wchar_t *pattern, wchar_t *regexp);

int match_str(wchar_t c, wchar_t *pattern, wchar_t *regexp) {
   while (*pattern == c) {
      pattern++;
   }
   return match_here(pattern, regexp);
}

int match_here(wchar_t *pattern, wchar_t *regexp) {
   if (*regexp == '\0' || *regexp == '$') {
      return 1;
   }
   if (*pattern == '\0') {
      return 0;
   }
   if (regexp[1] == '*') {
      return match_str(regexp[0], pattern, regexp+2);
   }
   // マッチ
   if (*pattern == *regexp) {
      pattern++;
      regexp++;
      return match_here(pattern, regexp);
   }
   return 0;
}

int match(wchar_t *pattern, wchar_t *regexp) {
   if (*regexp == '^') {
      return match_here(pattern, regexp+1);
   }
   do {
      if (match_here(pattern, regexp) == 1) {
	 return 1;
      }
   } while ( *pattern++ != '\0');
   return 0;
}


#endif
