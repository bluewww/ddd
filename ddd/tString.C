// This may look like C code, but it is really -*- C++ -*-
 
/*
 A test file for strings
*/

#include "strclass.h"
#include "assert.h"

#include <stream.h>  // see note below on `dec(20)' about why this will go away
#include <std.h>

// can't nicely echo assertions because they contain quotes

#define tassert(ex) {if (!(ex)) \
                     { cerr << "failed assertion at " << __LINE__ << "\n"; \
                       abort(); } }

  string X = "Hello";
  string Y = "world";
  string N = "123";
  string c;
  char*  s = ",";
  regex  r = "e[a-z]*o";

void decltest()
{
  string x;
  cout << "an empty string:" << x << "\n";
  assert(x.OK());
  assert(x == "");

  string y = "Hello";
  cout << "A string initialized to Hello:" << y << "\n";
  assert(y.OK());
  assert(y == "Hello");

  if (y[y.length()-1] == 'o')
	y = y + '\n';
  assert(y == "Hello\n");
  y = "Hello";

  string a = y;
  cout << "A string initialized to previous string:" << a << "\n";
  assert(a.OK());
  assert(a == "Hello");
  assert(a == y);

  string b (a.at(1, 2));
  cout << "A string initialized to previous string.at(1, 2):" << b << "\n";
  assert(b.OK());
  assert(b == "el");

  char ch = '@';
  string z(ch); 
  cout << "A string initialized to @:" << z << "\n";
  assert(z.OK());
  assert(z == "@");

  // XXX: `dec' is obsolete.  Since string.h includes iostream.h, and not
  //      stream.h, we include stream.h in this file for the time being.  This
  //      test will be rewritten to be done the "right" way, but for now, let's
  //      save some time and go the easy route.
  string n = dec(20);
  cout << "A string initialized to dec(20):" << n << "\n";
  assert(n.OK());
  assert(n == "20");

  int i = atoi(n);
  double f = atof(n);
  cout << "n = " << n << " atoi(n) = " << i << " atof(n) = " << f << "\n";
  assert(i == 20);
  assert(f == 20);

  assert(X.OK());
  assert(Y.OK());
  assert(x.OK());
  assert(y.OK());
  assert(z.OK());
  assert(n.OK());
  assert(r.OK());
}

void cattest()
{
  string x = X;
  string y = Y;
  string z = x + y;
  cout << "z = x + y = " << z << "\n";
  assert(x.OK());
  assert(y.OK());
  assert(z.OK());
  assert(z == "Helloworld");

  x += y;
  cout << "x += y; x = " << x << "\n";
  assert(x.OK());
  assert(y.OK());
  assert(x == "Helloworld");

  y = Y;
  x = X;
  y.prepend(x);
  cout << "y.prepend(x); y = " << y << "\n";
  assert(y == "Helloworld");

#if 0
  y = Y;
  x = X;
  cat(x, y, x, x);
  cout << "cat(x, y, x, x); x = " << x << "\n";
  assert(x == "HelloworldHello");

  y = Y;
  x = X;
  cat(y, x, x, x);
  cout << "cat(y, x, x, x); x = " << x << "\n";
  assert(x == "worldHelloHello");
#endif

  x = X;
  y = Y;
  z = x + s + ' ' + y.at("w") + y.after("w") + ".";
  cout << "z = x + s +  + y.at(w) + y.after(w) + . = " << z << "\n";
  assert(z.OK());
  assert(z == "Hello, world.");

}

void comparetest()
{  
  string x = X;
  string y = Y;
  string n = N;
  string z = x + y;

  assert(x != y);
  assert(x == "Hello");
  assert(x != z.at(0, 4));
  assert (x < y);
  assert(!(x >= z.at(0, 6)));
  assert(x.contains("He"));
  assert (z.contains(x));
  assert(x.contains(r));

  assert(!(x.matches(r)));
  assert(x.matches(rxalpha));
  assert(!(n.matches(rxalpha)));
  assert(n.matches(rxint));
  assert(n.matches(rxdouble));

  assert(x.index("lo") == 3);
  assert(x.index("l", 2) == 2);
  assert(x.index("l", -1) == 3);
  assert(x.index(r)  == 1);
  assert(x.index(r, -2) == 1);

  assert(x.contains("el", 1));
  assert(x.contains("el"));

  assert(common_prefix(x, "Help") == "Hel");
  assert(common_suffix(x, "to") == "o");

  assert(fcompare(x, "hELlo") == 0);
  assert(fcompare(x, "hElp") < 0);
}

void substrtest()
{
  string x = X;

  char ch = x[0];
  cout << "ch = x[0] = " << ch << "\n";
  assert(ch == 'H');

  string z = x.at(2, 3);
  cout << "z = x.at(2, 3) = " << z << "\n";
  assert(z.OK());
  assert(z == "llo");

  x.at(2, 2) = "r";
  cout << "x.at(2, 2) = r; x = " << x << "\n";
  assert(x.OK());
  assert(x.at(2,2).OK());
  assert(x == "Hero");

  x = X;
  x.at(0, 1) = "j";
  cout << "x.at(0, 1) = j; x = " << x << "\n";
  assert(x.OK());
  assert(x == "jello");

  x = X;
  x.at("He") = "je";
  cout << "x.at(He) = je; x = " << x << "\n";
  assert(x.OK());
  assert(x == "jello");

  x = X;
  x.at("l", -1) = "i";
  cout << "x.at(l, -1) = i; x = " << x << "\n";
  assert(x.OK());
  assert(x == "Helio");
  
  x = X;
  z = x.at(r);
  cout << "z = x.at(r) = " << z << "\n";
  assert(z.OK());
  assert(z == "ello");

  z = x.before("o");
  cout << "z = x.before(o) = " << z << "\n";
  assert(z.OK());
  assert(z == "Hell");

  x.before("ll") = "Bri";
  cout << "x.before(ll) = Bri; x = " << x << "\n";
  assert(x.OK());
  assert(x == "Brillo");

  x = X;
  z = x.before(2);
  cout << "z = x.before(2) = " << z << "\n";
  assert(z.OK());
  assert(z == "He");

  z = x.after("Hel");
  cout << "z = x.after(Hel) = " << z << "\n";
  assert(z.OK());
  assert(z == "lo");

  x.after("Hel") = "p";  
  cout << "x.after(Hel) = p; x = " << x << "\n";
  assert(x.OK());
  assert(x == "Help");

  x = X;
  z = x.after(3);
  cout << "z = x.after(3) = " << z << "\n";
  assert(z.OK());
  assert(z == "o");

  z = "  a bc";
  z  = z.after(rxwhite);
  cout << "z =   a bc; z = z.after(rxwhite); z =" << z << "\n";
  assert(z.OK());
  assert(z == "a bc");
}


