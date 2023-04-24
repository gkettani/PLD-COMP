#if ! defined ( Fonction )
#define FONCTION_H

#include <string>
#include <list>
using namespace std;

class FunctionTable
{

public:

void addFunction(string name , string returnType);




protected:
list <Function> FunctionList;
};

#endif // VARIABLE_H

struct Function {
  string name;
  string returnType;
};