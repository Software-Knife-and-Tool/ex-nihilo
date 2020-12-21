(:defsym lambda (:macro (lambda-list :rest body)
  (unless (listp lambda-list) (raise "lambda list is not a list" lambda-list))
  (let ((nreqs ())
        (optionals ())
        (rest ()))
    (mu::mapl
     (:lambda (l)
       (unless (symbolp lex) (raise "non-symbol in lambda list" lex))
       (cond
        ((eq form :option) )
        ((eq form :rest)
         (fmt :t "rest sym seen~%"))))
     lambda-list)
  (mu::mapc
   (:lambda (e) (fmt :t "body: ~A~%" e))
   body)
  ()))
     
