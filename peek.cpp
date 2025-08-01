#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>

std::string help() {
	return
		"Help Section\n"
		"-------------------------\n"
		"1. Options\n"
		"  +-- -p, --path <path>\n"
		"  |    +-- Specifies the directory path to search. Default is the current directory ('.').\n"
		"  +-- -T, --threads\n"
		"  |    +-- Enables multi-threading for faster file searching.\n"
		"\n"
		"2. Search Filters\n"
		"  +-- -n, --name <name>\n"
		"  |    +-- Search for a file with the exact specified base name (without extension).\n"
		"  +-- -t, --text <text>\n"
		"  |    +-- Search for files containing the specified text string.\n"
		"  +-- --ext <extension>\n"
		"  |    +-- Search for files with the specified file extension (e.g., .cpp).\n"
		"  +-- -i, --ignore-case\n"
		"  |    +-- Makes the search case-insensitive for name, text, and extension.\n"
		"\n"
		"3. General\n"
		"  +-- -h, --help\n"
		"  |    +-- Displays this help section.\n"
		"-------------------------\n";
}

std::string to_lower(std::string str) {
	std::string out = str;
	std::transform(out.begin(), out.end(), out.begin(), ::tolower);
	return out;
}

int main(int argc, char* argv[]) {

	auto start_time = std::chrono::high_resolution_clock::now();

	std::string path = ".";
	std::string name;
	std::string text;
	std::string extension;
	int thread_count = 1;

	bool arg_name = false;
	bool arg_text = false;
	bool arg_ext = false;
	bool arg_i = false;

	if (argc == 1) {
		std::cout << help() << '\n';
		return 0;
	}
	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];
		if (arg == "-h" || arg == "--help") {
			std::cout << help() << '\n';
			return 0;
		}

		else if (arg == "-n" || arg == "--name") {
			if (i + 1 < argc) {
				arg_name = true;
				name = argv[++i];
			}
			else {
				std::cerr << "Missing value for -n/--name\n";
				return 1;
			}
		}

		else if (arg == "-p" || arg == "--path") {
			if (i + 1 < argc) {
				path = argv[++i];
			}
			else {
				std::cerr << "Missing value for -p/--path\n";
				return 1;
			}
		}

		else if (arg == "-t" || arg == "--text") {
			if (i + 1 < argc) {
				arg_text = true;
				text = argv[++i];
			}
			else {
				std::cerr << "Missing value for -t/--text\n";
				return 1;
			}
		}

		else if (arg == "--ext") {
			if (i + 1 < argc) {
				arg_ext = true;
				extension = argv[++i];
			}
			else {
				std::cerr << "Missing value for --ext\n";
				return 1;
			}
		}

		else if (arg == "-T" || arg == "--threads") {
			if (i + 1 < argc && argv[i + 1][0] != '-') {
				std::string thread_str = argv[++i];

				for (char c : thread_str) {
					if (!isdigit(c)) {
						std::cout << "Thread must be a number\n";
						return 1;
					}
				}
				thread_count = std::stoi(thread_str);
				if (thread_count < 1 || thread_count > 5) {
					std::cout << "Thread count must be between 1 and 5\n";
					return 1;
				}
			}
			else {
				std::cerr << "Missing value for -T/--threads\n";
				return 1;
			}
		}

		else if (arg == "-i" || arg == "--ignore-case") {
			arg_i = true;
		}

		else {
			std::cerr << "Unknown argument: " << arg << '\n';
			return 1;
		}
	}

	if (!arg_name && !arg_text) {
		std::cerr << "You must provide at least one of (--name/-n) or (--text/-t)\n";
		return 1;
	}

	if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
		std::cerr << "The specified path does not exist or is not a directory.\n";
		return 1;
	}
	std::vector<std::filesystem::path> file_paths;
	for (auto entry : std::filesystem::recursive_directory_iterator(path)) {
		if (entry.is_regular_file()) {
			file_paths.push_back(entry.path());
		}
	}

	if (file_paths.empty()) {
		std::cerr << "No files found in the specified path.\n";
		return 1;
	}

	bool found = false;
	std::mutex mtx;
	std::vector<std::thread> threads;
	int chunk_size = (file_paths.size() + thread_count - 1) / thread_count;
	if (arg_i) {
		if (arg_name) name = to_lower(name);
		if (arg_text) text = to_lower(text);
		if (arg_ext) {
			if (!extension.empty() && extension[0] != '.')
				extension = "." + extension;
			extension = to_lower(extension);
		}
	}
	if (arg_ext && !extension.empty() && extension[0] != '.') {
		extension = "." + extension;
	}
	for (int i = 0; i < thread_count; ++i) {
		int start = i * chunk_size;
		int end = std::min(start + chunk_size, static_cast<int>(file_paths.size()));

		threads.emplace_back([&, start, end]() {
			for (int j = start; j < end; ++j) {
				auto entry = file_paths[j];

				std::string filename = entry.stem().string();
				std::string file_extension = entry.extension().string();
				std::string check_filename = filename;
				std::string check_extension = file_extension;

				if (arg_i) {
					check_filename = to_lower(filename);
					check_extension = to_lower(file_extension);
				}

				bool name_matches = (!arg_name || (arg_i ? check_filename == name : filename == name));
				bool ext_matches = (!arg_ext || (arg_i ? check_extension == extension : file_extension == extension));
				bool text_matches = false;

				std::string line;
				if (arg_text) {
					std::ifstream file(entry);
					while (std::getline(file, line)) {
						std::string check_line = arg_i ? to_lower(line) : line;
						if (check_line.find(text) != std::string::npos) {
							text_matches = true;
							break;
						}
					}
				}

				if ((arg_name && arg_text && text_matches && name_matches && ext_matches) ||
					(arg_name && !arg_text && name_matches && ext_matches) ||
					(!arg_name && arg_text && text_matches && ext_matches)) {
					std::lock_guard<std::mutex> lock(mtx);
					std::cout << "Found: " << entry << '\n';
					found = true;
				}
			}
			});
	}

	for (auto& t : threads) {
		t.join();
	}

	if (!found) {
		std::cerr << "No matching files found.\n";
		return 1;
	}
	
	auto end_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> duration = end_time - start_time;
	std::cout << "Execution Time: " << duration.count() << " seconds\n";

	return 0;
}

