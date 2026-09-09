#!/usr/bin/env fish

argparse 'h/help' 'p/port=' 'd/directory=' -- $argv
or return

if set -ql _flag_help
    echo "Usage: serve.fish [-p|--port <port>] [-d|--directory <dir>]"
    return 0
end

set -l port 8000
if set -ql _flag_port
    set port $_flag_port
end

set -l docs_dir $_flag_directory

if test -z "$docs_dir" -o ! -d "$docs_dir"
    if test -d "docs/html"
        set docs_dir "docs/html"
    else if test -d "$BUILD_WORKING_DIRECTORY/bazel-bin/docs/html"
        set docs_dir "$BUILD_WORKING_DIRECTORY/bazel-bin/docs/html"
    end
end

if not test -d "$docs_dir"
    echo "Error: Documentation directory not found: $docs_dir" >&2
    return 1
end

echo "Serving documentation at http://localhost:$port (Press Ctrl+C to stop)..."
python3 -m http.server $port --directory "$docs_dir"
