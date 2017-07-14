// CSCI 2530
// Assignment: 7
// Author:     Brandon Gachuz
// File:       lis.cpp
// Tab stops:  4


/*
 
 This module is designed to find the best increasing sublist (bis)
 of a given list of mountains by displaying them in increasing order
 of elevation and by the order in which they were read from the book.
 
 RESTRICTIONS: The book may only hold maxMountains and the mountain
 name can only be maxChars characters long with no spaces.
 
 The module handles the following functions:
 
 Book readMountains();
 void insertMountains(Book& book, char* name, int elev);
 void updateList(int repWith, int repAt, Sublists& s);
 bool lowerElevation(int i, int j, const Book mountains);
 int needsExtension(int prefix, Book b, Sublists s);
 int head(const List l);
 List tail(List l);
 List cons(int h, List p);
 bool isEmpty(List l);
 bool isSublistsEmpty(Sublists e);
 int getCount(Book b);
 void printMountains(Book b, List t);
 Sublists BBIS(Book& b);
 void drop(List x);
 void bump(List cell);
 void setList(List& a, List b);
 void setTracing(int argc, char** argv);
 
 */

#include <cstdio>
#include <iostream>
#include <cstring> // for strcmp
#include <iomanip>
using namespace std;

// maxMountains is the limit of how big the array
// of mountains can be.

const int maxMountains = 100;

// maxChars is the limit of how long the name
// of a mountain can be.

const int maxChars = 50;

// Trace variable is false by default. It is true
// if '-t' is typed in the terminal.

bool trace = false;

// Prototype structure

struct ListCell;

// Type Definition

typedef ListCell* List;

// Function Prototypes

void bump(List cell);
void setList(List& a, List b);

/*
 
 ListCell holds the following fields: 
    - An item
    - The number of reference counts
    - A pointer to the next list cell
 
 ListCell also contains a constructor, which takes an item
 and a pointer to the next list cell. It initializes those fields
 respectively. The reference count of a cell is zero, by default.
 
 */

struct ListCell
{
    int item;
    ListCell* next;
    int refCount;
    
    ListCell(int i, ListCell* nxt)
    {
        item = i;
        next = NULL;
        setList(next, nxt);
        refCount = 0;
        bump(next);
    }
};

/*
 
 Structure Book holds two fields: 
    - The name of the mountain
    - The elevation.
 
 It initializes each aforementioned field
 in a parameterless constructor. The name is set to be empty quotes and
 the elevation is 0.

 */

struct Mountain
{
    const char* name;
    int elevation;
    
    Mountain()
    {
        name = "";
        elevation = 0;
    }
};

/*
 
 MountainsArray holds the array of mountains entered. It has
 the following fields:
 
    - The count (number of mountains in the array)
    - The array of mountains
 
 It initializes the aforementioned fields in a parameterless contructor.
 The count to be intially 0 and creates the array
 of mountains with the size of 100.
 
 */

struct MountainsArray
{
    int count;
    Mountain* mountArr;
    
    MountainsArray()
    {
        count = 0;
        mountArr = new Mountain[maxMountains];
    }
};

/*
 
 BackwardSublist holds the array of the bbis. The structor
 contains the following fields:
 
    - The length of the longest sublist
    - Creates the array of bbis
 
 BackwardSublist contains a parameterless constructor, which
 sets the length of the bbis to be initially 0 and creates the
 array of bbis's of size 100. A for-loop initializes each pointer
 in the array of bbis's to be NULL.
 
 */

struct BackwardSublist
{
    int length;
    ListCell** arr;
    
    BackwardSublist()
    {
        length = 0;
        arr = new ListCell*[maxMountains + 1];
        
        for(int i = 0; i < maxMountains + 1; ++i)
        {
            arr[i] = NULL;
        }
    }
};

// Type definitions

typedef MountainsArray* Book;
typedef BackwardSublist* Sublists;


// Function Prototypes

Book readMountains();
void insertOneMountain(Book& book, char* name, int elev);
void updateList(int repWith, int repAt, Sublists& s);
bool lowerElevation(int i, int j, const Book mountains);
int needsExtension(int prefix, Book b, Sublists s);
int head(const List l);
List tail(List l);
List cons(int h, List p);
bool isEmpty(List l);
bool isSublistsEmpty(Sublists e);
int getCount(Book b);
void printMountains(Book b, List t);
Sublists BBIS(Book& b);
void drop(List x);
void setTracing(int argc, char** argv);


int main(int argc, char** argv)
{
    setTracing(argc, argv);
    Book b = readMountains();
    Sublists bList = BBIS(b);
    if (trace)
    {
        cout << "\nThe number of mountains in the book: "
            << b->count << endl;
        cout << "Length of best sublist: "
            << bList->length << "\n" << endl;
    }
    cout << setw(28) << "Best increasing sublist:" << endl;
    printMountains(b, bList->arr[bList->length]);
    putchar('\n');
    return 0;
}

