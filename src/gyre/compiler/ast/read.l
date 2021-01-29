;;;
;;; read
;;;
(defun gyre:read (src)
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
         (fixp (:lambda (str)
           (cond
            ((zerop (length str)) :nil)
            (:t (block :nil
                  (mu::vector-mapc
                   (:lambda (ch)
                     (when (null (find-in ch "0123456789")) (return :nil)))
                   str)
                  :t)))))
         (comment (:lambda (src)
           (while 
               (let ((ch (read-char src)))
                 (cond
                  ((null ch) (raise "end of file in comment" :nil))
                  ((eq ch #\newline) :t)
                  (:t :nil))))
           (cons :comment :nil)))
         (ws-until-eof (:lambda (src)
           (while 
               (let ((ch (read-char src)))
                 (cond
                  ((null ch) :nil)
                  ((eq (syntax-type ch) 'whitespace) :t)
                  (:t (unread-char ch src) :nil)))))))
    (ws-until-eof src)
    (let ((ch (read-char src)))
      (cond
       ((null ch) (cons 'eof :nil))        ;; eof
       ((eq (syntax-type ch) 'constituent) ;; symbol/number
        (let ((str (open-output-string "")))
          (princ ch str)
          (while
              (let ((ch (read-char src)))
                (cond
                 ((null ch) :nil)
                 ((eq (syntax-type ch) 'constituent) (princ ch str) :t)
                 (:t (unread-char ch src) :nil))))
          (let ((token (get-output-stream-string str)))
            (fmt :t "symbol: ~A~%" token)
            (cons ((fixp token) :fixnum :symbol) token))))
       ((eq (syntax-type ch) 'tmacro)      ;; quote/list/string/comment/backquote
        (cond
         ((eq ch #\') (list :quote (gyre:read src)))
         ((eq ch #\;) (comment src))
         ; ((eq ch #\`) :nil)
         ((eq ch #\")
          (let ((str (open-output-string "")))
            (while
                (let ((ch (read-char src)))
                  (cond
                   ((null ch) (raise "early eof in string" :nil) :nil)
                   ((eq ch #\") :nil)
                   (:t (princ ch str) :t))))
            (cons :string (get-output-string-stream str))))
         ((eq ch #\()                      ;; list/cons
          (let ((el (gyre:read src)))
            (cond
             ((eq (car el) :eol) :nil)
             (:t (list el (gyre:read src))))))
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
