;;; atom
(defun atom (obj)
  (null (or (null obj) (consp obj))))

;;; typep
(defun typep (object type)
  (unless (keywordp type) (error "is not a type keyword (typep)" type)) 
  (or (eq type :t) (eq (type-of object) type)))

;;; 1+ fixnum
(defun 1+ (fx)
  (check-type fx :fixnum "is not a fixnum (1+)")
  (fixnum+ fx 1))

;;; 1- fixnum
(defun 1- (fx)
  (check-type fx :fixnum "is not a fixnum (1-)")
  (fixnum- fx 1))

;;; lambda
(defmacro lambda (args :rest body)
  (unless (listp args) (raise "must be a list (lambda)" args))
  (list* :lambda () body))

