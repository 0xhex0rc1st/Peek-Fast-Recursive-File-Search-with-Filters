# Peek: Fast, Recursive File Search with Filters

**Peek** is a fast, multi-threaded file search tool designed to help you quickly find files and content across large directories. Whether you're a developer or just dealing with a lot of files, Peek offers powerful search options to find exactly what you need in no time.

## 🚀 Features

* **Super-fast searches** using multi-threading.
* **Recursive directory search** to explore subdirectories.
* **Search by file name**, extension, or content.
* **Case-insensitive search** option for easier matching.
* **Customizable thread count** to speed up searches.
* **Simple to use** with a clean, easy-to-understand interface.

## 🔧 Installation

1. **Clone the repository** to your local machine:

   ```bash
   git clone https://github.com/yourusername/peek.git
   ```

2. **Build the project** using your preferred build system (e.g., CMake).

## 🧾 Usage

### Run Peek with the following command:

```bash
./peek [OPTIONS]
```

### Main Options:

* **`-p <path>`** or **`--path <path>`**
  Specifies the directory path to search. Default is the current directory (`.`).

* **`-n <name>`** or **`--name <name>`**
  Search for files with a specific base name (without the file extension).

* **`--ext <extension>`**
  Filter by file extension (e.g., `.cpp`, `.txt`).

* **`-t <text>`** or **`--text <text>`**
  Search for files containing a specific text string inside them.

* **`-i`** or **`--ignore-case`**
  Makes the search case-insensitive.

* **`-T <threads>`** or **`--threads <threads>`**
  Use multiple threads to speed up the search (1-5 threads recommended).

* **`-h`** or **`--help`**
  Display the help message with all available options.

## 🔍 Examples

* **Search for a file by name:**

  ```bash
  ./peek --name myfile
  ```

* **Search for a specific text inside files:**

  ```bash
  ./peek --text "TODO"
  ```

* **Search using multiple threads:**

  ```bash
  ./peek --threads 4 --path /my/project --name config
  ```

* **See all available options:**

  ```bash
  ./peek --help
  ```

## 💡 Tips

* **Want faster results?** Try using more threads (`-T <number>`) to speed up searches in large directories.
* **Looking for a specific file?** Use the `--name` option to find a file based on its base name.

## 📄 License

MIT License - see the LICENSE file for more information.
