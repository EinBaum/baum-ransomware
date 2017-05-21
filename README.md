```
baum-ransomware 1.0 - useful utility to encrypt your home folder Kappa

Usage example:
baum-ransomware 1.0 [(-h|--help)] [(-v|--verbose)] [(-V|--version)][(-e|--encrypt) keyfile] [(-d|--decrypt) keyfile][(-p|--print)] [(-i|--infect)]

Options:
-h or --help: Displays this information.
-V or --version: Displays the current version number.
-v or --verbose: Verbose mode on.
-t or --test: Do not delete or alter files.
-e or --encrypt keyfile: Encrypt user home and save key file.
-d or --decrypt keyfile: Decrypt user home using a key file.
-p or --print: Display payment information (default).
-i or --infect: Make payment information appear on startup.
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

* Check if files (key/data) exist before overriding them
* Create --uninfect command
* Don't encrypt the program itself
* Add some GUI utility
* Create wrapper that does everything
* Transfer key to the internet
* Create webserver
