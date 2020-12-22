(:defsym lambda (:macro (lambda-list :rest body)
  (unless (listp lambda-list) (raise "lambda list is not a list" lambda-list))
  (letf ((reqs (ll)
           (mu:block ()
             (let ((required ()))
               (mu::mapl
                (:lambda (l)
                  (let ((sym (car l))
                        (rest (cdr l)))
                    (cond
                     ((null (symbolp sym)) (raise "is not a symbol (lambda)" sym))
                     ((keywordp sym) (mu:return (cons required l)))
                     (:t (:letq required (append required (list sym)))))))
                ll)
               (cons required ()))))
         (opts (ll)
           (mu:block ()
             (cond
              ((or (null (eq (car ll) :option)) (null ll)) (cons () ll))
              (:t (let ((optional ()))
                    (mu::mapl
                     (:lambda (l)
                        (let ((sym (car l))
                              (rest (cdr l)))
                          (cond
                           ((null (symbolp sym)) (raise "is not a symbol (lambda)" sym))
                           ((keywordp sym) (mu:return (cons optional l)))
                           (:t (:letq optional (append optional (list sym)))))))
                     (cdr ll))
                    (cons optional ()))))))
         (restsym (ll)
           (cond
             ((null ll) ())
             ((null (eq 2 (length ll))) (raise "excess elements (lambda)" ll))
             (:t
              (let* ((key (nth 0 ll))
                     (sym (nth 1 ll)))
                (cond
                 ((eq key :rest)
                  (unless (keywordp key)
                    (raise "missing :rest (lambda)" key))
                  (cons :rest sym))
                 (:t (raise "missing :rest (lambda)" key))))))))

        (let* ((requireds (reqs lambda-list))
               (optionals (opts (cdr requireds)))
               (rest (restsym (cdr optionals))))
          (fmt :t "~A reqs: ~A opts ~A rest ~A body ~A~%"
               lambda-list requireds optionals rest body)))



  
  ()))

(lambda ())
(lambda (a b c) 1 2 3)

(lambda (a b c :option d e) 1 2 3)
(lambda (a b c :option e f :rest rest) 1 2 3)

(lambda (:option a b) 1 2 3)
(lambda (:option a b :rest rest) 1 2 3)

(lambda (:rest a) 1 2 3)

