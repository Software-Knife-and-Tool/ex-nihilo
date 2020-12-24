#|
    A lambda-list has four parts, any or all of which may be empty:

    Specifiers for the required parameters.
        These are all the parameter specifiers up to the first lambda-list keyword;
           if there is no such lambda-list keyword,
           then all the specifiers are for required parameters.
    Specifiers for optional parameters.
        If the lambda-list keyword :option is present,
           the optional parameter specifiers are those following the
           lambda-list keyword &optional up to the next lambda-list keyword
           or the end of the list.
    A specifier for a rest parameter.
        The lambda-list keyword :rest, if present,
           must be followed  by a single rest parameter specifier,
           which in turn must be followed by another lambda-list keyword
           or the end of the lambda-list.
    Specifiers for keyword parameters.
        If the lambda-list keyword :key is present,
           all specifiers up to the next lambda-list keyword
           or the end of the list are keyword parameter specifiers.
|#

(:defsym parse-lambda (:lambda (lambda-list)
  (unless (listp lambda-list) (raise "lambda list is not a list" lambda-list))
  (letf* ((inits (llist)
           (let ((terms ()))
             (mu:block ()
               (mu::mapl
                (:lambda (l)
                  (let ((term (car l)) (rest (cdr l)))
                    (when (keywordp term) (mu:return (cons terms l)))
                    (unless (or (consp term) (symbolp term))
                      (raise "is not an opt symbol or initializer (lambda)" term))
                    (cond
                     ((consp term)
                      (unless (eq 2 (length term))
                        (raise "is not a legal term (lambda)" term))
                      (let ((sym (nth 0 term)) (value (nth 1 term)))
                        (unless (and (symbolp sym) (null (keywordp sym)))
                          (raise "is not a legal term (lambda)" term))
                        (:letq terms (append terms (list term)))))
                     ((symbolp term)
                      (when (keywordp term)
                        (raise "is not a legal term (lambda)" term))
                      (:letq terms (append terms (list (list term ()))))))))
                (cdr llist))
               (cons terms ()))))
          (reqs (llist)
           (mu:block ()
             (let ((required ()))
               (mu::mapl
                (:lambda (l)
                  (let ((sym (car l)) (rest (cdr l)))
                     (when (keywordp sym) (mu:return (cons required l)))
                     (unless (symbolp sym) (raise "is not a symbol (lambda)" sym))
                     (:letq required (append required (list sym)))))
                llist)
               (cons required ()))))
          (opts (llist)   
            (cond
             ((or (null (eq (car llist) :option)) (null llist)) (cons () llist))
             (:t (inits llist))))
          (keys (llist)   
            (cond
             ((or (null (eq (car llist) :key)) (null llist)) (cons () llist))
             (:t (inits llist))))
          (rest (llist)
             (cond
              ((or (null (eq (car llist) :rest)) (null llist)) (cons () llist))
              (:t (unless (eq 2 (length llist))
                    (raise "excess elements (lambda)" llist))
                  (let ((sym (nth 1 llist)))
                    (unless (and (symbolp sym) (null (keywordp sym)))
                      (raise "illegal rest clause (lambda)" llist))
                    (cons :rest sym))))))

         (let* ((requireds (reqs lambda-list))
                (optionals (opts (cdr requireds)))
                (keys (keys (cdr optionals)))
                (rest (rest (cdr keys))))`
           (fmt :t "~A reqs: ~A opts ~A keys ~A rest ~A~%"
                lambda-list (car requireds) (car optionals) (car keys) rest)
           (list (car requireds) (car optionals) (car keys) rest)))))

(:defsym lambda (:macro (lambda-list :rest body)
   (let* ((llist (parse-lambda lambda-list))
          (reqs (nth 0 llist))
          (opts (nth 1 llist))
          (keys (nth 2 llist))
          (rest (nth 3 llist)))
     'stuff)))

(parse-lambda ())
(parse-lambda '(a b c))

(parse-lambda '(a b c :option d e))
(parse-lambda '(a b c :option d (e 1)))

(parse-lambda '(a b c :option e f :rest rest))
(parse-lambda '(a b c :option e (f 5) :rest rest))

(parse-lambda '(:option a b))
(parse-lambda '(:option a (b 1)))
(parse-lambda '(:option a b :rest rest))
(parse-lambda '(:option a (b 1) :rest rest))

(parse-lambda '(:rest a))

