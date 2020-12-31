;;;
;;;    SPDX-License-Identifier: MIT
;;;    Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
;;;
(:defsym parse-bq (:lambda (form)
   (letf ((bq (form)
     (cons 'list
       (mu::mapcar
        (:lambda (el)
           (cond
            ((consp el)
             (let ((fn (car el)) (form (nth 1 el)))
               (cond
                ((eq fn 'unquote) (list form))
                ((eq fn 'unquote-splice)
                  (unless (listp form) (raise "unquote-splice: is not a list" form))
                  form)
                (:t (list :quote el)))))
            (:t (parse-bq el))))
        form))))
         (cond
          ((null form) form)
          ((consp form) (bq form))
          ((and (vectorp form) (eq :t (vector-type form))) (raise "implement bq vectors" form))
          (:t (list :quote form))))))

(mu::mapc
 (:lambda (el)
    (fmt :t "~A -> ~A~%" el (parse-bq el)))
 '(()
  foo
  (a)
  (a (+ 1 2) c)
  (a (unquote (+ 1 2)) c)
  ((unquote 123))
  ((unquote-splice '(123)))))

#|
Backquote Syntax	Equivalent List-Building Code	Result
`(a (+ 1 2) c)	        (list 'a '(+ 1 2) 'c)	(a (+ 1 2) c)
`(a ,(+ 1 2) c)	        (list 'a (+ 1 2) 'c)	(a 3 c)
`(a (list 1 2) c)	(list 'a '(list 1 2) 'c)	(a (list 1 2) c)
`(a ,(list 1 2) c)	(list 'a (list 1 2) 'c)	(a (1 2) c)
`(a ,@(list 1 2) c)	(append (list 'a) (list 1 2) (list 'c))	(a 1 2 c)
|#