void utiltest()
{
  string x = X;
  int matches = x.gsub("l", "ll");
  cout << "x.gsub(l, ll); x = " << x << "\n";
  assert(x.OK());
  assert(matches == 2);
  assert(x == "Hellllo");

  x = X;
  assert(x.OK());
  matches = x.gsub(r, "ello should have been replaced by this string");
  assert(x.OK());
  cout << "x.gsub(r, ...); x = " << x << "\n";
  assert(x.OK());
  assert(matches == 1);
  assert(x == "Hello should have been replaced by this string");

  matches = x.gsub(rxwhite, "#");
  cout << "x.gsub(rxwhite, #); x = " << x << "\n";
  assert(matches == 7);
  assert(x.OK());

  string z = X + Y;
  z.del("loworl");
  cout << "z = x+y; z.del(loworl); z = " << z << "\n";
  assert(z.OK());
  assert(z == "Held");

  x = X;
  z = reverse(x);
  cout << "reverse(x) = " << z << "\n";
  assert(z.OK());
  assert(z == "olleH");

  x.reverse();
  cout << "x.reverse() = " << x << "\n";
  assert(x.OK());
  assert(x == z);

  x = X;
  z = upcase(x);
  cout << "upcase(x) = " << z << "\n";
  assert(z.OK());
  assert(z == "HELLO");

  z = downcase(x);
  cout << "downcase(x) = " << z << "\n";
  assert(z.OK());
  assert(z == "hello");

  z = capitalize(x);
  cout << "capitalize(x) = " << z << "\n";
  assert(z.OK());
  assert(z == "Hello");

  /* Let's see how apostrophe is handled. */
  z = "he asked:'this is nathan's book?'. 'no, it's not',i said.";
  cout << "capitalize(z) = " << capitalize (z) << "\n";
  
  z = replicate('*', 10);
  cout << "z = replicate(*, 10) = " << z << "\n";
  assert(z.OK());
  assert(z == "**********");
  assert(z.length() == 10);
}

void splittest()
{
  string z = "This string\thas\nfive words";
  cout << "z = " << z << "\n";
  string w[10];
  int nw = split(z, w, 10, rxwhite);
  assert(nw == 5);
  cout << "from split(z, rxwhite, w, 10), n words = " << nw << ":\n";
  for (int i = 0; i < nw; ++i)
  {
    assert(w[i].OK());
    cout << w[i] << "\n";
  }
  assert(w[0] == "This");
  assert(w[1] == "string");
  assert(w[2] == "has");
  assert(w[3] == "five");
  assert(w[4] == "words");
  assert(w[5] == (char*)0);

  z = join(w, nw, "/");
  cout << "z = join(w, nw, /); z =" << z << "\n";
  assert(z.OK());
  assert(z == "This/string/has/five/words");
}


void iotest()
{
  string z;
  cout << "enter a word:";
  cin >> z;
  cout << "word =" << z << " ";
  cout << "length = " << z.length() << "\n";
}

void identitytest(string a, string b)
{
  string x = a;
  string y = b;
  x += b;
  y.prepend(a);
  assert((a + b) == x);
  assert((a + b) == y);
  assert(x == y);
  assert(x.after(a) == b);
  assert(x.before(b, -1) == a);
  assert(x.from(a) == x);
  assert(x.through(b, -1) == x);
  assert(x.at(a) == a);
  assert(x.at(b) == b);

  assert(reverse(x) == reverse(b) + reverse(a));
  
  assert((a + b + a) == (a + (b + a)));

  x.del(b, -1);
  assert(x == a);

  y.before(b, -1) = b;
  assert(y == (b + b));
  y.at(b) = a;
  assert(y == (a + b));

  x = a + reverse(a);
  for (int i = 0; i < 7; ++i)
  {
    y = x;
    x += x;
    assert(x.OK());
    assert(x == reverse(x));
    assert(x.index(y) == 0);
    assert(x.index(y, -1) == int(x.length() / 2));
  }
}

void freqtest()
{
  string x = "Hello World";
  subString y = x.at(0,5);
  assert(x.freq('l') == 3);	// char
  assert(x.freq("lo") == 1);	// char*
  assert(x.freq(x) == 1);	// string
  assert(x.freq(y) == 1);	// Substring
}

int main()
{
  decltest();
  cattest();
  comparetest();
  substrtest();
  utiltest();
  splittest();
  freqtest();
  identitytest(X, X);
  identitytest(X, Y);
  identitytest(X+Y+N+X+Y+N, "A string that will be used in identitytest but is otherwise just another useless string.");
  iotest();
  cout << "\nEnd of test\n";
  return 0;
}
