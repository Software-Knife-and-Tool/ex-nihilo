;;; zip-1 - cons pairs of elements in a single list
(defun zip-1 (fn values)
  (check-type fn :lambda "is not a function (zip-1)")
  (when (oddp (length values)) (raise "is not an even list" values))
  (mu:foldl (:lambda (acc acons)
           (if (null acons)
             acc
             (fn acons acc)))
         ()
         (mu.maplist
          (:lambda (cdr)
            (unless (oddp (length cdr)) (cons (nth 0 cdr) (nth 1 cdr))))
            values)))

;;; zip-2 - cons pairs of elements in two lists
(defun zip-2 (list-1 list-2)
  (let ((zip (:lambda (fn l1 l2)
    (if (or (null l1) (null l2))
      ()
      (cons (cons (car l1) (car l2)) (fn fn (cdr l1) (cdr l2)))))))
    (zip zip list-1 list-2)))
