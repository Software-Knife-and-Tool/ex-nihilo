;;;
;;; debug.l
;;;

;;; pprint
(defun pprint (sexpr stream)
   (letf ((pprint (sexpr stream)
           (let ((function (:lambda (fn)
                              (let ((mapfn
                                     (:lambda (sym)
                                        (when (and (boundp sym) (eq fn (symbol-value sym)))
                                          (return (symbol-name sym))))))
                                (block :nil
                                       (mu::mapc mapfn (ns-symbols (current-ns)))
                                       (mu::mapc mapfn (ns-symbols (find-ns "canon")))
                                       (mu::mapc mapfn (ns-symbols (find-ns "mu")))
                                       ":lambda")))))
             (typecase sexpr
               (:null "()")
;;;               (:cons (fmt stream "~A" (mapcar-1 (:lambda (el) (pprint el stream)) sexpr)))
               (:cons (fmt stream "~A" sexpr))
               (:string (fmt stream "~W" sexpr))
               (:char (fmt stream "~W" sexpr))
               (:func (function sexpr))))))
         (pprint sexpr stream)))

;;; describe
(defun describe (object)
  (let ((shim
          (:lambda (depth)
            (let* ((spacer (open-output-string ""))
                   (loop (:lambda (fn d)
                           (if (eq d 0)
                               (get-output-string-stream spacer)
                             (progn
                               (mu::print "    " spacer :nil)
                               (fn fn (fixnum- d 1)))))))
              (loop loop depth)))))

    (letf ((describe-object (object depth)
              (let* ((spacer (shim depth))
                     (view (view object))
                     (type (mu::vector-ref view 0))
                     (tagptr (mu::vector-ref view 2)))
                     
                (cond
                 ((keywordp object) (fmt :nil "~A~S is an immediate keyword symbol @~X~%" spacer object tagptr))
                 ((and (symbolp object) (macro-function object))
                  (fmt :nil "~A~S is a :macro symbol @~X~%" spacer (symbol-name object) tagptr)
                  (describe-object (macro-function object) (fixnum+ depth 1)))
                 ((symbolp object) (fmt :nil "~A~S is a heap symbol :symbol~%~A    name: ~A~%~A    namespace: ~A~%~A    value:~%~A~%"
                                        spacer
                                        object
                                        spacer
                                        (symbol-name object)
                                        spacer
                                        (ns-name (symbol-ns object))
                                        spacer
                                        (describe-object (symbol-value object) (fixnum+ depth 1))))
                 ((stringp object) (fmt :nil "~A~S is a ~A string of length ~A (@~X)~%"
                                        spacer object (if (fixnum< (mu::vector-length object) 8) "immediate" "heap") (mu::vector-length object) tagptr))
                 ((vectorp object) (fmt :nil "~A~S is a ~A :vector of length ~A (@~X)~%" spacer object (svtype object) (mu::vector-length object) tagptr))
                 ((fixnump object) (fmt :nil "~A~S is a fixnum (@~X)~%" spacer object tagptr))
                 ((floatp object) (fmt :nil "~A~S is a float (@~X)~%" spacer object tagptr))
                 ((consp object) (fmt :nil "~A~S is a cons (@~X)~%" spacer object tagptr))
                 ((streamp object) (fmt :nil "~A~S is a simple stream (@~X)~%" spacer object tagptr))
                 ((structp object) (fmt :nil "~A~S is a struct (@~X)~%" spacer object tagptr))
                 ((namespacep object) (fmt :nil "~A~S is a namespace (@~X)~%" spacer object tagptr))
                 ((codep object) (fmt :nil "~A~S is a code object (@~X)~%" spacer object tagptr))
                 ((functionp object)
                  (fmt :nil
                       "~A~S is a function (@~X)~%~A    symbol: ~A~%~A    nreqs: ~A~%~A    lambda: ~A~%~A    body: ~A~%~A    frame-id: ~A~%~A    name: ~A~%"
                       spacer
                       object
                       tagptr
                       spacer
                       (pprint object :nil)
                       spacer
                       (mu::vector-ref view 7)
                       spacer
                       (mu::vector-ref view 6)
                       spacer
                       (mu::vector-ref view 3)
                       spacer
                       (mu::vector-ref view 5)
                       spacer
                       (mu::vector-ref view 8)))
                 (:t (fmt :nil "~Aundecoded ~W type-of ~A (@~X)~%" spacer object (type-of object) tagptr))))))
          (mu::print (describe-object object 0) :nil :nil)))
    :nil)

