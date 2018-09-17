// Author: hiromi-mi <hiromi hyphen mi at cat dot zaq dot jp>
//
//正規表現エンジンもどきの何か

#include "oblaka.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

int main() {
   char pat[256], reg[256];
   wchar_t pattern[256], regexp[256];
   int i;
   setlocale(LC_ALL, "");
   printf("Pattern: ");
   if (fgets(pat, 256, stdin) == NULL) {
      return -1;
   }
   printf("Regexp: ");
   if (fgets(reg, 256, stdin) == NULL) {
      return -1;
   }
   for (i = 0; i < 256; i++) {
      if (pat[i] == '\n') {
         pat[i] = '\0';
         break;
      }
      if (i == 255) {
         while (getchar() != '\n')
            ;
      }
   }
   for (i = 0; i < 256; i++) {
      if (reg[i] == '\n') {
         reg[i] = '\0';
         break;
      }
      if (i == 255) {
         while (getchar() != '\n')
            ;
      }
   }
   mbstowcs(regexp, reg, 256);
   mbstowcs(pattern, pat, 256);

   puts((match(pattern, regexp) == 1) ? "Matched" : "Not Matched");
   return 0;
}
