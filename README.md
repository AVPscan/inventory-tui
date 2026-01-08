# Products Secure Reporter (2026)

Lightweight C tool for managing a product database and sending secure encrypted reports via SMTP.

## Project Structure
- `products.c`: Main project logic and entry point.
- `LibOpt.c / .h`: Core library for encryption, HTML generation, and secure mailing.
- `products.txt`: Database containing product list.
- `send.txt`: Encrypted credentials file (Email & SMTP password).

## How it works
1. Place your credentials in `send.txt` as: `user@domain.com password ` (space at the end).
2. Run the application. The system will:
   - Encrypt `send.txt` into a HEX-string (hiding your password).
   - Generate `report.html` from `report.txt`.
   - Securely send the report via `curl` using encrypted credentials.

## Compilation
```bash
gcc products.c LibOpt.c -o products

