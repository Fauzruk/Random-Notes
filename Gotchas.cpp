There is a lot of undefined, unspecified, and implementation-defined behaviour in C++. For example, there are not necessarily 8 bits in a byte. But assuming a byte has 8 bits is not likely to cause your code to break unless you port to obscure architectures, so I'm going to omit "pedantic" examples like this. Of course I'll also include things that are perfectly well-defined but programmers often don't know about. I will omit things that affect a large number of programming languages, such as floating point inexactness. Without further ado, here's a very incomplete and possibly inaccurate list:
volatile is useless for concurrency; it offers no guarantee of atomicity.
Unsequenced writes to the same scalar variable cause undefined behaviour. So do a read and write to the same scalar variable, when unsequenced.
Evaluation of argument expressions in a function call are unsequenced (i.e., they might even overlap)
Returning a local variable by reference is always a bug, even if it's an rvalue reference.
Declaring a static data member inline doesn't define it, even if it has an initializer.
Any declaration of a member function hides all declarations of members of base classes with the same names.
Unqualified lookup for a nondependent name will not find members of dependent base classes.
Types nested within dependent classes need to be introduced with the typename keyword: typename Foo<T>::Bar.
Templates nested within dependent classes need to be prefixed by template: Foo<T>::template Bar<U>.
Variables first declared at namespace that are const or constexpr (and not volatile) have internal linkage (rather than external).
The order of initialization of non-local static variables in different translation units is unspecified.
If you explicitly call the destructor for a local variable, it's still going to be called again at the end of its scope, which usually results in undefined behaviour.
Only variables of type volatile std::sig_atomic_t and lock-free atomic variables may be safely accessed inside signal handlers. Only a small set of C library functions may be safely called inside signal handlers. Note that printf is not one of them!
If a variable is allocated using new and deallocated using free, the behaviour is undefined. Same goes if the variable is allocated using malloc and deallocated using delete. A pointer to an array allocated with new[] must be deallocated with delete[], or the behaviour is undefined.
If a dynamically allocated object is deleted through a pointer to a base class of the object type, the class must have a virtual destructor, or the behaviour is undefined.
Dynamic memory allocation with new may fail by throwing std::bad_alloc (most programmers forget to account for the fact that it might fail).
wchar_t may not be 16 bits long, and may not be large enough to hold arbitrary Unicode code points.
It's normally undefined behaviour to access an object using a pointer to an unrelated type, unless it's char or unsigned char. (strict aliasing) Also, doing so generally involves making non-portable assumptions about the representations of types in memory, anyway.
The four categories of pointers may all have different sizes: object pointers, function pointers, pointers to data members, and pointers to function members.
Most vexing parse
decltype(x) and decltype((x)) may have different meanings. In C++14, beware of using decltype(auto) to deduce function return types; return x; and return (x); may deduce different types.
auto and decltype do not necessarily deduce types in the same way.
Lambda expressions are not true closures in that they do not extend the lifetimes of objects captured by reference.
Every lambda expression in a program has a different type, even two occurrences of [](){}.
Shifts have lower precedence than addition and subtraction.
Before C++11, two closing template angle brackets had to be separated by whitespace.
If you call a virtual function inside a constructor or destructor, it behaves as though it's non-virtual: that is, dispatch always occurs on the type to which the constructor or destructor belongs and not the type of the complete object under construction or destruction.
If an exception leaks out of a destructor during stack unwinding, the program terminates abnormally (i.e. std::terminate is called)
sizeof when applied to a pointer always gives the size of the pointer type, even if the pointer points to an array; it doesn't give the size of the array.
Top-level const qualifiers on function parameters don't affect the function signature; a function with declaration void f(const int x); matches a definition that starts with void f(int x) { ...
Top-level array function parameters are silently rewritten to be pointers. In void f(int a[5]), sizeof(a) will always be sizeof(int*) and not sizeof(int[5]).
Attempting to modify a string literal causes undefined behaviour. (In fact, from C++11 onward, technically code like char* p = "Hello, world!" is ill-formed, but a lot of compilers still allow it. To get a char* you would need a const_cast.)
Shifting by an amount equal to or greater than the number of bits in the operand causes undefined behaviour.
The ternary conditional operator won't find a common base class of its second and third arguments.
A braced-init-list is not an expression and is only allowed in contexts where it is immediately used for initialization; for example, std::vector<int> v = foo ? {1, 2} : {3, 4}; is illegal.
The controlling expression for a switch statement can't be a string; it has to of integral or enumeration type or a class type that can be implicitly converted to integral or enumeration type.
If control flows off the end of a non-void-returning function (i.e., the function is not exited with return, throw, longjmp, or call to a function that terminates the thread or program) then the behaviour is undefined. The function does not necessarily return a value-initialized object.
A goto statement can't skip nontrivial initialization going forward, nor can it jump into or out of a function.
register often doesn't do anything in modern compilers.
inline often doesn't change whether or not a function is actually inlined, but allows the function to be defined in multiple translation units.
Partially specialized friends are not allowed.
The name of a reference is always an lvalue even if the reference is an rvalue reference.
A parameter declared to have type T&& where T is a template parameter is allowed to bind to lvalues; in this case T will be deduced as an lvalue reference type (see Universal References in C++11-Scott Meyers)
C99 variable-length arrays are not supported in C++ (if your compiler allows it, it's a non-portable extension)
Copy-initialization of a class does not call an assignment operator; an initialization is not an assignment.
If a virtual function has default arguments, the default argument values used when the virtual function is called are always those of the static type, not the dynamic type.
When a derived class object is passed by value or stored in an array of base class, the derived class object is "sliced" and becomes a base class object, losing its "derivedness".
In general, if T is a class template, B is a base class, and D is a derived class, then T<D> is not automatically convertible to T<B>.
When a function returns a local variable by value, the compiler is allowed to elide the copy from the local variable into the function's return value, by directly using the function's return value whenever the local variable is used inside the function. This is a form of return value optimization called "named return value optimization".
When a function returns by value, and the value is copied or moved into a variable of the same type, the compiler is allowed to elide the copy from the function's return value into the object it is used to initialize, by directly initializing the destination object in the return statement rather than constructing a temporary first and then copying or moving into the destination object. This is also called return value optimization, and may be combined with named return value optimization.
Return value optimziation is an exception to the "as-if" rule; the compiler is allowed to perform it even if this changes the program's observable behaviour (e.g., because the copy constructor has side effects)
A destructor needs a definition even when it is pure virtual. A pure virtual function can be called by qualifying it with the name of the class of which it is a member.
If two non-static data members have different visibilities within a class, their relative order in the class layout is unspecified.
Private member functions are not invisible in C++ like they are in Java: they still participate in overload resolution even when they are inaccessible, and they may be overridden.
Pure virtual member functions can't be implemented "sideways"; if A declares a pure virtual member function f and B implements a virtual member function f with the same signature, a class D that derives from A and B does not contain an implementation of f. Instead the program is ill-formed.
Access control for members is checked statically; if B has a public virtual function that is overridden by a private function in a derived class D, then the latter may be called unconditionally through a B*.
Private members of an object may be accessed by member functions of another object of the same type; access control is per-class, not per-object.
Friendship is neither inherited nor transitive.
When a member function of a derived class D attempts to access a non-static protected member of its base class B, the object expression used to access the member must have cv-unqualified D or a class derived from D.
A constructor can't be directly called by using its name (well, the standard says constructors don't have names; never mind that, it's just a matter of wording). To force a constructor to be called over a block of memory, use placement new.
In C++11 and later, destructors are noexcept(true) by default (unless they call functions that are allowed to throw) even if they contain throw expressions.
Non-virtual base classes are initialized in declaration order, regardless of the order in which they appear in a ctor-initializer.
Non-static data members are initialized in declaration order, regardless of the order in which they appear in a ctor-initializer; and they are initialized after base classes.
A mem-initializer overrides a brace-or-equal-initializer.
Base classes and non-static members are always initialized before the first statement in the constructor's body, even when they are not mentioned in either the ctor-initializer or a brace-or-equal-initializer; in that case they are default-initialized. Assignment to a non-static member in the constructor body is not initialization even when it occurs on the first line. Therefore, references and class types without default constructors must be initialized in the ctor-initializer or a brace-or-equal-initializer; you can't postpone their initialization until the constructor body.
Two function declarations are considered equivalent if they only differ in their default arguments.
Overload resolution is really complicated.
Overloaded assignment operators, function call operators, subscripting operators, and -> operators must be member functions.
The preprocessor doesn't know that, e.g., pair<int, int> should be treated as a single argument to a preprocessor macro, rather than two arguments, pair<int and int>. Use parentheses to prevent the preprocessor from breaking up a group of tokens.
Non-dependent names are bound at the point of declaration; dependent names are bound at the point of instantiation. (This is called "two-phase lookup").
Member function templates shall not be virtual, although member functions of class templates are allowed to be virtual.
An explicit specialization declaration has to start with template<>.
Most compilers won't allow you to refer to a template defined in another translation unit unless you're referring to a specialization of the template that was instantiated in the other translation unit. This is why the standard library containers are implemented almost completely in headers.
Function templates can't be partially specialized; just overloaded.
If an exception is caught as, say, e, the statement throw e; creates a new exception object by copying e. To rethrow e, the correct code is simply throw;
Catching an exception does not invoke user-defined conversions.
catch blocks are tried in order of appearance; the "best match" is not necessarily selected.
An exception caught by the function-try-block of a constructor or destructor will be inevitably rethrown; the exception cannot be "swallowed". This is because the failure of construction or destruction is a circumstance that cannot be handled locally.
The preprocessor operates on tokens, not strings. If we have #define wstr(s) w#s, then wstr(foo) will expand to something like w "foo" (two tokens) rather than the wide-string literal w"foo", which is a single token.
A moved-from container is not necessarily empty (but can be explicitly cleared).
std::type_info::name() does not necessarily return the actual name of a type. (For example, it might return "I" for int.)
std::make_pair and std::make_tuple always decay their arguments; if a pair or tuple is required containing (say) a reference, the std::pair or std::tuple template must be explicitly instantiated.
A std::unique_ptr that points to an array allocated with new T[n] must have the array type T[] as its template argument and not T itself, so it knows to call delete[]. (NB: std::vector is usually a better choice here.)
std::shared_ptr can only be used with arrays when a custom deleter is provided; the default deleter always calls delete and not delete[].
Inserting an element into a vector may invalidate iterators and references into the vector.
Deques are not guaranteed to store their elements contiguously in memory.
iterator and const_iterator may be the same type or different types for std::set, std::multiset, std::unordered_set, and std::unordered_multiset. It's unspecified.
The ordering predicate for an ordered associative container must be a strict weak ordering; otherwise the result is undefined behaviour.
std::vector<bool> is a strange type similar to a dynamic bitset and in general you can't obtain references to its elements. (This is widely regarded as a mistake.)
The std::stack, std::queue, and std::priority_queue container adaptors lack clear functions. Also, their pop functions return void, not the object popped.
The std::unique function doesn't remove any objects from containers.
A member operator< for use with ordered associative containers or standard library algorithms such as std::sort must be const-qualified and must take its argument by value or const reference but not non-const reference. If you use a non-member operator<, it must take both arguments by value or const reference but not non-const reference.
The gets function should basically never be used as it is impossible to protect against buffer overflow. Likewise, when reading a string with scanf an explicit limit on the number of characters to be stored should be specified.
Testing a stream for end-of-file doesn't return true immediately after the last character of the stream has been read. You have to attempt to read beyond the end of file before the end-of-file flag will be set.
Attempting to print out a volatile pointer or a pointer to member by inserting it into a stream will cause it to be converted to bool rather than printing out the numeric address.
Reading a character using operator>> will not read a whitespace character; this is in contrast to the behaviour of the %c format specifier for scanf, which always reads the next character regardless of what it is.
std::endl is an overloaded function, not an object.
iostreams tend to be slow unless you set ios::sync_with_stdio(false).
In C++03, file streams don't have constructors that accept std::string filenames.

Holy shitfuck this guy wrote everything credits to him he is awesome:
http://www.quora.com/Brian-Bi
