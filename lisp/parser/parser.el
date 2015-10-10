

(defconst alphanum "[A-Za-z_0-9:]+")
(defconst sep "[ \t]+")
(defconst sep0 "[ \t]*")
(defconst sep-cr "[ \t\n]+")
(defconst sep0-cr "[ \t\n]*")

(defconst cpp-type
  '( ( type-const     . (format-choice
                          (concat "const" sep)
                          ""))
     ( type-name      . (concat alphanum sep0))
     ( type-modifier  . (concat (format-choice "&" "\\*" "") sep0))
     ) )

;;(cdr (cdr (cdr cpp-type)))
;;(nconc cpp-type cpp-type)

(defconst cpp-var-declaration
  '(
    ( var-qualifier . (format-choice
                        (concat "static" sep)
                        ""))
     ( var-type      . cpp-type )
     ( var-name      . (concat alphanum sep0) )
     ( var-end       . (format-choice "," ";" ) )
     ) ) 

(defconst cpp-function-declaration
  '( ( fct-qualifier    .  (format-choice
                             (concat "static" sep)
                             (concat "virtual" sep)
                             ""))
     ( type             .  cpp-type )
     ( fct-name         .  (concat alphanum sep0) )
     ( fct-param-start  .  "(")
     ( fct-param-string .  "[^)]*" ) 
     ( fct-param-stop   .  ")" )
     ( fct-const        .  (format-choice (concat sep "const") "") )
     ( fct-end          .  (concat sep0 (format-choice ";" "{" ) ) )
     ) )

;;(defun push-back (element listname ) 
;;  (setf listname (cons listname element )))

(symbolp (cdr (car cpp-function-declaration)))
(symbolp (car (car cpp-var-declaration)))
(car cpp-var-declaration)

;;; syntax expander 
;;; ================
;; global variable
(setq expanded (list))
(setq loop-num 0)

;; entry point
(defun expand-syntax(l) 
  (setq expanded (list))
  (setq loop-num 0) 
  (expand-syntax-recursif l)
  (nreverse expanded)
  )

;; recursif call
(defun expand-syntax-recursif (l)
  (let ((l-item (car l)))
    (while l-item
      (if (symbolp (cdr l-item))
          (progn
            (expand-syntax-recursif (eval (cdr l-item)))
            (pop l))
        (push (pop l) expanded))
      (setq l-item (car l))           
      )
    )
  )
      
(expand-syntax cpp-var-declaration)


;; helper
;; ======
(defmacro inc (var)
  (list 'setq var (list '1+ var)))

(defun format-choice (&rest str-list)
  (concat "\\(?:" (mapconcat 'identity str-list "\\|" ) "\\)" )
  )

(defun format-group (str)
  (concat "\\(" (eval str) "\\)" )
  )

(listp (car cpp-type))
(symbol-value 'cpp-type)

;;(defun collect-regexp (syntax-alist)
;;  (loop for str in (mapcar 'cdr syntax-alist)
;;        collect (if (symbolp str)
;;                    (collect-regexp (symbol-value str))
;;                  str ) ) 
;;  )

(defun collect-regexp (syntax-alist)
  (loop for str in (mapcar 'cdr syntax-alist)
        collect str )) 

(defun collect-key (syntax-alist)
  (loop for str in (mapcar 'car syntax-alist)
        collect str))

(defun build-regexp (syntax-alist)
  (mapconcat 'format-group (collect-regexp syntax-alist) "")
  )

;;apply

;; test
;; (collect-key cpp-type)
;; (collect-regexp cpp-type)
;; (collect-regexp cpp-var-declaration)
;; (build-regexp cpp-type)
;; (build-regexp cpp-var-declaration)
;; (build-regexp cpp-function-declaration)
;; (eval (concat "rr"))

;; parser entry point
;; ==================
(defun parser (syntax-alist)
  (interactive)
  (parser-expanded-syntax (expand-syntax syntax-alist) ) 
  )

(defun parser-expanded-syntax (syntax-alist)
  (interactive)
  (let ((match-string-number 1)
        (res (list)) ;; an empty list to collect 
        (list-of-key (collect-key syntax-alist)))
    (if (re-search-forward (build-regexp syntax-alist))
        (progn
          (while (match-string match-string-number)
            (push (cons (pop list-of-key)
                        (match-string-no-properties match-string-number))
                  res )
            (inc match-string-number))
          (princ (nreverse res))
          ;;(res)
          )
      )
    )
  )

(defun debug-syntax-internal (str)
  (concat
   ":'" str "':'"
   (if (looking-at str)
       (progn
         (goto-char (match-end 0))
         (match-string 0)) 
     "not found")
   "'\n"
   )
  )
  

(defun debug-syntax(syntax-alist)
  (interactive)
  (message
   (loop for str in (mapcar 'cdr (expand-syntax syntax-alist))
         concat (debug-syntax-internal (eval str))
         )
   )
  )

(defun debug-syntax-2(syntax-alist)
  (interactive)
  (message
   (loop for str-alist in (expand-syntax syntax-alist)
         concat (concat "search " (symbol-name (car str-alist))
                        (debug-syntax-internal (eval (cdr str-alist)))
                        )
         )
   )
  )


(defun test()
  (interactive)
  ;;(parser cpp-type)
  ;;(parser cpp-var-declaration)
  ;;(debug-syntax cpp-var-declaration)
  ;;(parser cpp-function-declaration)
  (debug-syntax-2 cpp-function-declaration)
  )

(global-set-key (kbd "M-t") 'test) 
;; static const EvseModel * inst(qsdf) const;
;; static EvseModel * inst(qsdf) const;
;; static  &EvseModel * inst(qsdf) const;
;; static * EvseModel * inst(qsdf) const;
;; int* i;
;; int& i;
;; int * i;
;; int & i;
;; int *i;
;; int &i;
;; static int &i;
;; static int i;

    ;; // Singleton
    ;; static EvseConf *inst();
    ;; static void deleteInst(void);

    ;; // Register observer to update configuration 
    ;; void RegisterUpdateCallback(EvseConfUpdateCallback *observer) const;

    ;; // write configuration file
    ;;  void FlushToFile(std::ostream *outputReference = NULL);

    ;; // write configuration file
    ;; virtual const EvseConf * FlushToFile(std::ostream *outputReference = NULL);

;; // write configuration file
;;static const EvseConf *FlushToFile(std::ostream *outputReference = NULL);

    ;; // write configuration file
    ;;EvseConf *FlushToFile(std::ostream *outputReference = NULL);
