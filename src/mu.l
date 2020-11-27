;;;
;;; mu forms
;;;
(:defcon .version "0.0.18")

;;; list
(:defcon list (:lambda (:rest args) args))

;;; conditional
(:defcon mu:if (:macro (test-expr t-expr f-expr)
  (list (list 'mu.if
              test-expr
              (list :lambda () t-expr)
              (list :lambda () f-expr)))))

;;; predicates
(:defcon listp (:lambda (ptr)
  (mu:if (consp ptr) ptr (null ptr))))

;;; bool coerce
(:defcon bool (:lambda (ptr)
  (mu:if (null ptr) ptr :t)))

;;; list*
(:defcon list* (:lambda (:rest args)
  (mu:if (cdr args)
      (cons (car args) (apply list* (cdr args)))
    (car args))))

;;; progn/let/let*/macrolet
(:defcon progn (:macro (:rest body)
  (list (list* :lambda :nil body))))

(:defcon let (:macro (bindl :rest body)
   (list*
     (list* :lambda (mapcar car bindl) body)
       (list* (mapcar (:lambda (vl) (nth 1 vl)) bindl)))))

(:defcon macrolet (:macro (bindl :rest body)
   (list*
     (list* :macro (mapcar car bindl) body)
       (list* (mapcar (:lambda (vl) (nth 1 vl)) bindl)))))

(:defcon let* (:macro (bindl :rest body)
   (let ((fnc (:lambda (fn bindl body)
           (mu:if (cdr bindl)
             (list 'let (list (car bindl)) (fn fn (cdr bindl) body))
             (list* 'let (list (car bindl)) body)))))
     (fnc fnc bindl body))))

;;; and/or macros
(:defcon and (:macro (:rest forms)
  (let ((andf (:lambda (fn forms)
           (mu:if (cdr forms)
             (list 'mu:if (car forms) (fn fn (cdr forms)) :nil)
             (car forms)))))
    (mu:if forms (andf andf forms) :t))))

(:defcon or (:macro (:rest forms)
  (let* ((value (gensym))
         (orf (:lambda (fn forms)                               
           (mu:if (null (cdr forms))
             (car forms)
             (list 'let (list (list value (car forms)))
                   (list 'mu:if value value (fn fn (cdr forms))))))))
    (mu:if forms (orf orf forms) :nil))))

;;; gensym
(:defcon gensym
   (let ((gensym-counter 0))
     #'(:lambda ()
        (let ((name (open-output-string "")))
          (:letq gensym-counter (fixnum+ 1 gensym-counter))
          (print "GENSYM-" name :nil)
          (print gensym-counter name :nil)
          (make-symbol (get-output-string-stream name))))))

;;; when/unless macros
(:defcon when (:macro (test :rest forms)
  (list 'mu:if test (list* 'progn forms) :nil)))

(:defcon unless (:macro (test :rest forms)
  (list 'mu:if test :nil (list* 'progn forms))))

;;; block/return macros
(:defcon mu:block (:macro (tag :rest body)
  (list 'mu.block tag (list* :lambda () body))))

(:defcon mu:return-from (:macro (tag value)
  (list 'mu.return tag value)))

(:defcon mu:return (:macro (value)
  (list 'mu.return :nil value)))

;;; internal recursive functions
;;; note: this requires mutable lexicals
(:defcon mu.recur (:macro (fn lambda :rest body)
  (unless (symbolp fn) (error "is not a symbol (recur)" fn))
  (unless (listp lambda) (error "is not a list (recur)" lambda))
  (let ((f (gensym)))
    (list 'let                            
      (list (list f (list :lambda (list fn)
        (list 'closure
          (list :letq fn (list* :lambda lambda body))))))
      (list f :nil)))))

;;; letf - recursively defined lexical functions
(:defcon letf (:macro (bindl :rest body)
  (list* 'let
    (mapcar
     (:lambda (phrase)
        (list (car phrase) (list* 'mu.recur phrase)))
      bindl)
    body)))

;;; letf* - recursively defined lexical functions
(:defcon letf* (:macro (bindl :rest body)
  (list* 'let*
    (mapcar
     (:lambda (phrase)
        (list (car phrase) (list* 'mu.recur phrase)))
      bindl)
    body)))

;;; cond
(:defcon cond (:macro (:rest forms)
  (letf ((clauses (forms)
           (and forms
             (let ((clause (car forms))
                   (result (gensym)))
               (mu:if (consp clause)
                 (mu:if (cdr clause)
                        (list 'mu:if (car clause)
                              (list* 'progn (cdr clause))
                              (clauses (cdr forms)))
                        (list 'let (list (list result (car clause)))
                              (list 'mu:if result result (clauses (cdr forms)))))
                 (raise "illegal clause (cond)" forms))))))
        (and forms (clauses forms)))))

;;; defun (now with added recursion)
(:defcon defun (:macro (fn lambda :rest body)
  (unless (symbolp fn) (error "is not a symbol (defun)" fn))
  (unless (listp lambda) (error "is not a list (defun)" lambda))
  (list :defcon fn (list* 'mu.recur fn lambda body))))

;;; defconstant
(:defcon defconstant (:macro (name form)
  (unless (symbolp name) (error "is not a symbol (defun)" name))
  (list :defcon name form)))

;;; defmacro
(:defcon defmacro (:macro (name lambda :rest body)
  (unless (symbolp name) (error "is not a symbol (defun)" name))
  (unless (listp lambda) (error "is not a list (defun)" lambda))
  (list :defcon name (list* :macro lambda body))))

;;; foldl/foldr
(:defcon mu.foldl (:lambda (fold-fn fold-init fold-list)
  (letf ((fold (acc listf)
           (mu:if (null listf)
             acc
             (fold (fold-fn acc (car listf)) (cdr listf)))))
    (fold fold-init fold-list))))

(:defcon mu.foldr (:lambda (fold-fn fold-init fold-list)
   (letf ((fold (listf)
            (mu:if (null listf)
              fold-init
              (fold-fn (car listf) (fold (cdr listf))))))
     (fold fold-list))))

;;; append
(:defcon append (:lambda (:rest lists)
  (mu.foldr (:lambda (el acc)
           (mu:if (listp el)
               (mu.foldr (:lambda (el acc) (cons el acc)) acc el)
             el))
         ()
         lists)))

