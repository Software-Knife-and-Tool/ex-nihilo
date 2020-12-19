(:defsym lambda (:macro (lambda-list :rest body)
  (unless (listp lambda-list) (raise "lambda list is not a list" lambda-list))
  (mu::mapc
   (:lambda (lex)
      (fmt :t "lambda-list: ~A~%" lex)
      (unless (symbolp lex) (raise "non-symbol in lambda list" lex))
      (when (eq lex '&rest) (fmt :t "rest sym seen~%")))
   lambda-list)
  (mu::mapc
   (:lambda (e) (fmt :t "body: ~A~%" e))
   body)
  ()))
     
