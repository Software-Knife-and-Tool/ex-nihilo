;;;
;;;    SPDX-License-Identifier: MIT
;;;    Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
;;;
(:defsym parse-bq (:lambda (form)
   (letf ((bq (form)
     (cons 'append                
       (mu::mapcar
        (:lambda (el)
           (cond
            ((consp el)
             (let ((fn (car el)) (body (cdr el)))
               (cond
                ((eq fn 'comma) (list 'list (car body)))
                ((eq fn 'comma-at) body)
                (:t el))))
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
  (a b c d)
  ((comma 123))))
