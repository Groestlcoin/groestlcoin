(use-modules (toolchains))

(packages->manifest
 (append
  (let ((target (getenv "HOST")))
    (cond ((string-contains target "-linux-")
           (list (make-groestlcoin-cross-toolchain target)))
          (else '())))))
