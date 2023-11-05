/*
 * No changes are allowed in this file
 */
// char arr[5000];
int fib(int n) {
  if(n<2) return n;
  else return fib(n-1)+fib(n-2);
}

int _start() {
	int val = fib(30);
  // arr[1000]='a';
  // arr[4999]='b';
	return val;
}
