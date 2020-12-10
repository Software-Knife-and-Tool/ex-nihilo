;;;
;;; mu forms
;;;
(:defsym .version "0.0.21")

;;; too damn handy
(:defsym identity (:lambda (t) t))

;;; lists
(:defsym null (:lambda (t) (eq :nil t)))

(:defsym list (:lambda (:rest args) args))

(:defsym listp (:lambda (ptr)
  (null (eq (consp ptr) (null ptr)))))

;;; conditional
(:defsym if (:macro (test t f)
  (list (list (list (list :lambda () (list 'eq :nil test)))
        (list :lambda () f)
        (list :lambda () t)))))

;;; list*
(:defsym list* (:lambda (:rest args)
  (if (cdr args)
    (cons (car args) (apply list* (cdr args)))
    (car args))))

;;; progn/let/let*/macrolet
(:defsym progn (:macro (:rest body)
  (list (list* :lambda () body))))

(:defsym let (:macro (bindl :rest body)
  (list*
    (list* :lambda (mapcar car bindl) body)
    (list* (mapcar (:lambda (vl) (nth 1 vl)) bindl)))))

(:defsym macrolet (:macro (bindl :rest body)
   (list*
     (list* :macro (mapcar car bindl) body)
       (list* (mapcar (:lambda (vl) (nth 1 vl)) bindl)))))

;;; gensym
(:defsym gensym
   (let ((gensym-counter 0))
     #'(:lambda ()
        (let ((name (open-output-string "")))
          (:letq gensym-counter (fixnum+ 1 gensym-counter))
          (print "GENSYM-" name :nil)
          (print gensym-counter name :nil)
          (make-symbol (get-output-stream-string name))))))

;;; internal recursive functions
;;; think: this requires mutable lexicals
(:defsym mu.recur (:macro (fn lambda :rest body)
  (if (symbolp fn)
    (if (listp lambda)
      (let ((g (gensym)))
        (list 'let                            
          (list (list g (list :lambda (list fn)
            (list 'closure
              (list :letq fn (list* :lambda lambda body))))))
          (list g :nil)))
        (error "is not a list (recur)" lambda))
    (error "is not a symbol (recur)" fn))))

;;; let*
;;; think: can this use recur?
(:defsym let* (:macro (bindl :rest body)
   (let ((fnc (:lambda (fn bindl body)
           (if (cdr bindl)
             (list 'let (list (car bindl)) (fn fn (cdr bindl) body))
             (list* 'let (list (car bindl)) body)))))
     (fnc fnc bindl body))))

;;; and/or macros
;;; think: can these use recur?
(:defsym and (:macro (:rest forms)
  (let ((andf (:lambda (fn forms)
           (if (cdr forms)
             (list 'if (car forms) (fn fn (cdr forms)) :nil)
             (car forms)))))
    (if forms (andf andf forms) :t))))

(:defsym or (:macro (:rest forms)
  (let* ((value (gensym))
         (orf (:lambda (fn forms)                               
           (if (null (cdr forms))
             (car forms)
             (list 'let (list (list value (car forms)))
                   (list 'mu:if value value (fn fn (cdr forms))))))))
    (if forms (orf orf forms) :nil))))

;;; when/unless macros
(:defsym when (:macro (test :rest forms)
  (list 'if test (list* 'progn forms) :nil)))

(:defsym unless (:macro (test :rest forms)
  (list 'if test :nil (list* 'progn forms))))

;;; block/return macros
(:defsym mu:block (:macro (tag :rest body)
  (list 'mu.block tag (list* :lambda () body))))

(:defsym mu:return-from (:macro (tag value)
  (list 'mu.return tag value)))

(:defsym mu:return (:macro (value)
  (list 'mu.return :nil value)))

;;; letf - recursively defined lexical functions
(:defsym letf (:macro (bindl :rest body)
  (list* 'let
    (mapcar
     (:lambda (phrase)
        (list (car phrase) (list* 'mu.recur phrase)))
      bindl)
    body)))

;;; letf* - recursively defined lexical functions
(:defsym letf* (:macro (bindl :rest body)
  (list* 'let*
    (mapcar
     (:lambda (phrase)
        (list (car phrase) (list* 'mu.recur phrase)))
      bindl)
    body)))

;;; cond
(:defsym cond (:macro (:rest forms)
  (letf ((clauses (forms)
           (and forms
             (let ((clause (car forms))
                   (result (gensym)))
               (if (consp clause)
                 (if (cdr clause)
                        (list 'if (car clause)
                              (list* 'progn (cdr clause))
                              (clauses (cdr forms)))
                        (list 'let (list (list result (car clause)))
                              (list 'if result result (clauses (cdr forms)))))
                 (raise "illegal clause (cond)" forms))))))
        (and forms (clauses forms)))))

;;; foldl/foldr
(:defsym mu.foldl (:lambda (fold-fn fold-init fold-list)
  (letf ((fold (acc listf)
           (if (null listf)
             acc
             (fold (fold-fn acc (car listf)) (cdr listf)))))
    (fold fold-init fold-list))))

(:defsym mu.foldr (:lambda (fold-fn fold-init fold-list)
   (letf ((fold (listf)
            (if (null listf)
              fold-init
              (fold-fn (car listf) (fold (cdr listf))))))
     (fold fold-list))))

;;; append
(:defsym append (:lambda (:rest lists)
  (mu.foldr (:lambda (el acc)
           (if (listp el)
               (mu.foldr (:lambda (el acc) (cons el acc)) acc el)
             el))
         ()
         lists)))

