

; Error regular expression
(setq error-list '("^.+ \\([a-zA-Z0-9]+\.txt\\) \\[\\([0-9]+\\)\\]" 1 2)) ; foofoofoo essai1.txt [12]

; File name that contain fullpath of all file (result of shell command "find . ")
(setq files-list-name '"./files_list.txt")


; Parse error line and open file at the good line
(defun my-parse-error ()
  "Parse error and open file on the error line"
  (interactive)
  (beginning-of-line)
  (if (looking-at (nth 0 error-list))
      (progn 
       (let (
	     (error-file-name (match-string 1))
	     (error-line (string-to-int (match-string 2))))
	 (find-file-other-window (my-find-fullpath error-file-name files-list-name))
	 (goto-line error-line)
	 ))
    nil
    )

  )

; Find the full path name of a given file inside a list of files
(defun my-find-fullpath (file-name list-name)
  (save-window-excursion
    (find-file-other-window list-name)
    (goto-line 1)
    (search-forward-regexp (concat "^.*" "/" file-name))    
    (match-string 0)
    )
  )

(setq marker-save-path-output (mark-marker))

(defun mark-save-path-output ()
  (interactive)
  (setq marker-save-path-output (mark-marker))
  )

(defun save-path ()
  (interactive)
  (save-window-excursion)
  (let ((l-file-name (buffer-file-name))
	(l-line-number (line-number)))
    (let ((l-path (format "%s[%d]" l-file-name  l-line-number ))
	  (l-current-line-buffer 
	   (buffer-substring (line-beginning-position) (line-end-position) (current-buffer))))
      (print (format "%s:%s" l-path l-current-line-buffer) marker-save-path-output)
      )
    ) 
  )

    
	

    

	
    
	
    
    
		  