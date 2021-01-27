;;; print newline, then object 
(defun print (obj :rest args)
  (let ((stream
         (cond
          ((null args) standard-output)
          ((eq (mu::length args) 1)
             (if (null (car args))
               standard-output
               (car args)))
          (:t (error "improper argument list (print)" args)))))
    (cond
     ((streamp stream) (terpri stream) (mu::print obj stream :nil))
     (:t (error "is not a stream (print)" stream)))
  obj))

;;; print object with escapes
(defun prin1 (obj :rest args)
  (let ((stream
         (cond
          ((null args) standard-output)
          ((eq (mu::length args) 1)
             (if (null (car args))
               standard-output
               (car args)))
          (:t (error "improper argument list (prin1)" args)))))
    (cond
     ((streamp stream) (mu::print obj stream :t))
     (:t (error "is not a stream (prin1)" stream)))
  obj))

;;; print object without escapes
(defun princ (obj :rest args)
  (let ((stream
         (cond
          ((null args) standard-output)
          ((eq (mu::length args) 1)
             (if (null (car args))
               standard-output
               (car args)))
          (:t (error "improper argument list (princ)" args)))))
    (cond
     ((streamp stream) (mu::print obj stream :nil))
     (:t (error "is not a stream (princ)" stream)))
  obj))

;;; prin1-to-string object
(defun prin1-to-string (obj)
  (let ((stream (open-output-string "")))
    (mu::print obj stream :t)
    (get-output-string-stream stream)))

;;; princ-to-string object
(defun princ-to-string (obj)
  (let ((stream (open-output-string "")))
    (mu::print obj stream :nil)
    (get-output-string-stream stream)))

;;; read-from-string string
(defun read-from-string (str)
  (check-type str :string "read-from-string: is not a string (read-from-string)")
  (mu:read (open-input-string str)))

;;; read-from-string-stream string
(defun read-from-string-stream (stream)
  (check-type stream :stream "read-from-string: is not a stream (read-from-string-stream)")
  (read-from-string (get-output-string-stream stream)))

