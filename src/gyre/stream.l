;;; with-open-stream direction stream-type init-string fn
(defun with-open-stream (direction stream-type init-string fn)
  (cond
    ((and (null (eq direction :input))
          (null (eq direction :output))
          (raise "with-open-stream: direction must be a keyword (:input|:output)" direction))
     :nil)
    ((and (null (eq stream-type :file))
          (null (eq stream-type :string))
          (raise "with-open-stream: stream-type must be a keyword (:file|:string)" stream-type))
     :nil)
    ((null (stringp init-string))
     (raise "with-open-stream: init-string must be a string" init-string))
    ((null (functionp fn))
     (raise "with-open-stream: fn must be a function" fn))
    (:t (cond
         ((eq stream-type :file)
          (cond
           ((eq direction :input) (fn (open-input-file init-string)))
           ((eq direction :output) (fn (open-output-file init-string)))
         ((eq stream-type :string)
          (cond
           ((eq direction :input) (fn (open-input-string init-string)))
           ((eq direction :output)
            (let ((stream (open-output-string "")))
              (mu.print init-string stream :nil)
              (fn stream)))))))))))
