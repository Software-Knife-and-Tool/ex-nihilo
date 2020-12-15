;;; dotimes/dolist
(defmacro dotimes (sym countf :rest body) 
  (unless (and (symbolp sym) (null (keywordp sym)))
    (raise "is not a non-keyword symbol (dotimes)" sym))
  (let ((count-form countf))
    (unless (fixnump count-form) (raise "is not a fixnum (dotimes)" count-form))
    (let ((bodyf (list* :lambda (list sym) body)))
      (list 'block :nil
        (list 'let (list (list 'loopf (list :lambda (list 'fn 'n)
                            (list 'if (list 'eq count-form 'n)
                              :nil
                              (list 'progn (list bodyf 'n)
                                 (list 'fn 'fn (list 'fixnum+ 1 'n)))))))
              (list 'loopf 'loopf 0))
        :nil))))

(defmacro dolist (sym listf :rest body)
  (unless (and (symbolp sym) (null (keywordp sym)))
    (raise "is not a non-keyword symbol (dolist)" sym))
  (let ((list-form listf))
    (unless (listp list-form) (raise "is not a list (dolist)" list-form))
    (let ((bodyf (list* :lambda (list sym) body)))
      (list 'block :nil (list 'mu.mapc bodyf list-form)))))

;;; copy-list
(defun copy-list (list) (mu.mapcar identity list))

;;; zip-1 - cons pairs of elements in a single list
(defun zip-1 (fn values)
  (check-type fn :lambda "is not a function (zip-1)")
  (when (oddp (length values)) (error "is not an even list" values))
  (mu.foldl (:lambda (acc acons)
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
