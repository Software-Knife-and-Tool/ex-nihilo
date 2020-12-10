(:defsym recur (:macro (name lambda :rest body)
  (list :defsym 
