;;;
;;;    SPDX-License-Identifier: MIT
;;;    Copyright (c) 2017-2020 James M. Putnam <putnamjm.design@gmail.com>
;;;

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
                llist)
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
             (:t (inits (cdr llist)))))
          (keys (llist)
            (cond
             ((or (null (eq (car llist) :key)) (null llist)) (cons () llist))
             (:t (inits (cdr llist)))))
          (rest (llist)
            (cond
              ((or (null (eq (car llist) :rest)) (null llist)) (cons () llist))
              (:t (unless (eq 2 (length llist))
                    (raise "excess elements (lambda)" llist))
                  (let ((sym (nth 1 llist)))
                    (unless (and (symbolp sym) (null (keywordp sym)))
                      (raise "illegal rest clause (lambda)" llist))
                    (cons sym (nthcdr 2 llist)))))))

         (let* ((requireds (reqs lambda-list))
                (optionals (opts (cdr requireds)))
                (keywords (keys (cdr optionals)))
                (rest (rest (cdr keywords))))
           (list (car requireds) (car optionals) (car keywords) (car rest))))))

(:defsym lambda (:macro (lambda-list :rest body)
  (let* ((llist (parse-lambda lambda-list))
          (reqs (nth 0 llist))
          (opts (nth 1 llist))
          (keys (nth 2 llist))
          (rest (nth 3 llist)))
     (cond
      ((null rest)
       (fmt :t "lambda: ~A~%" (list* :lambda reqs body))
       (list* :lambda reqs body))
      (:t)))))

#|
(fmt :t "~A~%" (parse-lambda ()))
(fmt :t "~A~%" (parse-lambda '(a b c)))

(fmt :t "~A~%" (parse-lambda '(a b c :option d e)))
(fmt :t "~A~%" (parse-lambda '(a b c :option d (e 1))))

(fmt :t "~A~%" (parse-lambda '(a b c :option e f :rest rest)))
(fmt :t "~A~%" (parse-lambda '(a b c :option e (f 5) :rest rest)))

(fmt :t "~A~%" (parse-lambda '(:option a b)))
(fmt :t "~A~%" (parse-lambda '(:option a (b 1))))
(fmt :t "~A~%" (parse-lambda '(:option a b :rest rest)))
(fmt :t "~A~%" (parse-lambda '(:option a (b 1) :rest rest)))

(fmt :t "~A~%" (parse-lambda '(:rest a)))
|#
