(:defsym lambda (:macro (lambda-list :rest body)
  (unless (listp lambda-list) (raise "lambda list is not a list" lambda-list))
  (letf ((reqs (ll)
           (mu:block ()
             (let ((required ()))
               (mu::mapl
                (:lambda (l)
                  (let ((sym (car l)) (rest (cdr l)))
                     (when (keywordp sym) (mu:return (cons required l)))
                     (unless (symbolp sym) (raise "is not a symbol (lambda)" sym))
                     (:letq required (append required (list sym)))))
                ll)
               (cons required ()))))
         (opts (ll)
           (mu:block ()
             (cond
              ((or (null (eq (car ll) :option)) (null ll)) (cons () ll))
              (:t (let ((optional ()))
                    (mu::mapl
                     (:lambda (l)
                        (let ((term (car l)) (rest (cdr l)))
                          (when (keywordp term) (mu:return (cons optional l)))
                          (unless (or (consp term) (symbolp term))
                            (raise "is not an opt symbol or initializer (lambda)" term))
                          (cond
                           ((consp term)
                            (unless (eq 2 (length term))
                              (raise "is not a legal opt term (lambda)" term))
                            (let ((sym (nth 0 term)) (value (nth 1 term)))
                              (unless (and (symbolp sym) (null (keywordp sym)))
                                (raise "is not a legal opt term (lambda)" term))
                              (:letq optional (append optional (list term)))))
                           ((symbolp term)
                            (when (keywordp term)
                              (raise "is not a legal opt term (lambda)" term))
                            (:letq optional (append optional (list (list term ()))))))))
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
(lambda (a b c :option d (e 1)) 1 2 3)

(lambda (a b c :option e f :rest rest) 1 2 3)
(lambda (a b c :option e (f 5) :rest rest) 1 2 3)

(lambda (:option a b) 1 2 3)
(lambda (:option a (b 1)) 1 2 3)
(lambda (:option a b :rest rest) 1 2 3)
(lambda (:option a (b 1) :rest rest) 1 2 3)

(lambda (:rest a) 1 2 3)

