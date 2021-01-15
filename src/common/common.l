;;; typep
(defun typep (object type)
  (unless (keywordp type) (error "is not a type keyword (typep)" type)) 
  (or (eq type :t) (eq (type-of object) type)))

;;; eql
(defun eql (x y)
  (let ((eql (:lambda (x y)
          (cond
           ((eq x y) :t)
           ((null (eq (type-of x) (type-of y))) :nil)
           ((stringp x) (string= x y))
           ((and (vectorp x) (vectorp y))
            (and (eq (mu::vector-type x) (mu::vector-type y))
                 (eq (mu::vector-length x) (mu::vector-length y))
                 (eql (vector-to-list x) (vector-to-list y))))
           ((consp x)
            (and (eq (length x) (length y))
                 (eql (car x) (car y))
                 (eql (cdr x) (cdr y))))
           (:t :nil)))))
    (eql x y)))

;;; check-type form type string
(defmacro check-type (form type reason)
  (unless (keywordp type) (raise "type is not a keyword" type))
  (unless (stringp reason) (raise "reason is not a string" reason))
  (let ((arg-form (gensym)))
    (list 'let (list (list arg-form form))
      (list 'if (list 'eq type :list)
        (list 'if (list 'listp arg-form)
          :nil
          (list 'raise reason arg-form))
        (list 'if (list 'eq (list 'type-of arg-form) type)
            :nil
            (list 'raise reason arg-form))))))

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

