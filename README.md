```
baumcrypt 1.0 - useful utility to encrypt your home folder Kappa

Options:
-h or --help: Displays this information.
-V or --version: Displays the current version number.
-v or --verbose: Verbose mode on.
-t or --test: Do not delete or alter files.
-f or --force: Overwrite files even if they exist.
-k or --key: Create random key and write it to a file.
-e or --encrypt keyfile: Encrypt user home using a key file.
-d or --decrypt keyfile: Decrypt user home using a key file.
-p or --print: Display information (default).
-i or --infect: Make -p information appear on startup.
-u or --uninfect: Remove infect information.
```

Building:

```
make update
make
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
