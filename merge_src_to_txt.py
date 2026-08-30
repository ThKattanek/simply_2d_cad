import os

# Endungen, die berücksichtigt werden sollen
EXTENSIONS = {'.cpp', '.h', '.hpp', '.cxx', '.cc', '.ui'}
# Ordner, die ignoriert werden sollen
IGNORE_DIRS = {'build','build_w64', '.git', '.vscode', '.qtcreator', 'cmake-build-debug'}

OUTPUT_FILE = "gesammelter_code.txt"

def merge_cpp_files(root_dir):
    with open(OUTPUT_FILE, 'w', encoding='utf-8') as outfile:
        for root, dirs, files in os.walk(root_dir):
            # Ignorierte Ordner überspringen
            dirs[:] = [d for d in dirs if d not in IGNORE_DIRS]

            for file in sorted(files):
                if any(file.endswith(ext) for ext in EXTENSIONS):
                    file_path = os.path.join(root, file)
                    outfile.write(f"\n\n// {'='*50}\n")
                    outfile.write(f"// FILE: {file_path}\n")
                    outfile.write(f"// {'='*50}\n\n")

                    try:
                        with open(file_path, 'r', encoding='utf-8', errors='ignore') as infile:
                            outfile.write(infile.read())
                    except Exception as e:
                        print(f"Fehler beim Lesen von {file_path}: {e}")

    print(f"Fertig! Alle Quelldateien wurden in '{OUTPUT_FILE}' zusammengeführt.")

if __name__ == "__main__":
    merge_cpp_files("./src")
