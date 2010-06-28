#ifndef TESTEXCEPTION_H
#define TESTEXCEPTION_H

#include <exception>

class TestException : public std::exception
{
   virtual const char* what() const throw() {
      return "Test failed";
   }
};

#endif /* TESTEXCEPTION_H */
