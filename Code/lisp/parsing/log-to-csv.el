;;; Main entry point is "log-to-csv()"
;;;
;;; This function parse a log like this:
;;;
;;; 0230|Io_CPW:558                0595.3507 [INFO ][IO][CPW]             GetCPW1Voltage: 9452 milivolts (647)
;;; 0230|Io_CPW:268                0597.5592 [INFO ][IO][CPW]             UpdateEVStatePresenceM12 KO: last value is:true
;;; 0230|Io_CPW:558                0595.3377 [INFO ][IO][CPW]             GetCPW1Voltage: 11497 milivolts (787)
;;;
;;; And produce in a buffer named 'foo' a output in csv format
;;; in order to be openned by Excel like application:
;;;
;;; 0595.3507; ; ;9452
;;; 0597.5592; ;true
;;; 0595.3377; ; ;11497

(defconst log-to-csv-separator ";" )

(setq log-to-csv-timestamp "^.* \\([[:digit:]]+\.[[:digit:]]+\\) \\[.*")

(setq log-to-csv-regexp-list
      (list
       ;; ( "name_of_column"  \\(balise-in-log\\) ... \\(value\\) )
   '("PresenceM12" . "UpdateEVStatePresenceM12 KO: last value is:\\(true\\|false\\)")
   '("CPW1Voltage" . "GetCPW1Voltage: \\([[:digit:]]+\\) milivolts")
   '("CpwErrorCounter" . "IncrementCommunicationErrorCounter exits (counter: \\([[:digit:]]+\\)")
   )
  )

;;; create void column
(defun log-to-csv-make-indent (nb-col)
  (setq indent-string "")
  (dotimes (i nb-col indent-string)
    (setq indent-string (concat indent-string " " log-to-csv-separator))
    )
  )

;;; create index
(defun log-to-csv-create-column-header (column-list)
  (prin1 (get 'column-list 'list-order) (get-buffer "foo"))
  )


;;; main entry point
(defun log-to-csv()
  (interactive)
  ;; prepare output buffer
  (generate-new-buffer "foo")
  ;; init
  (let ((level 0) (first-timestamp 0))
    ;; step 1: parse each log line
    (while (< (point) (point-max))
      (setq level 0)
      (dolist (expr log-to-csv-regexp-list)
        (setq level (+ level 1))
        (if (looking-at (concat log-to-csv-timestamp (cdr expr)))
            (progn
              (setq current-timestamp (string-to-number (match-string 1)))
              ;; save time origin
              (if (eq 0 first-timestamp)
                  (setq first-timestamp current-timestamp))
              ;; create a csv line
              (princ
               (concat (match-string 1) log-to-csv-separator;; timesamp
                       (number-to-string (- current-timestamp first-timestamp))
                       log-to-csv-separator ;; delta timestamps from origin
                       (log-to-csv-make-indent level) ;; adding void column
                       (match-string 2) ;; value
                       "\n")
               (get-buffer "foo")
               )
              (return)
            )
          )
        ) ;; end dolist
      (forward-line 1)
      ) ;; end while
    ) ;; end let
    ;; step 2: adding column header
    ;;;;(goto-char (point-min))
  (with-current-buffer "foo"
    (goto-char (point-min))
    (princ (concat "time;delta" ) (get-buffer "foo"))
    (dolist (expr log-to-csv-regexp-list)
      (princ
       (concat log-to-csv-separator (car expr))
       (get-buffer "foo")
       )
      ) ;; end dolist
    (princ "\n" (get-buffer "foo"))
    );; end with-current-buffer
  ) ;; end defun

;; eval buffer & launch test
(defun log-to-csv-test()
  (interactive)
  (save-current-buffer
    (eval-buffer "log-to-csv.el")
    (set-buffer "foo")
    (erase-buffer)
    (set-buffer "test.txt")
    (goto-char (point-min))
    (log-to-csv)
    )
  )
