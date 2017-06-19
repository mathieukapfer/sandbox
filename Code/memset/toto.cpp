#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
  int tab[200][20];

  for (int i=0; i<200; i++) {
    for (int j=0; j<20; i++) {
      tab[i][j] = i + j;
    }
  }

  for (int i=0; i<200; i++) {
    printf("%x", tab[i]);
  }

  printf("\ntab:%x, &tab:%x\n", tab, &tab);

  //memset(tab, 0, sizeof(tab));
  memset(tab, 0, 200 * sizeof(int));
  memset(&tab, 0, 200 * sizeof(int));

  for (int i=0; i<200; i++) {
    printf("%x", tab[i]);
  }

  printf("\n");

  return 0;
}
