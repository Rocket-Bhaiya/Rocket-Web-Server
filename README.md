# Rocket Web Server

Rocket Web Server is a lightweight HTTP server built using the [libmicrohttpd](https://www.gnu.org/software/libmicrohttpd/) library. It supports serving static HTML files and executing PHP scripts for dynamic content. The server can be customized with command-line arguments to specify the port and the file to serve.

## Features

- Serve static `.html` files
- Execute `.php` files via PHP
- Customize the server port with `--port <port>`
- Specify which file to serve at the root URL using `--file <file>`
- Help message to guide users with `--help`

## Prerequisites

- GCC compiler for compiling the C code
- [libmicrohttpd](https://www.gnu.org/software/libmicrohttpd/) library (for the HTTP server)
- PHP installed on the system for serving `.php` files

## Installation

1. Clone the repository or download the source code.

2. Install the necessary dependencies:

   - **For Ubuntu/Debian-based systems:**
     ```bash
     sudo apt update
     sudo apt install libmicrohttpd-dev php
     ```

3. Compile the server code:

   ```bash
   gcc -o Rocket-Web-Server Rocket-Web-Server.c -lmicrohttpd
   ./Rocket-Web-Server --help