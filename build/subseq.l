;;; subseq
(defun subseq (seq start :rest args)
  (unless (sequencep seq) (raise "is not a sequence (subseq)"))
  (unless (fixnump start) (raise "is not a fixnum (subseq)" start))
  (when (minusp start) (raise "start may not be negative (subseq)" start))
  (block :nil
    (when (zerop (length seq)) (return seq))
    (when (fixnum< (length seq) start) (return :nil))
    (let ((sseq (:lambda (start end) seq))
          (nargs (length args))
          (end (car args)))
      (cond
       ((zerop nargs) (sseq start (length list)))
       ((and (eq 1 nargs) (null end))
        (sseq start (length list)))
       ((and (eq 1 nargs) (fixnump end))
          (when (minusp end) (raise "end may not be negative (subseq)" end))
          (sseq start end))
       (:t (raise "end botch (subseq)" args))))))
