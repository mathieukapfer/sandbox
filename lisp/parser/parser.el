

(defconst alphanum "[A-Za-z_0-9:]+")
(defconst sep "[ \t]+")
(defconst sep0 "[ \t]*")
(defconst sep-cr "[ \t\n]+")
(defconst sep0-cr "[ \t\n]*")

(defconst cpp-type
  '( ( 'type-const     . (format-choice
                          (concat "const" sep)
                          ""))
     ( 'type-name      . (concat alphanum sep0))
     ( 'type-modifier  . (format-choice "&" "\\*" sep) )
     ) )

;;(cdr (cdr (cdr cpp-type)))
;;(nconc cpp-type cpp-type)

(defconst cpp-var-declaration
  '( ( 'var-type      . cpp-type )
     ( 'var-name      . (concat alphanum sep0) )
     ) ) 

(defconst cpp-function-declaration
  '( ( 'fct-qualifier    . (format-choice
                            (concat "static" sep)
                            (concat "virtual" sep)
                            ""))
     ( 'ret-const        . (format-choice
                            (concat "const" sep)
                            ""))
     ( 'ret-type         . (concat alphanum sep0) )
     ( 'ret-modifier     . (format-choice "&" "\\*" sep) )
     ( 'fct-name         . (concat alphanum sep0) )
     ( 'fct-param-start  .  "(")
     ( 'fct-param-string .  "[^)]*" ) 
     ( 'fct-param-stop   .  ")" )
     ( 'fct-const        .  (format-choice (concat sep "const") "") )
     ( 'fct-end          .  (concat sep0 (format-choice ";" "{" ) ) )
     ) )

;;(defun push-back (element listname ) 
;;  (setf listname (cons listname element )))

(symbolp (cdr (car cpp-function-declaration)))
(symbolp (cdr (car cpp-var-declaration)))
(car cpp-var-declaration)

(setq expanded (list))
(setq loop-num 0)

(defun expand-syntax(l) 
  (setq expanded (list))
  (setq loop-num 0) 
  (expand-syntax-internal l)
  (nreverse expanded)
  )

(defun expand-syntax-internal (l)
  (let ((l-item (car l)))
    (while l-item
      (if (symbolp (cdr l-item))
          (progn
            (expand-syntax-internal (eval (cdr l-item)))
            (pop l))
        (push (pop l) expanded))
      (setq l-item (car l))           
      ;;(princ (format "%d: " (inc loop-num))) 
      ;;(princ expanded)
      ;;(princ "\n")
      )
    )
  )
      
(expand-syntax cpp-var-declaration)

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

(defun collect-regexp (syntax-alist)
  (loop for str in (mapcar 'cdr syntax-alist)
        collect (if (symbolp str)
                    (collect-regexp (symbol-value str))
                  str ) ) 
  )

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

(defun parser (syntax-alist)
  (interactive)
  (let ((match-string-number 1)
        (res (list))
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


(defun test()
  
  (parser cpp-function-declaration)
  (interactive)
  ;;(parser cpp-type)
  ;;(parser cpp-var-declaration)
  )

(global-set-key (kbd "M-t") 'test) 
;; static const EvseModel * inst(qsdf) const;
;;static EvseModel * inst(qsdf) const;
;; static  &EvseModel * inst(qsdf) const;
;; static * EvseModel * inst(qsdf) const;
;; static int i;
;; int* i;
;; int& i;
;; int * i;
;; int & i;
;; static int &i;

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
;;static const EvseConf * FlushToFile(std::ostream *outputReference = NULL);

    ;; // write configuration file
    ;;EvseConf *FlushToFile(std::ostream *outputReference = NULL);
