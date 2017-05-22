```
baumcrypt 1.0 - useful utility to encrypt your home folder Kappa

Options:
-h or --help: Displays this information.
-V or --version: Displays the current version number.
-v or --verbose: Verbose mode on.
-t or --test: Do not delete or alter files.
-f or --force: Overwrite files even if they exist.
-k or --key file: Create random key and write it to a file.
-K or --key-stdout: Create random key and write it to stdout.
-e or --encrypt file: Encrypt user home using a key file.
-E or --encrypt-stdin: Encrypt user home using a key from stdin.
-d or --decrypt file: Decrypt user home using a key file.
-D or --decrypt-stdin: Decrypt user home using a key from stdin.
-p or --print: Display information (default).
-i or --infect: Make -p information appear on startup.
-u or --uninfect: Remove infect information.
```

Building:

```
make update
make release
```

Encrypted file format:

1. Extension is specified in "settings.h"
2. First 16 Bytes: AES initialization vector (AES CBC)
3. Rest of the file: AES256 encrypted contents

Todo list:

* Create --uninfect command
* Add some GUI utility
* Create wrapper that does everything
* Transfer key to the internet
* Create webserver
