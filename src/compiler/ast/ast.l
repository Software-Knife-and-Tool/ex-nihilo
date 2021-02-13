;;;
;;; ast
;;;

#| gyre syntax
  (:liter form)
  (:list form)
  (:vector form)
  (:fcall form)
  (:scall form)
|#

(defun ast (src)
  (compiler:lex src))
