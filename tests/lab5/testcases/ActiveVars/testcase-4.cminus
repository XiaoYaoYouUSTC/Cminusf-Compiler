/* this is the sample program in C- in the book "Compiler Construction" */
/* A program to perform selection sort on a 10 element array. */
int x[10];
int minloc ( int a[], int low, int high )
{ int i; int x; int k;
  k = low;
  x = a[low];
  i = low + 1;
  while (i < high)
    { if (a[i] < x)
      { x = a[i];
        k = i; }
      i = i + 1;
     }
  return k;
 }

 void sort( int a[], int low, int high)
 { int i; int k;
   i = low;
   while ( i < high-1)
   { int t;
     k = minloc(a, i, high);
     t = a[k];
     a[k] = a[i];
     a[i] = t;
     i = i + 1;
  }
  return;
 }

 void main(void)
 {  int i;
   i = 0;
   
   x[0] = 4;
   x[1] = 7;
   x[2] = 9;
   x[3] = 2;
   x[4] = 0;
   x[5] = 6;
   x[6] = 1;
   x[7] = 3;
   x[8] = 5;
   x[9] = 8;
      
   sort(x, 0, 10);
   i = 0;
   while (i < 10)
     { output(x[i]);
       i = i + 1; }
    return;
}