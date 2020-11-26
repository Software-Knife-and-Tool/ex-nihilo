### *Mu*

   A Lisp-1 runtime for POSIX  environments.

- 3 bit tagged 64 bit pointers in a 64 bit address space
- 

#### The Mu Kernel Language

##### Core Addresses

Core addresses are confined to the machine heap (`rethink this`). A core address is a 52 bit virtual address that can fit in 7 bytes. `Shouldn't these be byte offsets into the heap so the heap can float?` What about foreign pointers?

##### Tagged Pointers

*mu* employs a 3 bit low tag scheme in a `uint64_t`. The upper bits of a tagged pointer are either a *core address* or immediate data.

```
 EFIX 0 even fixnum (62 bits)
 SYMB 1 symbol/keyword
 EXTI 2 extended immediate
 INDR 3 indirect
 OFIX 4 odd fixnum (62 bits)
 FUNC 5 function
 CONS 6 cons
 FLOT 7 float
```

*Fixnums* are tagged directly so that add/subtract can be performed on the pointers with no boxing overhead. 

*Indirect* pointers are the address of a *heap* object with a type header and *gc* information.

```
 CONDITION
 FFI
 MACRO
 NAMESPACE
 STREAM
 STRUCT
 VECTOR
```

*Immediates* are pointers whose values fit into a *pointer* word. *Strings* of seven or less bytes and *keywords* whose names are an immediate string are coded as an extended immediate. *Char* immediates waste a substantial part of the *pointer* word, even if they were to support *Unicode* 16 bit code points. (it may be worth coding these as *fixnums* as in the Kernel DIN proposal by Baker (see section 4 "Boolean and Character Data Types") in <http://home.pipeline.com/~hbaker1/CritLisp.html>.

```
 CHAR
 KEYWORD
 STRING
 MACROFN
```

All pointers are *uint64_t* constants or reside in an *mmap*(2)ed heap file. Heap allocations are on 8 byte boundaries.

###### System Classes

```
 CHAR
 CONDITION
 CONS
 DOUBLE
 FIXNUM
 FLOAT
 FFI
 FUNCTION
 MACRO
 NAMESPACE
 STREAM
 STRUCT
 SYMBOL
 STRING
 VECTOR
```

##### Mu Types

All data types are immutable. Symbols may not be redefined.

Boolean values are constant keyword symbols, `:t` or `:nil`.

A *list* is a *cons*, the empty list, or *:nil* (see`list?`). The empty list and `:nil` are folded to the same constant object by the `reader` and are consequently `eq`. 

A *number* is a *fixnum*, a *float*, or a *double*. 

All other types correspond directly to system classes.

*Fixnum*s are 62 bit signed integers.

*Keyword* names are limited to seven characters. Keywords bind to themselves.

*Type symbols* are keywords.

*Functions* are of two types; interpreted *lambdas*, and *mu* functions, which are platform machine code.

*Vectors* support both general *pointers* and packed *char*, *fixnum*, *float*, and *double* types. There is a maximum length of 1024 for vectors (*rethink this*).

*Streams* are file, socket, and string-based.

*Conditions* need a lot of work.

##### Special Operators

    :defcnst
    :lambda
    :quote
##### Constant Symbols

```
*standard-input*
*standard-output*
*error-output*
```

##### Macros

```
progn (:rest body)
if (test true-form false-form)
let (binds :rest body)
let* (bindl :rest body)
and (:rest forms)
or (:rest forms)
```

##### *Mu* Functions

**bound?**		 *symbol*  =>  *boolean*

```
returns `:t` if symbol is bound to a value and :nil if not.   
signals *type-error* if the argument is not a *symbol*.
```

**key?**				*object*  **=>**  *boolean*

​		returns `:t` if *object* is of type *keyword* and `:nil` if not.

**symbol?**    	*object*  **=>**  *boolean*

​		returns `:t` if *object* is a *symbol* and `:nil` otherwise.

**name-of**	*symbol*  **=>**  *string*

​		returns the *print name* of *symbol* as a *string*.

​		signals an error of `type-error` if *symbol* is not of type *symbol*.

**val-of**		*symbol*   **=>**  *object*

​		returns the value of *symbol*.

​		signals an error of `type-error` if *symbol* is not of type `symbol`

###### Conses

**car** *list*   **=>**  *object*

​		returns the first element of a non-empty list or `:nil` if the argument is `:nil`.

​        signals *type-error* if the argument is not a *list*.

**cdr** *list*  **=>**  *object*

​		returns a *list* which is the tail of the argument list or `:nil` if the argument is `:nil`.

​		signals *type-error* if the argument is not a `list`.

**cons**		 *object-1* *object-2*  **=>** *cons*

​		returns a new *cons* whose *car* is *object-1* and *cdr* is *object-2*.

**cons?**		*object*   **=>** *boolean*

​		returns `:t` if *object* is a *cons* and `:nil` if not. See `list?`.

**listlen**				*list*  **=>** *fixnum* 

​		returns the length of *list* as a *fixnum*. The empty list and `:nil` return 0.

​		signals an error of *type-error* if the argument is not a `list`.

**list?**					*object*  **=>**  *boolean*  

​		returns `:t` if *object* is of type `list` or `:nil` if not.

**listvec**				  *list*  **=>**  *vector*

​		returns a *vector* whose elements are the members of *list*

​		signals an error of `type-error`if the argument is not a `list`.

**map**				*function*  *list*  **=>**  *list*-1

​		returns a *list* constructed by mapping *function* onto *list*.

​		signals an error of *type-error* if *function* is not a *function* or *list* is not a *list*.

**nth**			*fixnum*  *list*  **=>**   *object*

​		returns the *nth* member of the *list* if *fixnum* is less than the length of *list* or `:nil` otherwise.

**nthcdr**		*fixnum*  *list*  **=>** *list-1*

​		returns a *list* composed of the *fixnum*th element to the end of the list.

​        if *fixnum* is greater than or equal to the length of *list*, return `:nil`.

###### Chars

**char?**		  *object*  **=>**  *boolean*

​		returns `:t` if *object* is of type *char* or `:nil` if not.

**charint** 	  	*char*   **=>**  *fixnum*

​		returns a *fixnum* which is the ANSI character code of *char*.   

​		signals *type-error* if the argument is not a *char*.

###### Mu

**eq**	  		*object-1* *object-2*  **=>** *boolean*

​		returns `:t` if *object-1* and *object-2* are the same object and `:nil` if not.

**error** 		*error-type* *string*

​		signals an error of *type-error* and prints *string* on the `*error-output*` stream. Does not return.

**eval**   		*object*  **=>**  *object*

​		evaluates *object* in the global environment and returns the result.

**exit**			*fixnum*

​		exits the process with return code *fixnum*. Does not return.

**forign?** 		*object*  **=>**  *boolean*

​		returns `:t` if *object* is of type *foreign* and `:nil` if not.

**typeof**				*object*  **=>**  *symbol*

​		returns the type symbol corresponding to *object*.

**null** 		*object*  **=>** *boolean*

​		returns `:t` if *object* is `:nil` or `:nil` if not.

**room**			**=>**   `:nil`

​		*room* prints heap statistics on `*standard-output*` and returns `:nil`

**specop?**    *symbol*  **=>**  *boolean*

​		returns `:t` if *symbol* names a `special operator` and *:nil* otherwise.

​		signals an error of *type-error* if the argument isn't a *symbol*.

###### Fixnums

**fixnum?** 	*object*  **=>**  *boolean*

​		returns `:t` if object is of type *fixnum* and `:nil` if not.

**fixnum- **		*fixnum-1*  *fixnum-2*  **=>**  *fixnum*

​		returns the *fixnum* difference between *fixnum-1* and *fixnum-2*.

​		signals an error of  *type-error* if either argument is not a *fixnum*.

**fixnum***  		*fixnum-1*  *fixnum-2*  **=>**  *fixnum*

​		returns the *fixnum* difference between *fixnum-1* and *fixnum-2*.

​		signals an error of  *type-error* if either argument is not a *fixnum*.

**fixnum<**  			*fixnum-1*  *fixnum-2*  **=>**  *boolean*

​		returns `:t` if *fixnum-1* is less than *fixnum-2* and `:nil` if not.

​		Signals an error of  *type-error* if either argument is not a *fixnum*.

**fixnum+**   		*fixnum-1*  *fixnum-2*  **=>**  *fixnum*

​		Returns the *fixnum* sum of *fixnum-1* and *fixnum-2*.

​		Signals an error of  *type-error* if either argument is not a *fixnum*.

**logand**     	*fixnum-1*  *fixnum-2*  **=>**  *fixnum*

​		returns the `bitwise and` of the arguments as a *fixnum*.

​		signals an error of *type-error* if either argument is not a *fixnum*.

**logor**				 *fixnum-1*  *fixnum-2*  **=>**  *fixnum*

​		returns the `bitwise or` of the arguments as a *fixnum*.

​		signals an error of *type-error* if either argument is not a *fixnum*.

###### Floats

**float-**	  	*float-1* *float-2*  **=>**  *float*

​		returns the *float* difference between *float-1* and *float-2*.

​		signals an error of  *type-error* if either argument is not a *float*.

**float*-mul**		*float-1*  *float-2*  **=>** *float*   

​		returns the *float* product of *float-1* and *float-2*.

​		Signals an error of *type-error* if either argument is not a *float*.
​		<u>Signal overflow?</u>

**float/** 		*float-1*  *float-2*  **=>**  *float*

​		returns the *float* quotient of *float-1* and *float-2*.

​		signals an error of *type-error* if either argument is not a *float*.

​		<u>Signal underflow?</u>

**float+**		*float-1*  *float-2*  **=>** *float*  

​		returns the *float* sum of *float-1* and *float-2*.

​		signals an error of  *type-error* if either argument is not a *float*.

**float<**		*float-1*  *float-2*  **=>** *float*  

​		returns the boolean of *float-1* < *float-2*.

​		signals an error of  *type-error* if either argument is not a *float*.


**float**     	*number*  **=>**  *float*

​		coerce *number* to *float* and return it.

​		Signals an error of *type-error* when the argument is not a number.

**float?**		*object*  **=>** *boolean* 

​		returns `:t` if *object* is of type *float* and `:nil` if not.

###### Functions

**functn?** 			*object*  **=>**  *boolean*

​		returns `:t` if *object* is of type *foreign* and `:nil` if not.

**fncall** 			*object*  **=>**  *boolean*

​		returns `:t` if *object* is of type *foreign* and `:nil` if not.

**tramp**		*function*  *cons*  **=>**  *object*

​		repeatedly call *function* with *cons* as arguments until *function* no longer returns a function.

​        return whatever the last call returned. 

​		this is the classic trampoline for implementing tail-recursive functions.

###### Macros

**macxpnd**   		*list*  **=>** *list*-1

​		returns the macro expansion of *list*.

​		signals an error of *type-error* if the argument is not a *list*.

**macfn**			*symbol*  **=>**  *function*

​		returns the macro expander function bound to *symbol*.

​		signals an error of *type-error* if the argument is not a symbol.

###### Printer

**prin1**			*object*  *stream*  **=>**  *object*

**princ**			*object*   *stream*  **=>** *object*

​		*princ* and *prin1* write a text representation of *object* to *stream*. 

​		*prin1* writes the *object* suitable for consumption by *read*, and *princ* by humans.

​		signals an error of *type-error* if the *stream* argument is not an output *stream* or `:nil`.

**terpri**

​		print a newline to `*standard-output*`.  <u>Seriously?</u> Get rid of this. 

**writech**				*char*  *stream* **=>**  *char*

​		writes a byte from *char* to *stream* and returns *char*.

​		signals an error of *type_error* if *char* is not of type *char* or *stream* is not a `:nil` or an output *stream*.

###### Reader

**read**				*stream*   **=>**  *object*

​		*read* converts a textual representation of an object from *stream* to an *object* and returns it.

​		signals an error of *type-error* if the *stream* argument is not an input *stream* or `:t`.

**readch**				*stream*  **=>**  *char*

​		*readch* consumes a character from *stream*, converts to it a *char* and returns it.

​		signals an error of *type-error* if the *stream* argument is not an input *stream* or `:t`.

###### Streams

**stream?**		*object*  **=>**  *boolean* 

​		returns `:t` if *object* is a *stream* and `:nil` otherwise.

**load**					*string*  **=>**   *boolean*

​		reads a series of s-expressions from the file system, where *string* is the file path name.  

​        returns `:t` if the file was successfully read, and `:nil` if not.

​		signals an error of *type-error* if the argument if the argument is not a *string* or a *stream*.

###### Strings

**string?**   		*object*  **=>**  *boolean*

​		returns `:t` if *object* is a *string* and `:nil` otherwise.

**string=**			*string-1*  *string-2*   **=>** *boolean*

​		returns `:t` if *string-1* and *string-2* are the same character-for-character or `:nil` otherwise.

​		signals an error of *type-error* if the arguments aren't both of type *string*.

**strlen**			*string*  **=>**  *fixnum*

​		returns the length of *string* as a *fixnum*.

​		signals an error of *type-error* if *string* is not of type *string*.

###### Vectors

**vector?**			*object*   **=>**  *boolean*

​		returns `:t` if *object* is of type *vector* and `:nil` otherwise.

**veclen**			*vector*  **=>**  *fixnum*

​		returns the length of *vector* as a *fixnum*.

​		signals an error of *type_error* if *vector* is not a vector.

**vecref**			*fixnum*  *vector*  **=>**   *object*

​		returns the *fixnum*th element of *vector*. <u>What happens if *fixnum* is > length?</u>

​		signals an error of *type_error* if *fixnum* is not a *fixnum* or *vector* is not a *vector*.

**vectype**			*vector*  **=>**  *symbol* 

​		returns the type of the elements in *vector* as a *symbol*.



##### Appendix A:  Runtime definitions

```
(:defunc list (:rest lists) lists)
```

```
(:defunc vector (type :rest els) (listvec type els))
```

```
(:defmac progn (:rest body)
  (list 'apply (list :fn (cons :lambda (cons :nil body))) :nil))
```

```
(:defmac lambda (:lambda :rest body)
  (list :funct (cons :lambda (cons lambda body))))
```

```
(:defmac let (bindl :rest body)
  (list 'fncall
    (cons 'lambda (cons (map (:lambda (vl) (car vl)) bindl) body))
                  (cons 'list (map (:lambda (vl) (nth 1 vl)) bindl))))
```



