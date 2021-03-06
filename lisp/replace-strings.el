;; The replace strings table
;; =============================
(setq my-replace-strings-table
      (list 
       '("ThisString" . "ByThisOne" )
       '("1" . "one")
       '("2" . "two")
       '("3+" . "three...")
       '("\\(one\\)+" . "one ...")
       )
      )


;; Log function 
;; =============
;;(setq m (mark-marker))
(setq m nil)
(setq level 0)

(defun my-log (string &optional no-cr only-message)
  (setq indent (make-string level ? ))
  ( if only-message
      nil
    (princ (format "%s%s" indent string) m)
    (if (not no-cr)  (princ "\n" m)))
  (message string)      
  )

(defun init-output-buffer ()
  ;;(interactive)
  ;;(save-excursion
	  (setq m (set-marker (make-marker)
                        (buffer-size "*Replace-strings*")
                        (get-buffer-create "*Replace-strings*")))

  ;;  )
)


;; implementation using existing function
;; ========================================
(defun my-replace-strings2 ()
  (interactive)
  (format-replace-strings my-replace-strings-table)
  )

;; new implementation 
;; ========================================

;; public entry point
(defun my-replace-strings () 
  (interactive)
  (save-excursion
    (my-replace-strings-internal my-replace-strings-table)
    )
  )

;; private function
(defun my-replace-strings-internal (replace-strings-table) 

  ;; Prepare buffer for rich log
  (init-output-buffer)

  (let ((local-replace-table replace-strings-table))

    ;; loop on table items
    (while (setq item (car local-replace-table))
      
      ;; trace
      (my-log (format "replace '%s' by '%s': in progress ..." 
                      (car item) (cdr item)) t t)
      
      ;; loop for one item replacement
      (goto-line 1)
      (setq replace-count 0)
      (while (search-forward-regexp (car item) nil t)
        (replace-match (cdr item))
        (setq replace-count (+ 1 replace-count)) 
        )
      
      ;; trace
      (my-log (format "replace '%s' by '%s': %s done" 
                      (car item) (cdr item) replace-count))
      
      ;; next item
      (setq local-replace-table (cdr local-replace-table))
      )
    
    )
  )
  



;;(debug-on-entry 'multi-replace)