;;; break loop
(defun break (ex)
  (let ((env (mu::env-view)))
    (when (eq (type-of ex) :except)
      (let* ((ex-view (view ex))
             (tag (mu::vector-ref ex-view 3))
             (source (mu::vector-ref ex-view 4))
             (reason (mu::vector-ref ex-view 5))
             (frame (mu::vector-ref ex-view 6)))
        (fmt :t "break loop invoked with ")
        (cond       
         ((eq tag :arith) (fmt :t "arithmetic-error~%"))
         ((eq tag :cell) (fmt :t "cell-error~%"))
         ((eq tag :control) (fmt :t "control-error~%"))
         ((eq tag :eof) (fmt :t "end-of-file on stream ~A~%" source))
         ((eq tag :file) (fmt :t "file-error on ~A~%" source))
         ((eq tag :fpinex) (fmt :t "floating-point-inexact~%"))
         ((eq tag :fpinv) (fmt :t "floating-point-invalid-operation~%"))
         ((eq tag :fpover) (fmt :t "floating-point-overflow~%"))
         ((eq tag :fpunder) (fmt :t "floating-point-underflow~%"))
         ((eq tag :parse) (fmt :t "parse-error while reading ~A~%" source))
         ((eq tag :print) (fmt :t "object ~A signals print-not-readable~%"))
         ((eq tag :program) (fmt :t "program-error~%"))
         ((eq tag :read) (fmt :t "reader-error~%"))
         ((eq tag :simple) (fmt :t "simple-error~%"))
         ((eq tag :store) (fmt :t "storage-condition~%"))
         ((eq tag :stream) (fmt :t "stream-error~%"))
         ((eq tag :type) (fmt :t "type-error from ~A~%" source))
         ((eq tag :unfunc) (fmt :t "undefined-function ~A~%" source))
         ((eq tag :unslot) (fmt :t "unbound-slot ~A~%"))
         ((eq tag :unsym) (fmt :t "the symbol ~A is unbound~%" source))
         ((eq tag :zerodiv) (fmt :t "arithmetic error division-by-zero~%"))
         (:t (fmt :t "undecoded-exception-type~%")))
        (fmt :t ":p for help~%~%")
        (fmt :t "    ~A (~A . ~A)~%" reason (mu::vector-ref frame 1) (mu::vector-ref frame 3))))

    (block :nil
      (while (null (eofp :t))
        (let ((sexpr (progn (fmt :t "break> ") (read :t))))
          (cond
           ((eq sexpr :p) (fmt :t ":p - this message~%")
            (fmt :t ":c - continue~%")
            (fmt :t ":b - backtrace~%")
            (fmt :t ":e - exception details~%"))
           ((eq sexpr :c) (return :t))
           ((eq sexpr :e)
            (when (eq (type-of ex) :except)
              (let* ((ex-view (view ex))
                     (tag (mu::vector-ref ex-view 3))
                     (source (mu::vector-ref ex-view 4))
                     (frame (mu::vector-ref ex-view 6))
                     (reason (mu::vector-ref ex-view 5)))
                (fmt :t "exception: ~A~%" ex)
                (fmt :t "      tag: ~A~%" tag)
                (fmt :t "   source: ~A~%" (pprint source :nil))
                (fmt :t "   reason: ~A~%" reason)
                (fmt :t "    frame: ~A~%" frame)
                (fmt :t "           ~A~%"
                     (cons (pprint (mu::vector-ref frame 1) :nil) (mu::vector-ref frame 3))))))
           ((eq sexpr :b)
            (let* ((stack (mu::vector-ref env 5)))
              (fmt :t "backtrace:~%")
              (mu::mapc
               (:lambda (el)
                  (let ((function (mu::vector-ref el 1))
                        (nargs (mu::vector-ref el 2))
                        (argl (mu::vector-ref el 3))
                        (frame-id (mu::vector-ref el 4)))
                    (if (null argl)
                        (fmt :t "  (~A)~%" function)
                      (fmt :t "  (~A ~A)~%" function argl))))
               (cdr stack))))
           (:t (with-exception
                (:lambda () (fmt :t "~A~%" (eval sexpr)))
                (:lambda (ex))))))))))
