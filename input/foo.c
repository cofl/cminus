#include <stdio.h>

void goo();

int sum100()
{
  int i, s;

  s = 0;
  for (i=1; i<=100; i++)
    s += i;
  return s;  
}

int sumI()
{
  static int up=10;
  int i, s;

  s = 0;
  for (i=1; i<=up; i++)
    s += i;
  up += 10;
  return s;
}

int foo()
{
  unsigned long addr = (unsigned  long) goo;
  printf("input the follow two integers: %d %d\n", (int)addr, (int)(addr>>32)); 
  return 0;
}

void goo()
{
  printf("You code is hacked\n");
}
