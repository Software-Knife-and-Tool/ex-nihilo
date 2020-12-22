(:defsym lambda+ (:macro (lambda-list :rest body)
  (unless (listp lambda-list) (raise "lambda list is not a list" lambda-list))
  (letf ((reqs (ll)
           (block ()
             (let ((required ()))
               (mu::mapl
                (:lambda (l)
                  (let ((sym (car l))
                        (rest (cdr l)))
                    (cond
                     ((null (symbolp sym)) (raise "is not a symbol (lambda)" sym))
                     ((keywordp sym) (return (cons required l)))
                     (:t (:letq required (append required (list sym)))))))
                ll)
               (fmt :t "req: ~A~%" (cons required ()))
               (cons required ())))))

        (fmt :t "reqs: ~A~%" (reqs lambda-list)))
    (mu::mapc
     (:lambda (e) (fmt :t "body: ~A~%" e))
     body)
    ()))

(lambda+ (a b c d))     

