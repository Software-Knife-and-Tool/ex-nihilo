;;;
;;;    SPDX-License-Identifier: MIT
;;;    Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
;;;

;;; quasiquote
(:defsym parse-quasi (:lambda (quote-form)
   (letf ((quasi-list (form)
            (cond
              ((null form) form)
              ((and (consp form) (eq :unquote (car form))) (list 'list (nth 1 form)))
              ((and (consp form) (eq :splice (car form))) (nth 1 form))
              ((and (consp form) (eq :quasi (car form)) (atom (nth 1 form))) (nth 1 form))
              ((and (consp form) (eq :quasi (car form)) (consp (nth 1 form)))
               (list* 'append (mu::mapcar quasi-list (nth 1 form))))
              ((and (vectorp form) (eq :t (vector-type form))) (raise "implement bq vectors" form))
              (:t (list 'list (list :quote form))))))
       (let ((form (nth 1 quote-form)))
         ;;; (fmt :t "parse-quasi: ~A -> ~A~%" quote-form form)
         (cond
           ((or (null form) (atom form)) form)
           ((and (consp form) (eq :unquote (car form))) (nth 1 form))
           ((and (consp form) (eq :splice (car form)) (nth 1 form))) ;;; undefined consequences
           ((and (consp form) (eq :quasi (car form)) (atom (nth 1 form))) (nth 1 form))
           ((and (vectorp form) (eq :t (vector-type form))) (raise "implement bq vectors" form))
           ((consp form) (quasi-list form))
           (:t :t))))))
         
(mu::mapc
 (:lambda (test)
    (let ((form (car test)) (result (parse-quasi (car test))))
      (fmt :t "---~%~A~%~A~%~A = ~A~%" form result (eval result) (cdr test))))
 '(((:quasi 123) . 123)
   ((:quasi (:unquote 123)) . 123)
   ((:quasi (:unquote (fixnum+ 1 2))) . 3)
   ((:quasi (a (fixnum+ 1 2) c)) . (a (fixnum+ 1 2) c))
   ((:quasi (a (:unquote (fixnum+ 1 2)) c)) . (a 3 c))
   ((:quasi (a (list 1 2) c)) . (a (list 1 2) c))
   ((:quasi (a (:unquote (list 1 2)) c)) . (a (1 2) c))
   ((:quasi (a (:splice (list 1 2)) c)) . (a 1 2 c))))

#|
Backquote Syntax	Equivalent List-Building Code	Result
`(a (+ 1 2) c)	        (list 'a '(+ 1 2) 'c)	(a (+ 1 2) c)
`(a ,(+ 1 2) c)	        (list 'a (+ 1 2) 'c)	(a 3 c)
`(a (list 1 2) c)	(list 'a '(list 1 2) 'c)	(a (list 1 2) c)
`(a ,(list 1 2) c)	(list 'a (list 1 2) 'c)	(a (1 2) c)
`(a ,@(list 1 2) c)	(append (list 'a) (list 1 2) (list 'c))	(a 1 2 c)
|#
