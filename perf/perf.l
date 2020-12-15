;;; time macro
(defmacro perf-time (form)
  (let ((now (:lambda ()
         (let* ((now (runtime))
           (sec (float (nth 0 now)))
           (usec (float/ (float (nth 1 now)) 1000000.0)))
           (float+ sec usec))))
        (now-secs (gensym))
        (start-time (gensym))
        (start-room (gensym))
        (stop-time (gensym))
        (stop-room (gensym))
        (start-used (gensym))
        (stop-used (gensym))
        (result (gensym)))
    
    (list 'let* (list (list now-secs now)
                      (list start-room (list 'heap-info :t))
                      (list start-time (list now-secs))
                      (list result form)
                      (list stop-time (list now-secs))
                      (list stop-room (list 'heap-info :t))
                      (list start-used
                            (list 'fixnum- (list 'mu.vec-ref start-room 1)
                                           (list 'mu.vec-ref start-room 3)))
                      (list stop-used
                            (list 'fixnum- (list 'mu.vec-ref stop-room 1)
                                           (list 'mu.vec-ref stop-room 3))))

          (list 'cons (list 'float- stop-time start-time) (list 'fixnum- stop-used start-used)))))

;;; report
(defun perf-report (log)
  (let* ((functions '("gc"
                      "canon:mapc"
                      "canon:mapcar"
                      "mu.foldl"
                      "mu.foldr"
                      "mu.mapcar"
                      "mu.mapc"
                      "core.acons"
                      "core.constantp"
                      "core.eql"
                      "core.pairp"
                      "core.sequencep"
                      "core.schar"
                      "core.stringp"
                      "core.string="
                      "core.vector"
                      "core.vector-to-list"))
         (rep (:lambda (log which)
           (let ((log-temp (fmt :nil "~A.tmp" log)))
             (system (fmt :nil "echo \"(\" > ~A" log-temp))
             (if (null which)
                 (system (fmt :nil "cat ~A >> ~A" log log-temp))
               (system (fmt :nil "grep ~A ~A >> ~A" which log log-temp)))
             (system (fmt :nil "echo \")\" >> ~A" log-temp))
  
             (let* ((logf (open-input-file log-temp))
                    (times (read logf))
                    (ntimes (list-length times))
                    (sums
                     (mu.foldl
                      (:lambda (acc el)
                               (cons (float+ (car el) (car acc))
                                     (fixnum+ (cdr el) (cdr acc))))
                      (cons 0.0 0)
                      times)))
               (close logf)
               (system (fmt :nil "rm ~A" log-temp))
               (cons
                (float/ (car sums) (float ntimes))
                (fixnum/ (cdr sums) ntimes))))))
         (stat (:lambda (which)
           (let*
               ((stats (rep log which))
                (nused (car stats))
                (secs (cdr stats)))
             (fmt :t "~A: ~A ~A~%" which nused secs)))))
    (mapc (:lambda (fn) (stat fn)) functions))
    ())
