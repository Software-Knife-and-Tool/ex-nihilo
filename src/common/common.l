;;; (in-ns (find-ns "common"))

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

;;; error
(defun error (obj :rest fmt-list)
  (let ((fmt-string (car fmt-list))
        (args (cdr fmt-list)))
    (if (stringp fmt-string)
      (raise (mu::apply fmt (list* :nil fmt-string args)) obj)
      (raise "error: " obj))))

;;; lambda
(defmacro lambda-common (args :rest body)
  (unless (listp args) (raise "must be a list (lambda)" args))
  (list* :lambda () body))

