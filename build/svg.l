;;;
;;; svg struct
;;;
(defstruct svg
  (stream standard-output :stream)
  (stroke "black" :string)
  (width 1 :fixnum)
  (view-w 1600 :fixnum)
  (view-h 800 :fixnum)
  (view-x (:lambda (svg x) (fixnum+ x (fixnum/ (svg-view-w svg) 2))) :func)
  (view-y (:lambda (svg y) (fixnum+ y (fixnum/ (svg-view-h svg) 2))) :func)

  (open (:lambda (svg)
           (fmt (svg-stream svg) "<!DOCTYPE html>~%")
           (fmt (svg-stream svg) "<html>~%")
           (fmt (svg-stream svg) "<body>~%")
           (fmt (svg-stream svg) "<svg width=\"~A\" height=\"~A\">~%"
                (svg-view-w svg) (svg-view-h svg)))
        :func)
  (close (:lambda (svg)
            (fmt (svg-stream svg) "</svg>~%")
            (fmt (svg-stream svg) "</body>~%")
            (fmt (svg-stream svg) "</html>~%")
            (close (svg-stream svg)))
         :func)
  
  (crt (:lambda (r theta) (cons r theta)) :func)
  (cxy (:lambda (x y) (cons x y)) :func)

  (translate (:lambda (svg length rc)
                ((svg-crt svg) (float+ (float length) (float (car rc))) (cdr rc)))
             :func)
  (rotate (:lambda (svg theta rc)
             ((svg-crt svg) (car rc) (float+ (float theta) (float (cdr rc)))))
          :func)
  (polar-to-rect (:lambda (svg pc)
                    (let ((r (float (car pc)))
                          (theta (float- (float (cdr pc)) 90.0)))
                      ((svg-crt svg)
                       (float* r (cos theta))
                       (float* r (sin theta)))))
                 :func)
  (rect-to-polar (:lambda (svg rc)
                    (let ((x (float (car rc)))
                          (y (float (cdr rc)))
                          (atan (:lambda (theta) (pow (tan (float theta)) -1.0))))
                      ((svg-cxy svg)
                       (sqrt (float+ (float* x x) (float* y y)))
                       (atan (float/ y x)))))
                 :func)
  (polar-line (:lambda (svg pc length)
                 (let ((start ((svg-polar-to-rect svg) svg pc))
                       (end ((svg-polar-to-rect svg) svg ((svg-translate svg) length pc))))
                   ((svg-line svg)
                    svg
                    (fixnum (car start))
                    (fixnum (cdr start))
                    (fixnum (car end))
                    (fixnum (cdr end)))
                   end))
              :func)
  (circle (:lambda (svg x y r fill)
                 (let ((x-0 ((svg-view-x svg) svg x))
                       (y-0 ((svg-view-y svg) svg y)))
                     (cond
                      ((null fill)
                       (fmt (svg-stream svg)
                            "<circle cx=\"~A\" cy=\"~A\" r=\"~A\" style=\"stroke:~A;stroke-width:~A\" fill=\"none\"/>~%"
                            x-0 y-0 r (svg-stroke svg) (svg-width svg)))
                      (:t
                       (fmt (svg-stream svg)
                            "<circle cx=\"~A\" cy=\"~A\" r=\"~A\" style=\"stroke:~A;stroke-width:~A\" fill=\"~A\"/>~%"
                            x-0 y-0 r (svg-stroke svg) (svg-width svg) fill)))))
          :func)
  (ellipse (:lambda (svg p1 r line-width color frame)
                    (let ((sc1 (svg-rc rc1 frame))
                          (sc2 (svg-rc rc2 frame)))
                      (fmt (frame-stream frame)
                           "<line x1=\"~A\" y1=\"~A\" x2=\"~A\" y2=\"~A\" style=\"stroke:~A;stroke-width:~A\" />~%"
                           (x sc1) (y sc1) (x sc2) (y sc2) color line-width)))
           :func)
  (polygon (:lambda (svg p1 r line-width color frame)
                    (let ((sc1 (svg-rc rc1 frame))
                          (sc2 (svg-rc rc2 frame)))
                      (fmt (frame-stream frame)
                           "<line x1=\"~A\" y1=\"~A\" x2=\"~A\" y2=\"~A\" style=\"stroke:~A;stroke-width:~A\" />~%"
                           (x sc1) (y sc1) (x sc2) (y sc2) color line-width)))
          :func)
  (line (:lambda (svg x y x1 y1)
                 (let ((x-0 ((svg-view-x svg) svg x))
                       (y-0 ((svg-view-y svg) svg y))
                       (x-1 ((svg-view-x svg) svg x1))
                       (y-1 ((svg-view-y svg) svg y1)))
                   (fmt (svg-stream svg)
                        "<line x1=\"~A\" y1=\"~A\" x2=\"~A\" y2=\"~A\" style=\"stroke:~A;stroke-width:~A\"/>~%"
                        x-0 y-0 x-1 y-1 (svg-stroke svg) (svg-width svg))))
        :func)
  (rect (:lambda (svg x y width height fill)
                 (let ((x-0 ((svg-view-x svg) svg x))
                       (y-0 ((svg-view-y svg) svg y)))
                   (cond
                    ((null fill)
                     (fmt (svg-stream svg)
                          "<rect x=\"~A\" y=\"~A\" width=\"~A\" height=\"~A\" style=\"stroke:~A;stroke-width:~A\" fill=\"none\"/>~%"
                          x-0 y-0 width height (svg-stroke svg) (svg-width svg)))
                   (:t
                    (fmt (svg-stream svg)
                         "<rect x=\"~A\" y=\"~A\" width=\"~A\" height=\"~A\" style=\"stroke:~A;stroke-width:~A\" fill=\"~A\"/>~%"
                         x-0 y-0 width height (svg-stroke svg) (svg-width svg) fill)))))
        :func))

