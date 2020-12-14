;;;
;;; mu forms
;;;
(:defsym .version "0.0.21")

;;; lists
(:defsym null (:lambda (t) (eq :nil t)))

(:defsym list (:lambda (:rest args) args))

(:defsym listp (:lambda (ptr)
  (null (eq (consp ptr) (null ptr)))))

;;; block/return macros
(:defsym mu:block (:macro (tag :rest body)
  (list 'mu.block tag (list* :lambda () body))))

(:defsym mu:return-from (:macro (tag value)
  (list 'mu.return tag value)))

(:defsym mu:return (:macro (value)
  (list 'mu.return :nil value)))

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

;;; progn/let/let*/letf/letf*/macrolet
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

;;; when/unless macros
(:defsym when (:macro (test :rest forms)
  (list 'if test (list* 'progn forms) :nil)))

(:defsym unless (:macro (test :rest forms)
  (list 'if test :nil (list* 'progn forms))))

;;; gensym
;;; think: this requires mutable lexicals
(:defsym gensym
   (let ((gensym-counter 0))
     #'(:lambda ()
        (let ((name (open-output-string "")))
          (:letq gensym-counter (fixnum+ 1 gensym-counter))
          (print "GENSYM-" name :nil)
          (print gensym-counter name :nil)
          (make-symbol (get-output-stream-string name))))))

;;; lexical bindings
;;; think: this requires mutable lexicals
(:defsym recur (:macro (fn lambda :rest body)
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

;;; letf - recursive serial lexical functions
(:defsym letf (:macro (bindl :rest body)
  (list* 'let
    (mapcar
     (:lambda (phrase)
        (list (car phrase) (list* 'recur phrase)))
      bindl)
    body)))

;;; letf* - recursive parallel lexical functions
(:defsym letf* (:macro (bindings :rest body)
  (list* 'let*
    (mapcar
     (:lambda (phrase)
        (list (car phrase) (list* 'recur phrase)))
      bindings)
    body)))

;;; let*
(:defsym let* (:macro (bindings :rest body)
   (letf ((bindfn (bindings body)
           (if (cdr bindings)
             (list 'let (list (car bindings)) (bindfn (cdr bindings) body))
             (list* 'let (list (car bindings)) body))))
     (bindfn bindings body))))

;;; and/or
(:defsym and (:macro (:rest forms)
  (letf ((andf (forms)
           (if (cdr forms)
             (list 'if (car forms) (andf (cdr forms)) :nil)
             (car forms))))
    (if forms (andf forms) :t))))

(:defsym or (:macro (:rest forms)
   (let ((value (gensym)))
     (letf ((orf (forms)
              (if (null (cdr forms))
                 (car forms)
                 (list 'let (list (list value (car forms)))
                   (list 'mu:if value value (orf (cdr forms))))))))
       (if forms (orf forms) :nil))))

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
(:defsym foldl (:lambda (fold-fn fold-init fold-list)
  (letf ((fold (acc listf)
           (if (null listf)
             acc
             (fold (fold-fn acc (car listf)) (cdr listf)))))
    (fold fold-init fold-list))))

(:defsym foldr (:lambda (fold-fn fold-init fold-list)
   (letf ((fold (listf)
            (if (null listf)
              fold-init
              (fold-fn (car listf) (fold (cdr listf))))))
     (fold fold-list))))

;;; string accessor
(:defsym schar (:lambda (string index)
  (vector-ref string index)))

;;; stringp - string predicate
(:defsym stringp (:lambda (object)
  (and (vectorp object) (eq (vector-type object) :char))))

;;; string - string coercion
(:defsym string (:lambda (object)
  (cond
   ((stringp object) object)
   ; ((charp object) (fmt :nil "~A" object))
   ((symbolp object) (symbol-name object))
   (:t (raise "illegal conversion (string)" object)))))

;;; string= - string comparison
(:defsym string= (:lambda (str1 str2)
  (unless (stringp str1) (raise "is not a string (string=)" str1))
  (unless (stringp str2) (raise "is not a string (string=)" str2))
  (when (eq (vector-length str1) (vector-length str2))
    (block :nil
      (let ((nth 0)
            (len (vector-length str1)))
        (letf ((inc ()
           (cond
            ((eq nth len) :t)
            ((eq (vector-ref str1 nth) (vector-ref str2 nth))
             (:letq nth (fixnum+ 1 nth))
             inc)
            (:t :nil))))
              (trampoline inc)))))))

;;; vector-to-list - make a list from vector
(:defsym vector-to-list (:lambda (vector)
  (let ((lst :nil))
    (vector-mapc
     (:lambda (el) (:letq lst (cons el lst)))
     vector)
    (foldl (:lambda (x r) (cons r x)) () lst))))

;;; append
(:defsym append (:lambda (:rest lists)
  (foldr (:lambda (el acc)
           (if (listp el)
               (mu.foldr (:lambda (el acc) (cons el acc)) acc el)
             el))
         ()
         lists)))

