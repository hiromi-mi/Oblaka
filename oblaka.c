// Author: hiromi-mi
//
//正規表現エンジンもどきの何か

#include <stdio.h>

int match_here(char *pattern, char *regexp);

int match_str(char c, char *pattern, char *regexp) {
   while (*pattern == c) {
      pattern++;
   }
   return match_here(pattern, regexp);
}

int match_here(char *pattern, char *regexp) {
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

int match(char *pattern, char *regexp) {
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
   char pattern[256], regexp[256];
   int i;
   printf("Pattern: ");
   if (fgets(pattern,256 ,stdin) == NULL) {
      return -1;
   }
   printf("Regexp: ");
   if (fgets(regexp, 256 ,stdin) == NULL) {
      return -1;
   }
   for (i=0;i<256;i++) {
      if (pattern[i] == '\n') {
	 pattern[i] = '\0';
	 break;
      }
      if (i == 255) {
	 while(getchar() != '\n');
      }
   }
   for (i=0;i<256;i++) {
      if (regexp[i] == '\n') {
	 regexp[i] = '\0';
	 break;
      }
      if (i == 255) {
	 while(getchar() != '\n');
      }
   }

   puts((match(pattern, regexp) == 1) ? "Matched" : "Not Matched");
   return 0;
}
