
(require 'semantic/bovine/gcc)
         
;; Qt header completion
(setq qt5-base-dir "/usr/local/Qt-5.7.0/include")
(semantic-add-system-include qt5-base-dir 'c++-mode)
(add-to-list 'auto-mode-alist (cons qt5-base-dir 'c++-mode))
(add-to-list 'semantic-lex-c-preprocessor-symbol-file (concat qt5-base-dir "/Qt/qconfig.h"))
(add-to-list 'semantic-lex-c-preprocessor-symbol-file (concat qt5-base-dir "/Qt/qconfig-dist.h"))
(add-to-list 'semantic-lex-c-preprocessor-symbol-file (concat qt5-base-dir "/Qt/qglobal.h"))



;; Header name completion
;; dependencies: provide 'achead:include-directories' with system and project header directories
;; ================================================================================
; let's define a function which initializes auto-complete-c-headers and gets called for c/c++ hooks
;; (defun my:ac-c-header-init ()
;;   (require 'auto-complete-c-headers)
;;   (add-to-list 'ac-sources 'ac-source-c-headers)
;;   (add-to-list 'achead:include-directories qt5-base-dir)
;;   )
;; ;; now let's call this function from c/c++ hooks
;; (add-hook 'c++-mode-hook 'my:ac-c-header-init)
;; (add-hook 'c-mode-hook 'my:ac-c-header-init)


;; (ede-cpp-root-project "Test"
;;                 :name "Test Project"
;;                 :file "makefile"
;;                 :include-path '("/"
;;                                 "/Common"
;;                                 "/Interfaces"
;;                                 "/Libs"
;;                                )
;;                 :system-include-path '("~/exp/include")
;;                 :spp-table '(("isUnix" . "")
;;                              ("BOOST_TEST_DYN_LINK" . "")))
