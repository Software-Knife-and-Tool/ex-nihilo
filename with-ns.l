(:defsym with-ns (:macro (ns :rest body)
  (let ((ns-gensym (gensym)))
    (list* (list 'let (list gensym (list 'current-ns)) body)
           
          
