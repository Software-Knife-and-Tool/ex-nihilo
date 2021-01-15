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
      (list 'block :nil (list 'mu::mapc bodyf list-form)))))

;;; copy-list
(defun copy-list (list) (mu.mapcar identity list))

