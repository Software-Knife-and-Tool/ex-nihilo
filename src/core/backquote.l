;;;
;;;    SPDX-License-Identifier: MIT
;;;    Copyright (c) 2017-2021 James M. Putnam <putnamjm.design@gmail.com>
;;;
(:defsym parse-bq (:lambda (form)
    (letf ((bq (form)
             (mu::mapcar
              (:lambda (el)
                (fmt :t "bq: ~A~%" el)
                (cond
                 ((consp el)
                  (let ((fn (car el)) (body (cdr el)))
                    (cond
                     ((eq fn 'comma) (fmt :t "saw ,~A~%" body))
                     ((eq fn 'comma-at) (fmt :t "saw ,@~A~%" body))
                     (:t el))))
                 (:t el)))
              form)))
          (fmt :t "parse-bq: ~A~%" form)
          (cond
           ((null form) form)
           ((consp form) (bq form))
           ((and (vectorp form) (eq :t (vector-type form))) (raise "implement bq vectors" form))
           (:t (list :quote form))))))

(mu::mapc
 (:lambda (el) (parse-bq el))
 '(()
  foo
  (a b c d)
  ((comma 123))))
