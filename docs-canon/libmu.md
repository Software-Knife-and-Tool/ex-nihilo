### Libmu Reference

version 0,0,15

Libmu is a C++0x14 runtime for the Logica Programming environment.

#### *API*

```
void* libmu_t()
```

returns an opaque :t pointer

```
void* libmu_nil()
```

returns an opaque :nil pointer

```
const char* libmu_version()
```

returns the library version string

```
void* libmu_eval(void* env, void* pointer)
```

 returns the evaluation of the pointer form on the env environment

```
void* libmu_read_stream(void*, void*);
```

 

```
void* libmu_read_string(void*, std::string);*
```



```
void* libmu_read_cstr(void*, const char*);
```

 

```
void  libmu_print(void*, void*, void*, bool);*
```

 

```
const char* libmu_print_cstr(void*, void*, bool);
```

 

```
void  libmu_terpri(void*, void*);
```

 

```
void  libmu_withCondition(void*, std::function<void(void*)>);
```

 

```
void* libmu_env_default(Platform*);*
```



```
void* libmu_env(Platform*, Platform::StreamId, Platform::StreamId, Platform::StreamId);
```



#### *Types*

*libmu* type symbols are keywords.

    :char       character


    :code       code

```
:cons       cons
```

```
:double     64 bit IEEE float
```

```
:except     condition
```

```
:fixnum     62 bit signed integer
```

```
:float      32 bit IEEE float
```

```
:func       lambda, native
```

```
:keyword    7 byte keyword symbol
```

```
:macro      macro function
```

```
:ns         namespace symbol bindings
```

```
:stream     file, string, socket
```

```
:struct     defstruct
```

```
:symbol     Lisp-1 binding
```

```
:string	 		:char vector
```

```
:vector     :t, :byte, :char, :fixnum, :float specialized vectors
```





  