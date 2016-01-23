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
/* Return the updated result */
inline double listInc(list<double> *myList, int pos, double value)
{
  list<double>::iterator it1;
  it1 = myList->begin();
  advance(it1,pos-1);
  
  *it1 += value;

  return *it1;
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


inline bool withinRx (double x, double y, double z, double ro_sq)
{
   if (x*x + y*y + z*z <= ro_sq) { return true; }
   else { return false; }
}

inline void getRandPos(LAB524Rand* rng, double ro, double ro_sq , double *x, double *y, double *z)
{
   double a, b, c;
   
   while (1)
   {
      a = ro*(2*rng->getUniform() - 1);
      b = ro*(2*rng->getUniform() - 1);
      c = ro*(2*rng->getUniform() - 1);

      if (withinRx(a,b,c,ro_sq)) { break; }
   }
   
   *x = a;
   *y = b;
   *z = c;
}

inline void getRandPos(LAB524Rand* rng, double ro, double ro_sq , list<double> *x, list<double> *y, list<double> *z, int pos)
{
   double a, b, c;
   
   while (1)
   {
      a = ro*(2*rng->getUniform() - 1);
      b = ro*(2*rng->getUniform() - 1);
      c = ro*(2*rng->getUniform() - 1);

      if (withinRx(a,b,c,ro_sq)) { break; }
   }
   
   listWrite(x,pos,a);
   listWrite(y,pos,b);
   listWrite(z,pos,c);
}
