;;;
;;; rewrite
;;;
(defun rewrite (form)
  (let ((rewrites
         (list
          (cons 'list (:lambda (fn args)
                        (cond ((null args)
                               (fmt :t ";;;    null list rewrite: :nil~%") :nil)
                              (:t
                               (let ((fold (constant-fold fn args)))
                                 (if fold
                                     (progn (fmt :t ";;;    constant list rewrite: ~A~%" (:quote fold))
                                            (:quote fold))
                                     form))))))
          (cons 'fixnum/ (:lambda (fn args)
                           (let ((arg0 (nth 0 args))
                                 (arg1 (nth 1 args)))
                             (cond
                              ((eq arg1 1)
                               (fmt :t ";;;    fixnum/ identity rewrite: ~A~%" arg0) arg0)
                              (list :t form)))))
          (cons 'fixnum- (:lambda (fn args)
                           (let ((arg0 (nth 0 args))
                                 (arg1 (nth 1 args)))
                             (cond--------------------------
                              ((zero? arg1)
                               (fmt :t ";;;    fixnum- identity rewrite: ~A~%" arg0) arg0)
                              (list :t form)))))
          (cons 'fixnum+ (:lambda (fn args)
                           (let ((arg0 (nth 0 args))
                                 (arg1 (nth 1 args)))
                             (cond
                              ((zero? arg0)
                               (fmt :t ";;;    fixnum+ identity rewrite: ~A~%" arg1) arg1)
                              ((zero? arg1)
                               (fmt :t ";;;    fixnum+ identity rewrite: ~A~%" arg0) arg0)
                             (list :t form)))))
          (cons 'fixnum* (:lambda (fn args)
                           (let ((arg0 (nth 0 args))
                                 (arg1 (nth 1 args)))
                             (cond
                              ((eq 1 arg0)
                               (fmt :t ";;;    fixnum* identity rewrite: ~A~%" arg1) arg1)
                              ((eq 1 arg1)
                               (fmt :t ";;;    fixnum* identity rewrite: ~A~%" arg0) arg0)
                             (list :t form)))))
          (cons 'apply-1 (:lambda (fn args)
                           (fmt :t ";;;    apply-1 rewrite: ~S~%" fn)
                           (list* fn args))))))

    (cond ((constant? form) form)
          (:t (let ((rwf (assoc (car form) rewrites)))
                (if rwf
                  (rwf (car form) (cdr form))
                  form))))))

