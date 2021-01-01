;;;
;;;    SPDX-License-Identifier: MIT
;;;    Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
;;;
(:defsym parse-quasi (:lambda (form)
;;;   (fmt :t "parse-quasi: ~A~%" form)
   (letf ((unquote (form)
            (and (consp form) (eq :unquote (car form)) (list (nth 1 form))))
          (unquote-splice (form)
            (and (consp form) (eq :splice (car form)) (nth 1 form)))
          (quasi (form)
             (and (consp form)
                  (eq :quasi (car form))
                  (consp (nth 1 form))
                 (list* 'append (mu::mapcar parse-quasi (nth 1 form))))))
         (cond
          ((null form) form)
          ((unquote form))
          ((unquote-splice form))
          ((quasi form))
          ((and (vectorp form) (eq :t (vector-type form))) (raise "implement bq vectors" form))
          (:t (list (list :quote form)))))))

(mu::mapc
 (:lambda (form)
    (fmt :t "~A -> ~A~%" form (parse-quasi form)))
 '((:quasi (a (+ 1 2) c))
   (:quasi (a (:unquote (+ 1 2)) c))
   (:quasi (a (list 1 2) c))
   (:quasi (a (:unquote (list 1 2)) c))
   (:quasi (a (:splice (list 1 2)) c))))

#|
Backquote Syntax	Equivalent List-Building Code	Result
`(a (+ 1 2) c)	        (list 'a '(+ 1 2) 'c)	(a (+ 1 2) c)
`(a ,(+ 1 2) c)	        (list 'a (+ 1 2) 'c)	(a 3 c)
`(a (list 1 2) c)	(list 'a '(list 1 2) 'c)	(a (list 1 2) c)
`(a ,(list 1 2) c)	(list 'a (list 1 2) 'c)	(a (1 2) c)
`(a ,@(list 1 2) c)	(append (list 'a) (list 1 2) (list 'c))	(a 1 2 c)
|#
