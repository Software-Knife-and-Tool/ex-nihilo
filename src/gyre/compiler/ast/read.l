;;;
;;; read
;;;
(defun llc-read (src)
  (let* ((find-in (:lambda (ch str)
           (find-if (:lambda (el) (eq ch el)) str)))
         (syntax-type (:lambda (ch)
           (cond
            ((eq ch #\newline) 'whitespace)
            ((eq ch #\space) 'whitespace)
            ((eq ch #\#) 'ntmacro)
            ((find-in ch "~!$%&*+-./:<=>?@[]^_{}") 'constituent)
            ((find-in ch "abcdefghijklmnopqrstuvwxyz") 'constituent)
            ((find-in ch "ABCDEFGHIJKLMNOPQRSTUVWXYZ") 'constituent)
            ((find-in ch "0123456789") 'constituent)
            ((find-in ch "'();`\"") 'tmacro)
            (:t (raise "read: syntax-type not mapped" ch)))))
         (fixnump (:lambda (str)
           (cond
            ((zerop (length str)) :nil)
            (:t (block :nil
                  (svmapc
                   (:lambda (el)
                     (when (null (find-in ch "0123456789")) (return :nil)))
                   str)
                  :t)))))
         (comment (:lambda (src)
           (while
             (:lambda ()
                (let ((ch (read-char src)))
                 (cond
                  ((null ch) (raise "end of file in comment" :nil))
                  ((eq ch #\newline) :nil)
                  (:t :t)))))
           (cons :comment :nil)))
         (ws-until-eof (:lambda (src)
           (while
             (:lambda ()
               (let ((ch (read-char src)))
                 (cond
                  ((null ch) :nil)
                  ((eq (syntax-type ch) 'whitespace) :t)
                  (:t (unread-char ch src) :nil))))))))

    (ws-until-eof src)
    ;;; check here for eof
    (let ((ch (read-char src)))
      (cond
       ((null ch) (cons 'eof :nil))
       ((eq (syntax-type ch) 'constituent) ;; symbol/number
        (let ((str (make-output-string "")))
          (princ ch str)
          (while
            (:lambda ()
              (let ((ch (read-char src)))
                (cond
                 ((null ch) :nil) 
                 ((eq (syntax-type ch) 'constituent) (princ ch str) :t)
                 (:t (unread-char ch src) :nil)))))
          (let ((token (get-output-string-stream str)))
            (if (fixnump token)
              (cons :fixnum token)
              (cons :symbol token)))))
       ((eq (syntax-type ch) 'tmacro)  ;; quote/list/string/comment/backquote
        (cond
         ((eq ch #\') (list :quote (llc-read src)))
         ((eq ch #\;) (comment src))
         ; ((eq ch #\`) :nil)
         ((eq ch #\")
          (let ((str (make-output-string "")))
            (while
              (:lambda ()
                (let ((ch (read-char src)))
                  (cond
                   ((null ch) (raise "early eof in string" :nil) :nil)
                   ((eq ch #\") :nil)
                   (:t (princ ch str) :t))))
            (cons :string (get-output-string-stream str)))))
         ((eq ch #\()       ; list/cons
          (let ((el (llc-read src)))
            (cond
             ((eq (car el) 'eol) :nil)
             (:t (list el (llc-read src))))))
         ((eq ch #\)) (cons :eol :nil))
         (:t (raise "unimplemented terminating macro" ch) :nil)))
       ((eq (syntax-type ch) 'ntmacro)     ;; non-terminating macro
        (cond
         ((eq ch #\#)    ;; sharpsign syntax
          (let ((ch (read-char src)))
            (cond
             ((null ch) (raise "early eof in sharpsign syntax" :nil) :nil)
             ((eq ch #\() (cons :vector :nil))
             (:t (raise "unimplemented sharpsign syntax" ch) :nil))))
         (:t (raise "unimplemented nonterminating macro" ch) :nil)))
       (:t (princ type :nil) (terpri :nil) :nil)))))
