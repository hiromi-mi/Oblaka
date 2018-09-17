#include "oblaka.h"
#include <assert.h>
#include <locale.h>

int main() {
   setlocale(LC_ALL, "");
   assert(match(L"12345", L"12") == 1);
   assert(match(L"12345", L"^12") == 1);
   assert(match(L"12345", L"^23") == 0);
   assert(match(L"日本語", L"日*") == 1);
   assert(match(L"日本語", L"語$") == 1);
   assert(match(L"日本語", L"^語") == 0);
   assert(match(L"日本語語", L"語*") == 1);
   return 0;
}