;;; coordinate pairs
(defun crt (r theta) (cons r theta))
(defun cxy (x y) (cons x y))

(defun translate (length rc)
  (crt (float+ (float length) (float (car rc))) (cdr rc)))

(defun rotate (theta rc)
  (crt (car rc) (float+ (float theta) (float (cdr rc)))))

(defun polar-to-rect (pc)
  (let ((r (float (car pc)))
        (theta (float- (float (cdr pc)) 90.0)))
    (crt (float* r (cos theta))
         (float* r (sin theta)))))

(defun rect-to-polar (rc)
  (let ((x (float (car rc)))
        (y (float (cdr rc))))
    (cxy (sqrt (float+ (float* x x) (float* y y)))
         (atan (float/ y x)))))

(defun polar-line (svg pc length)
  (let ((start (polar-to-rect pc))
        (end (polar-to-rect (translate length pc))))
    ((svg-line svg)
     svg
     (fixnum (car start))
     (fixnum (cdr start))
     (fixnum (car end))
     (fixnum (cdr end)))
    end))

#|
 void drawDragon(Color)(Image!Color img, ref Turtle t, in uint depth,
                       in real dir, in uint step) {
    if (depth == 0)
        return t.forward(img, step);
    t.right(dir);
    img.drawDragon(t, depth - 1, 45.0, step);
    t.left(dir * 2);
    img.drawDragon(t, depth - 1, -45.0, step);
    t.right(dir);
}
|#

(defun dragon (svg order pc)
  (cond
   ((zerop order) (polar-line svg pc 50))
   (:t
    (dragon line (1- order) (rotate 45 pc))
    (dragon line (1- order) (rotate -45 pc)))))

(defun polar-abscissae (svg)
  (polar-line svg (crt 0 0) (svg-view-h svg))
  (polar-line svg (crt 0 90) (svg-view-w svg))
  (polar-line svg (crt 0 180) (svg-view-h svg))
  (polar-line svg (crt 0 270) (svg-view-w svg)))

(:defcon svg (make-svg stream (open-output-file "dragon.html") view-w 1600 view-h 800 width 1))

((svg-open svg) svg)
((svg-rect svg) svg -100 -100 100 100 "black")

(polar-abscissae svg)

((svg-rect svg) svg -100 -100 100 100 "black")
((svg-rect svg) svg 0 0 100 100 "black")
((svg-rect svg) svg 0 -100 100 100 :nil)
((svg-rect svg) svg -100 0 100 100 :nil)
((svg-circle svg) svg 0 0 141 :nil)

;;; (dragon svg 5 (crt 100 270))

((svg-close svg) svg)
(system "open dragon.html")
