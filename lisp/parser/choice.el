;; (defmacro choice (&rest items)
;;   (list 'format-choice (list 'list items))

;; (macroexpand '(choice "a" "b" ) )

(defun format-choice (&rest str-list)
  (concat "\\(?:"
          (mapconcat 'identity str-list "\\|" )
          "\\)"
          )
  )

(defconst syntax-cpp-type
  '( ( 'qualifier . "\\(?:static\\|const\\|\\)" )
     ( 'name      . alphanum )
;;     ( 'modifier  . (choice  "&" "*" ))))
     ( 'modifier  . (format-choice "&" "*" ) ) ) )


(collect-name syntax-cpp-type)
(collect-regexp syntax-cpp-type)
(mapconcat 'format-group (collect-regexp syntax-cpp-type) sep)
