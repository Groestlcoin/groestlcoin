(use-modules (gnu packages bison)
             ((gnu packages compression) #:select (xz zip))
             (gnu packages gawk)
             ((gnu packages installers) #:select (nsis-x86_64))
             (gnu packages ninja)
             (gnu packages pkg-config)
             ((gnu packages python) #:select (python-minimal))
             ((gnu packages python-web) #:select (python-aiohttp))
             ((gnu packages python-xyz) #:select (python-lief))
             ((guix utils) #:select (substitute-keyword-arguments))
             ((guix packages) #:select (package package-arguments package-input-rewriting/spec))
             (toolchains))

(define python-aiohttp-no-tests
  (package
    (inherit python-aiohttp)
    (arguments
     (substitute-keyword-arguments (package-arguments python-aiohttp)
       ((#:tests? _ #t) #f)))))

;; Rewrite python-aiohttp dependencies to skip its test suite.
(define (rewrite-aiohttp-inputs p)
  ((package-input-rewriting/spec
    `(("python-aiohttp" .
       ,(lambda (_p) python-aiohttp-no-tests))))
   p))

;; python-lief and nsis-x86_64 transitively pull in packages whose
;; tests fail when building natively on riscv64:
;; - python-lief: python-psutil, python-pytest-xprocess, python-sh
;; - nsis-x86_64: python-psutil
(define (package-without-tests p)
  (package
    (inherit p)
    (arguments
     (substitute-keyword-arguments (package-arguments p)
       ((#:tests? _ #t) #f)))))

(define python-lief-no-riscv64-failing-tests
  ((package-input-rewriting/spec
    `(("python-psutil" . ,package-without-tests)
      ("python-pytest-xprocess" . ,package-without-tests)
      ("python-sh" . ,package-without-tests)))
   python-lief))

(define nsis-x86_64-no-riscv64-failing-tests
  ((package-input-rewriting/spec
    `(("python-psutil" . ,package-without-tests)))
   nsis-x86_64))

(packages->manifest
 (map rewrite-aiohttp-inputs
  (append
   (list ;; Compression and archiving
         xz
         ;; Build tools
         ninja
         ;; Packaging scripts
         python-minimal ;; 3.12
         ;; Python packages
         python-aiohttp-no-tests
         python-lief-no-riscv64-failing-tests)
   (let ((target (getenv "HOST")))
     (cond ((string-suffix? "-mingw32" target)
            (list nsis-x86_64-no-riscv64-failing-tests
                  zip))
           ((string-contains target "-linux-")
            (list bison
                  gawk
                  (make-groestlcoin-cross-toolchain target)
                  pkg-config))
           ((string-contains target "darwin")
            (list zip))
           (else '()))))))
