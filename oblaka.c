// Author: hiromi-mi <hiromi hyphen mi at cat dot zaq dot jp>
//
//正規表現エンジンもどきの何か

#define _XOPEN_SOURCE
#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>

int match_here(wchar_t *pattern, wchar_t *regexp);

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

int main() {
   char pat[256], reg[256];
   wchar_t pattern[256], regexp[256];
   int i;
   setlocale(LC_ALL, "");
   printf("Pattern: ");
   if (fgets(pat,256 ,stdin) == NULL) {
      return -1;
   }
   printf("Regexp: ");
   if (fgets(reg, 256 ,stdin) == NULL) {
      return -1;
   }
   for (i=0;i<256;i++) {
      if (pat[i] == '\n') {
	 pat[i] = '\0';
	 break;
      }
      if (i == 255) {
	 while(getchar() != '\n');
      }
   }
   for (i=0;i<256;i++) {
      if (reg[i] == '\n') {
	 reg[i] = '\0';
	 break;
      }
      if (i == 255) {
	 while(getchar() != '\n');
      }
   }
   mbstowcs(regexp, reg, 256);
   mbstowcs(pattern, pat, 256);

   puts((match(pattern, regexp) == 1) ? "Matched" : "Not Matched");
   return 0;
}
