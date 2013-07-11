(defun insert-virg-nexus-header(&optional arg)
  "Inserts copyright header for a file in the VIRG distribution"
  (interactive "P")
  (let ((cpyright
         (concat
          (when (boundp 'user-full-name)
            (concat "/**\n"
                    " * @file " (file-name-nondirectory (buffer-file-name)) "\n"
                    " *\n"
                    " * This file is part of the IYTE Visual Intelligence Research Group Software Library\n"
                    " *\n"
                    " * Copyright (C) " (format-time-string "%Y") " Izmir Institute of Technology. All rights reserved.\n"
                    " *\n"
                    " * @author " user-full-name "\n"))
          (when (boundp 'user-mail-address)
            (concat " *\n * Contact " user-mail-address " for comments and bug reports.\n"))
          " *\n"
          " */\n"))
        (temp-position))

    (goto-char (point-min))

    ;; If this is a script, put the copyright after the first line
    (when (re-search-forward "^#!" nil t)
      (beginning-of-line)
      (next-line 1))

    (let ((start (point)))
      (insert cpyright)
      (delete-trailing-whitespace))

    (if arg
        (let ((upcase-filename (upcase (file-name-nondirectory (buffer-file-name))))
              (dot-position)
              (date-string))
          (setq dot-position (string-match "\\." upcase-filename))
          (aset upcase-filename dot-position ?_)
          (insert "#ifndef VIRG_NEXUS_" upcase-filename "\n")
          (insert "#define VIRG_NEXUS_" upcase-filename "\n")
          (insert "\n#include \"virg/nexus/nx_config.h\"\n")
          (insert "\n__NX_BEGIN_DECL\n\n")
          (setq temp-position (point))
          (insert "\n\n__NX_END_DECL\n\n")
          (insert "#endif")
          (goto-char temp-position)))
    ;; (insert "\nnamespace virg {\n\n")
    ;; (setq temp-position (point))
    ;; (insert "\n\n} // namespace virg\n")
    (goto-char temp-position)))