// setTracing turns on tracing throughout the program
// whenever '-t' is used in the command line.

void setTracing(int argc, char** argv)
{
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-t") == 0) {
            trace = true;
        }
    }
}

// Returns the amount of mountains stored in 'b'

int getCount(Book b)
{
    return b->count;
}

/*
 
 readMountains() reads the book of mountains
 from the standard input. First the function reads in
 a name and then an elevation.
 
 RESTRICTIONS: The mountain name must not contain spaces
 and the elevation of a mountain must be greater than 0.
 
 Sample input:
 
 Eiger            3970
 Everest          8848
 Denali           5500
 Fuji             3776
 GangkharPuensum  7570
 K2               8611
 Kilamanjaro      5895
 Matterhorn       4478
 Olympus          2917
 Tocopuri         5808
 Tupungato        6570
 Whitney          4421
 
 */

Book readMountains()
{
    Book m = new MountainsArray();
    while (!feof(stdin) && m->count < maxMountains)
    {
        int elevation;
        char* mountainName = new char[maxChars];
        cin >> mountainName;
        cin >> elevation;
        
        if (!feof(stdin) && m->count < maxMountains)
        {
            insertOneMountain(m, mountainName, elevation);
        }
    }
    return m;
}

// insertMountains(book, name, elev) inserts the mountains inputted
// from the user into the array of mountains.

void insertOneMountain(Book& book, char* name, int elev)
{
    if (strcmp(name, "") != 0 && elev > 0)
    {
        book->mountArr[book->count].name = name;
        book->mountArr[book->count].elevation = elev;
        book->count++;
    }
}

/*
 
 printMountains(b, t) prints the mountains in list 't'.
 
 Sample output:
 
 Fuji             3776
 Matterhorn       4478
 Tocopuri         5808
 Tupungato        6570
 
 */

void printMountains(Book b, List t)
{
    if (t != NULL) {
        printMountains(b, tail(t));
        cout << setw(15) << b->mountArr[t->item].name << setw(10)
        << b->mountArr[t->item].elevation << endl;
    }
}


// bump(ListCell* cell) adds 1 to the reference count
// of the cell, provided cell is not empty.

void bump(List cell)
{
    if (cell != NULL)
    {
        cell->refCount++;
    }
}

// drop(x) decreases the reference count of the cell pointed to
// by x, provided that x is not empty. If the reference count of 'x'
// becomes zero, that cell is deleted.

void drop(List x)
{
    if (x!= NULL)
    {
        x->refCount--;
        if (x->refCount == 0)
        {
            drop(x->next);
            if (trace)
            {
                cout << "Deleting cell. Reference count of: "
                << x->refCount << endl;
            }
            delete x;
        }
    }
}

// setList(&a, b) changes list variable A to equal B, while
// managing reference counts.

void setList(List& a, List b)
{
    bump(b);
    drop(a);
    a = b;
}

/*
 
 BBIS(b) computes the best increasing sublist given from book 'b'.
 
 */

Sublists BBIS(Book& b)
{
    Sublists bestList = new BackwardSublist();
    for (int i = 0; i < b->count; i++)
    {
        int extn = needsExtension(i, b, bestList);
        
        if (trace)
        {
            cout << "\nBBIS of length " << extn << " needs extending."
            << endl;
            cout << "Current best increasing sublist:" << endl;
            printMountains(b, bestList->arr[bestList->length]);
        }
        updateList(i, extn, bestList);
    }
    return bestList;
}

// lowerElevation(i, j, mountains) returns true of mountains[i]
// is less that mountains[j], false otherwise.

bool lowerElevation(int i, int j, const Book mountains)
{
    if (mountains != NULL)
    {
        return mountains->mountArr[i].elevation
            < mountains->mountArr[j].elevation;
    }
    return false;
}

/*
 
 needsExtension(index, b, s) returns the length of the BBIS
 that needs extending in 's' at index 'index'.
 
 */

int needsExtension(int index, Book b, Sublists s)
{
    int i;
    for (i = s->length;
         i != 0 && !lowerElevation(s->arr[i]->item, index, b); --i)
    {}
    return i;
}

// updateList updates the best increasing sublist 's' using int variable
// 'repWith' and 'repAt'.

void updateList(int repWith, int repAt, Sublists& s)
{
    setList(s->arr[repAt+1], cons(repWith, s->arr[repAt]));
    if (repAt+1 > s->length)
    {
        s->length++;
    }
}

// Returns the head of list 'list' in the form of an integer.

int head(const List list)
{
     return list->item;
}

// Returns the tail of List 'l'.

List tail(List l)
{
    return l->next;
}

// cons(h, p) creates and returns a new list cell.

List cons(int h, List p)
{
    return new ListCell(h, p);
}

// isEmpty() returns true if 'l' is empty, false otherwise.

bool isEmpty(List l)
{
    return l == NULL;
}

// Returns true if sublist 'e' is empty, false otherwise

bool isSublistsEmpty(Sublists e)
{
    return e == NULL;
}
