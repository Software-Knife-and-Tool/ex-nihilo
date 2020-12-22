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
               (cons required ()))))
         (opts (ll)
           (block ()
             (cond
              ((or (null (eq (car ll :option)) (null ll))) (() . ()))
              (:t (let ((optional ()))
                    (mu::mapl
                     (:lambda (l)
                        (let ((sym (car l))
                              (rest (cdr l)))
                          (cond
                           ((null (symbolp sym)) (raise "is not a symbol (lambda)" sym))
                           ((keywordp sym) (return (cons optional l)))
                           (:t (:letq optional (append optional (list sym)))))))
                     (cdr ll))
                    (cons optional ()))))))
         (restsym (ll)
            (cond
             ((null ll) (cons () ()))
             ((null (eq 2 (length ll))) (raise "excess elements (lambda)" ll))
             (:t
              (let* ((key (nth 0 ll))
                     (sym (nth 1 ll)))
                (cond
                 ((eq key :rest)
                  (unless (and (symbolp sym) (null keywordp key))
                    (raise "missing :rest (lambda)" key))
                  (cons :rest sym))
                 (:t (raise "missing :rest (lambda)" key))))))))

        (let* ((requireds (reqs lambda-list))
               (optionals (opts (cdr requireds)))
               (rest (restsym (cdr optionals))))
          (fmt "reqs: ~A~%opts ~A~%rest ~A~%"
               requireds optionals rest)))

    (mu::mapc
     (:lambda (e) (fmt :t "body: ~A~%" e))
     body)
  ()))
