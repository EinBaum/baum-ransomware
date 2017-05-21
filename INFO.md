Encrypted file format:

1. Extension is specified in "settings.h"
2. First 16 Bytes: AES initialization vector
3. Rest of the file: AES256 encrypted contents
