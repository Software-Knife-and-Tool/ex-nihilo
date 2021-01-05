;;;
;;;    SPDX-License-Identifier: MIT
;;;    Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
;;;

;;; quasiquote
(:defsym parse-quasi (:lambda (form)
   (letf ((quasi-list (form)
     (letf ((unquote (form)
              (and (consp form) (eq :unquote (car form)) (list 'list (nth 1 form))))
            (unquote-splice (form)
              (and (consp form) (eq :splice (car form)) (nth 1 form)))
            (quasi-basic (form)
              (and (consp form) (eq :quasi (car form)) (atom (nth 1 form))(nth 1 form)))
            (quasi (form)
              (and (consp form) (eq :quasi (car form)) (consp (nth 1 form))
                   (list* 'append (mu::mapcar quasi-list (nth 1 form))))))
         (cond
          ((null form) form)
          ((unquote form))
          ((unquote-splice form))
          ((quasi-basic form))
          ((quasi form))
          ((and (vectorp form) (eq :t (vector-type form))) (raise "implement bq vectors" form))
          (:t (list 'list (list :quote form)))))))
       (cond
          ((null form) form)
          ((and (consp form) (eq :unquote (car form))) (nth 1 form))
          ((and (consp form) (eq :splice (car form)) (nth 1 form))) ;;; undefined consequences
          ((and (consp form) (eq :quasi (car form)) (atom (nth 1 form))) (nth 1 form))
          ((quasi-list form))
          ((and (vectorp form) (eq :t (vector-type form))) (raise "implement bq vectors" form))
          (:t (list 'list (list :quote form)))))))
         
(mu::mapc
 (:lambda (test)
    (let ((form (car test)) (result (cdr test)))
      (fmt :t "---~%~A~%~A~%~A~%~A~%" form (parse-quasi form) (eval (parse-quasi form)) result)))
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
