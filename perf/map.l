(defconstant lst '(0 1 2 3 4 5 6 7 8 9
                   0 1 2 3 4 5 6 7 8 9
                   0 1 2 3 4 5 6 7 8 9
                   0 1 2 3 4 5 6 7 8 9
                   0 1 2 3 4 5 6 7 8 9
                   0 1 2 3 4 5 6 7 8 9
                   0 1 2 3 4 5 6 7 8 9
                   0 1 2 3 4 5 6 7 8 9
                   0 1 2 3 4 5 6 7 8 9
                   0 1 2 3 4 5 6 7 8 9))

(fmt :t "~A ;;; 1000 canon:mapc~%" (perf-time (canon:mapc identity lst)))
(fmt :t "~A ;;; 1000 canon:mapcar~%" (perf-time (canon:mapcar identity lst)))
(fmt :t "~A ;;; 1000 mu.foldl~%" (perf-time (mu.foldr (:lambda (el acc) acc) :nil lst)))
(fmt :t "~A ;;; 1000 mu.foldr~%" (perf-time (mu.foldl (:lambda (el acc) acc) :nil lst)))
(fmt :t "~A ;;; 1000 mu.mapc~%" (perf-time (mu.mapc identity lst)))
(fmt :t "~A ;;; 1000 mu.mapcar~%" (perf-time (mu.mapcar identity lst)))
