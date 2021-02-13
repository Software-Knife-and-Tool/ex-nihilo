;;; mapc
(defun common:mapc (fn :rest lists)
  (mu:block ()
    (letf* ((map-cars (lists)
              (mu:mapcar
               (:lambda (lst)
                 (when (null (car (car lst))) (mu:return :nil))
                 (car (car lst)))
              lists))
            (map-cdrs (lists)
               (mu:mapcar
                (:lambda (lst) (mu:maplist identity lst))
                lists))
            (mcar (cdr-list)
              (mu:apply fn (map-cars cdr-list))
              (mcar (mu:mapcar cdr cdr-list))))
       (mcar (map-cdrs lists))))
  (car lists))

;;; mapcar
(defun common:mapcar (fn :rest lists)
  (letf* ((map-cars (lists)
            (mu:block ()
              (mu:mapcar
               (:lambda (lst)
                  (when (null (car (car lst))) (mu:return :nil))
                  (car (car lst)))
               lists)))
          (map-cdrs (lists)
             (mu:mapcar
              (:lambda (lst) (mu:maplist identity lst))
              lists))
          (mcar (cdr-list)
            (let ((argl (map-cars cdr-list)))
              (if (null argl)
                  :nil
                (cons (fn argl) (mcar (mu:mapcar cdr cdr-list)))))))
      (mcar (map-cdrs lists))))

;;; mapl
(defun common:mapl (fn :rest lists)
  (mu:block :nil
    (letf* ((map-cars (lists)
              (mu:mapcar
                (:lambda (lst)
                  (when (null (car lst)) (mu:return :nil))
                  (car lst))
                lists))
            (map-cdrs (lists)
              (mu:mapcar
                (:lambda (lst) (mu:maplist identity lst))
                lists))
            (ml (cdr-list)
               (mu:apply fn (map-cars cdr-list))))
      (ml (map-cdrs lists))))
  (car lists))

;;; maplist
(defun common:maplist (fn :rest lists)
  (letf* ((map-cars (lists)
            (mu:block ()
              (mu:mapcar
               (:lambda (lst)
                  (when (null (car lst)) (mu:return :nil))
                  (car lst))
               lists)))
          (map-cdrs (lists)
              (mu:mapcar
                (:lambda (lst) (mu:maplist identity lst))
                lists))
          (mlist (cdr-list)
            (let ((argl (map-cars cdr-list)))
              (if (null argl)
                  :nil
                (cons (mu:apply fn argl) (mlist (mu:mapcar cdr cdr-list)))))))
      (mlist (map-cdrs lists))))

;;; dotimes/dolist
(defmacro dotimes (sym countf :rest body) 
  (unless (and (symbolp sym) (null (keywordp sym)))
    (raise "is not a non-keyword symbol (dotimes)" sym))
  (let ((count-form countf))
    (unless (fixnump count-form) (raise "is not a fixnum (dotimes)" count-form))
    (let ((bodyf (list* :lambda (list sym) body)))
      (list 'block :nil
        (list 'let (list (list 'loopf (list :lambda (list 'fn 'n)
                            (list 'if (list 'eq count-form 'n)
                              :nil
                              (list 'progn (list bodyf 'n)
                                 (list 'fn 'fn (list 'fixnum+ 1 'n)))))))
              (list 'loopf 'loopf 0))
        :nil))))

(defmacro dolist (sym listf :rest body)
  (unless (and (symbolp sym) (null (keywordp sym)))
    (raise "is not a non-keyword symbol (dolist)" sym))
  (let ((list-form listf))
    (unless (listp list-form) (raise "is not a list (dolist)" list-form))
    (let ((bodyf (list* :lambda (list sym) body)))
      (list 'block :nil (list 'mu:mapc bodyf list-form)))))

;;; copy-list
(defun copy-list (list) (mu:mapcar identity list))

;;; last
(defun last (list n)
  (unless (listp list) (raise "is not a list (last)" list))
  (unless (fixnump n) (raise "is not a fixnum (last)" n))
  (when (fixnum< n 0) (raise "is not a positive fixnum (last)" n))
  (cond
   ((null list) ())
   ((zerop n) ())
   (:t
    (block :nil
      (letf ((last (n l)
        (cond
         ((eq n (mu:length list)) (return l))
         (:t (last (fixnum+ n 1) (cdr l))))))
        (last (fixnum- (fixnum- (mu:length list) n) 1) list))))))

;;; butlast
(defun butlast (list n)
  (unless (listp list) (raise "is not a list (butlast)" list))
  (unless (fixnump n) (raise "is not a fixnum (butlast)" n))
  (when (fixnum< n 0) (raise "is not a positive fixnum (butlast)" n))
  (cond
   ((null list) ())
   ((zerop n) list)
   (:t
    (block :nil
      (letf ((last (n l ll)
        (cond
         ((eq n (mu:length list)) (return (reverse ll)))
         (:t (last (fixnum+ n 1) (cdr l) (cons (car l) ll))))))
        (last (fixnum- (fixnum- (mu:length list) n) 1) list ()))))))
