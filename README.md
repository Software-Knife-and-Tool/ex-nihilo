# Gyre

### Do not use this, it's broken to hell and gone and is here for reference only, and will soon vanish

### The Gyre Programming Environment
is a single address space system that targets x86_64 and Aarch64 POSIX[1] kernels. The *libmu* kernel is written in C++14, compiled with Clang (or gcc), and implements a kernel language that supports Lisp-style data types in a garbage collected environment.

The implementation language, *mu*, is an immutable[0] namespaced *Lisp-1* that supports lexical bindings, macros, continuations[2], and closures. The *libmu* library uses 64 bit tagged data pointers and can accommodate an address space up to 61 bits[3].

The programming environment is implemented by several libraries; *mu* which provides a porting layer for *core*, which provides a Scheme/Lisp layer for applications. The *gyre* language provides syntax and functions familiar to the traditional Lisp programmer. *tools* provides a native code compiler and optimizer based on the *libmu* runtime.

The gyre and *mu* repls can be used as a shell and the *libmu* library extends an API for  embedded applications. While highly influenced by ANSI Common Lisp, *gyre* borrows heavily from Scheme, Clojure, and the DIN Kernel Lisp specification as critiqued by Baker.

http://www.pipeline.com/~hbaker1/CritLisp.html

#### Notes
Gyre is a work in progress and currently implements some 200+ functions, macros, and special operators.

### Prerequisites

    - clang++ (LLVM 7.0.0 or better), recent gcc
    - cmake (for unit tests)
    - clang-extra-tools (for linting)

```
[0] lexical variables are mutable through the :letq special form
[1] Linux, Mac OSX, and Windows Subsystem for Linux. Rump anykernel and sel4 support planned
[2] Real Soon Now, I Promise
[3] Current system architectures limit addresses to 48 bits, soon to be extended to 52.
```
