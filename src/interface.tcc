#ifndef SIMPLE_REPL_INTERFACE_TCC_
#define SIMPLE_REPL_INTERFACE_TCC_

/**
 * @file interface.tcc
 *
 * Template library for exporting native C and C++ functions as remote
 * procedure calls.
 *
 * For more information about (variadic) templates:
 * @li http://www.drdobbs.com/cpp/extracting-function-parameter-and-return/240000586
 * @li https://eli.thegreenplace.net/2014/variadic-templates-in-c/
 * @li https://en.cppreference.com/w/cpp/language/parameter_pack
 */
#include <iostream>

using namespace std;

typedef string String;

#include "eval.tcc"
#include "signature.tcc"
#include "tuple.tcc"


/**
 * Write the signature and documentation of a function to stdout.
 *
 * @param f Function pointer.
 * @param doc Function documentation.
 *
 * @private
 */
template <class F, class D>
void _writeDescription(F f, D doc) {
  cout << doc << "; (" << signature(f) << ")" << endl;
}


/**
 * Recursion terminator for @a _describe().
 *
 * @private
 */
inline void _describe(void) {}

/**
 * Describe a list of functions.
 *
 * We isolate the first two parameters @a f and @a doc, pass these to
 * @a _writeDescription() and make a recursive call to process the remaining
 * parameters.
 *
 * @param f Function pointer.
 * @param doc Function documentation.
 * @param args Remaining parameters.
 *
 * @private
 */
template <class F, class D, class... Args>
void _describe(F f, D doc, Args... args) {
  _writeDescription(f, doc);
  _describe(args...);
}

/**
 * Class member function.
 *
 * @private
 */
template <class U, class V, class D, class... Args>
void _describe(Tuple<U, V> t, D doc, Args... args) {
  _writeDescription(t.tail.head, doc);
  _describe(args...);
}


/**
 * Recursion terminator for @a _select().
 *
 * @private
 */
inline void _select(String) {}

/**
 * Select and call a function indexed by @a number.
 *
 * We isolate the parameter @a f and its documentation string, discarding the
 * latter. If we have arrived at the selected function (i.e., if @a depth
 * equals @a number), we call function @a f. Otherwise, we try again
 * recursively.
 *
 * @param number Function index.
 * @param depth Current index.
 * @param f Function pointer.
 * @param - Function documentation.
 * @param args Remaining parameters.
 *
 * @private
 */
template <class F, class D, class... Args>
void _select(String command, F f, D doc, Args... args) {
  if (doc == command) {
    eval(f);
    return;
  }
  _select(command, args...);
}


/**
 * REPL interface.
 *
 * This function expects parameter pairs (function pointer, documentation).
 *
 * A command is read from stdin into @a command, if the value equals @a help,
 * we describe the list of functions. Otherwise, we call the function indexed
 * by @a command.
 *
 * @param args Parameter pairs (function pointer, documentation).
 */
template <class... Args>
bool replInterface(Args... args) {
  String command;

  cout << "> ";
  cin >> command;

  if (command == "quit" || command == "exit") {
    return false;
  }
  if (command == "help") {
    _describe(args...);
    return true;
  }

  _select(command, args...);

  return true;
}

#endif
