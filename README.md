# WEBSERV42

WEBSERV42 is a custom HTTP/1.1 server developed as part of the 42 school curriculum. It is designed to handle multiple concurrent connections, serve static files, manage file uploads and deletions, and execute CGI scripts, all configurable via a custom configuration file.

## Features

*   **HTTP/1.1 Compliance:** Implements core functionalities of the HTTP/1.1 protocol.
*   **Configurable Servers:** Supports multiple virtual servers, each with distinct configurations (ports, hostnames, roots, error pages, etc.).
*   **Static File Serving:** Efficiently serves HTML, CSS, JavaScript, images, and other static content.
*   **Custom Error Pages:** Allows defining custom HTML pages for various HTTP error codes (e.g., 404 Not Found, 403 Forbidden, 405 Method Not Allowed, 413 Content Too Large).
*   **File Uploads:** Supports `POST` requests for uploading files to a specified directory, with configurable maximum body size.
*   **File Downloads:** Enables direct download of files via `GET` requests.
*   **File Deletion:** Handles `DELETE` requests, leveraging CGI for file system operations.
*   **CGI Execution:** Integrates Common Gateway Interface (CGI) for dynamic content generation and server-side scripting (e.g., Python scripts).
*   **Directory Listing (Autoindex):** Can automatically generate directory listings if enabled.
*   **Redirections:** Supports HTTP 301 (Moved Permanently) redirections.
*   **Event-Driven Architecture:** Utilizes `kqueue` for efficient, non-blocking I/O operations (primarily for BSD-based systems like macOS).

## Getting Started

### Prerequisites

*   A C++11 compatible compiler (e.g., Clang, GCC).
*   `make` for building the project.
*   An operating system that supports `kqueue` (e.g., macOS, FreeBSD).

### Building

To compile the server, navigate to the project's root directory and run `make`:

```bash
make
```

This will create an executable named `webserv`.

### Running the Server

The server requires a configuration file as an argument. An example configuration file might look like this (refer to the `Configuration` section for details):

```nginx
# config/default.conf
server {
    listen localhost:80;
    server_name mastermind_server;
    root serv1/;
    index homepage.html;
    error_page 404 /404.html;
    error_page 405 /405.html;
    error_page 413 /413.html;
    allow_methods GET POST DELETE;
    autoindex on;
    upload on;
    upload_path upload/;
    cgi_path cgi/;
    cgi_extension .py;
    client_max_body_size 1M;

    location /download {
        allow_methods GET;
        index download.html;
    }
    location /upload {
        allow_methods POST;
        index upload.html;
        upload on;
        upload_path upload/;
    }
    location /delete {
        allow_methods DELETE;
        index delete.html;
        cgi_path cgi/;
        cgi_extension .py;
    }
    location /redirection {
        redirect https://www.42.fr;
    }
}
```

To start the server with your configuration:

```bash
./webserv config/default.conf
```

The server will then listen on the ports specified in your configuration file.

## Configuration

The server's behavior is defined in a custom configuration file. The file is structured into `server` blocks, which can contain `location` blocks.

### Server Directives

These directives apply to the entire server block:

*   `listen <host>:<port>`: Specifies the IP address and port the server should listen on (e.g., `localhost:80`, `127.0.0.1:8080`). Multiple `listen` directives are allowed per server.
*   `server_name <name>`: Defines the virtual server name (e.g., `mastermind_server`).
*   `root <path>`: Sets the root directory for serving files (e.g., `serv1/`).
*   `index <filename>`: Specifies the default file to serve when a directory is requested (e.g., `homepage.html`).
*   `error_page <status_code> <path_to_html>`: Maps an HTTP status code to a custom error page (e.g., `error_page 404 /404.html`).
*   `allow_methods <METHOD1> <METHOD2> ...`: Defines allowed HTTP methods for the server (e.g., `allow_methods GET POST DELETE`).
*   `autoindex <on|off>`: Enables or disables automatic directory listing.
*   `upload <on|off>`: Enables or disables file uploads for the server.
*   `upload_path <path>`: Specifies the directory where uploaded files will be saved (e.g., `upload/`).
*   `cgi_path <path>`: Sets the base path for CGI scripts (e.g., `cgi/`).
*   `cgi_extension <.ext1> <.ext2> ...`: Defines file extensions that should be treated as CGI scripts (e.g., `cgi_extension .py`).
*   `client_max_body_size <size>`: Sets the maximum allowed size for client request bodies. Can be specified in bytes, kilobytes (K), or megabytes (M) (e.g., `1000000`, `1M`, `500K`).
*   `redirect <url>`: Configures a 301 redirection for the entire server.

### Location Directives

`location <path> { ... }` blocks allow specific configurations for different URL paths. Directives within a `location` block override the corresponding server-level directives for that path.

All server directives except `listen` and `server_name` can be used within a `location` block.

## Usage Examples

Once the server is running, you can interact with it using a web browser or tools like `curl`.

Assuming a server is configured to listen on `localhost:80` with `root serv1/` and `index homepage.html`:

*   **Access Homepage:**
    Open your browser to `http://localhost/`

*   **Download Files:**
    Navigate to `http://localhost/download`. You will find links to download various files (e.g., `cat.png`, `cat.jpg`, `cat.txt`, `cat.gif`).

*   **Upload Files:**
    Navigate to `http://localhost/upload`. Use the form to select a file and upload it. Uploaded files will be saved in the `upload_path` configured for the server (e.g., `upload/`).

*   **Delete Files:**
    Navigate to `http://localhost/delete`. Enter the name of a file (e.g., `test.txt` if it exists in the `upload/` directory) and submit the form. This will send a `DELETE` request handled by a CGI script.

*   **Check Error Pages:**
    Navigate to `http://localhost/errors`. This page provides links to trigger specific error pages like 404 (Page Not Found) and 405 (Method Not Allowed).

*   **Redirection:**
    Navigate to `http://localhost/redirection`. This will redirect your browser to `https://www.42.fr`.

## CGI Support

The server supports CGI execution for dynamic content. Currently, it is configured to handle `.py` files using the `cgi/delete.py` script as an example for file deletion.

The `cgi/delete.py` script takes two command-line arguments:
1.  The path to the file to be deleted.
2.  The path to an HTML error page to display if the file is not found.

## Error Pages

The server provides custom error pages for the following HTTP status codes:

*   `202 Accepted`
*   `204 No Content`
*   `403 Forbidden`
*   `404 Not Found`
*   `405 Method Not Allowed`
*   `413 Content Too Large`
*   `504 Gateway Time-out`

These pages are located in the `serv1/` and `serv2/` directories and can be customized via the `error_page` directive in the configuration.

## Credits

This server was made for the "Webserv" project by:

*   **Koyamba Maxime** - Mastermind Mkoyamba
*   **De Leeuw Basile** - Mastermind Bade-lee