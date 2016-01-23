/* 
 * Class "list" example.
 * I do not understand "iterator" and I think they are stupid.
 * So I write some wrapping functions to ease the use.
 * -----------------------------------------------------------------------------
 * Author: Ling-San Meng
 * Email: f95942117@gmail.com
 * Date: 2011/11/16
 */


#include <iostream>
#include <list>
#include <cstdio>

using namespace std;

/* Return "the pos th" element of an int list */
inline int listRead(list<int> myList, int pos)
{
  list<int>::iterator it1;
  it1 = myList.begin();
  advance(it1,pos-1);
  return *it1;
}

/* Return "the pos th" element of a double list */
inline double listRead(list<double> myList, int pos)
{
  list<double>::iterator it1;
  it1 = myList.begin();
  advance(it1,pos-1);
  return *it1;
}

/* Set "the pos th" element of an int list to "value" */
inline void listWrite(list<int> *myList, int pos, int value)
{
  if (pos > (myList->size())) { myList->push_back(value); }
  else
  {
    list<int>::iterator it1;
    it1 = myList->begin();
    advance(it1,pos-1);
    *it1 = value;
  }
}

/* Set "the pos th" element of a double list to "value" */
inline void listWrite(list<double> *myList, int pos, double value)
{
  if (pos > (myList->size())) { myList->push_back(value); }
  else
  {
    list<double>::iterator it1;
    it1 = myList->begin();
    advance(it1,pos-1);
    *it1 = value;
  }
}

/* Add an increment of "value" to "the pos th" element of a double list */
inline void listInc(list<double> *myList, int pos, double value)
{
  list<double>::iterator it1;
  it1 = myList->begin();
  advance(it1,pos-1);
  
  *it1 += value;
}

/* Remove "the pos th" element from an int list */
/* Return the element being erased */
inline int listErase(list<int>* myList, int pos)
{
  list<int>::iterator it1;
  it1 = myList->begin();
  advance(it1,pos-1);
  int value = *it1;
  myList->erase(it1);
  return value;
}

/* Remove "the pos th" element from a double list */
/* Return the element being erased */
inline double listErase(list<double>* myList, int pos)
{
  list<double>::iterator it1;
  it1 = myList->begin();
  advance(it1,pos-1);
  double value = *it1;
  myList->erase(it1);
  return value;
}

int main ()
{
  list<double> mylist;
  for (int i=1; i<=10; i++) mylist.push_back(i*1.167);


  // Before removal 
  cout << listRead(mylist,10) << endl;
  cout << mylist.size() << endl << endl;;

  listWrite(&mylist,11,1.4);
  cout << listRead(mylist,11) << endl << endl;
/*  
  // Remove the 1st element
  listErase(&mylist, 1);

  // After removal
  cout << listRead(mylist,1) << endl;
  cout << mylist.size() << endl << endl;

  // After value change
  listWrite(&mylist,1,4.4);
  cout << listRead(mylist,1) << endl << endl;

  // After value increment
  listInc(&mylist,1,-0.1);
  cout << listRead(mylist,1) << endl;
*/
  getchar();
  return 0;
}
