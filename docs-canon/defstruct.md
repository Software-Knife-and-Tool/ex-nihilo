### Canon *defstruct* macro

```
(defstruct struct-name slot-clauses+)
```

```
slot-clause ::= (slot-name init-form)
```

```
struct-name ::= :symbol (short enough to be made into a type keyword)
```

```
struct-name ::= :keyword
```

```
predicate
```

```
constructor
```



```
(make-struct-name slot-inits+)
```

```
(:defmac let (bindl :rest body)
  (list 'fncall
    (cons 'lambda (cons (map (:lambda (vl) (car vl)) bindl) body))
                  (cons 'list (map (:lambda (vl) (nth 1 vl)) bindl))))
```



