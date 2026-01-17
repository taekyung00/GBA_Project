from gitingest import ingest
import os

def split_content_by_files(content):
    """Split GitIngest output into individual file blocks."""
    separator = "=" * 48
    parts = content.split(separator)
    
    file_blocks = []
    i = 1
    while i < len(parts) - 1:
        header_part = parts[i]
        body_part = parts[i+1]
        if "FILE:" in header_part:
            full_block = f"{separator}{header_part}{separator}{body_part}"
            file_blocks.append(full_block)
            i += 2
        else:
            i += 1
    return file_blocks

def save_chunked_source(prefix, summary, tree, content, max_size_kb):
    """Save source code in chunks without cutting in the middle of a file."""
    file_blocks = split_content_by_files(content)
    common_header = f"{summary}\n\n{tree}\n\n"
    
    current_chunk_idx = 1
    current_content = ""
    max_bytes = max_size_kb * 1024

    print(f"   ℹ️  Total {len(file_blocks)} files packing into {max_size_kb}KB units.")

    for block in file_blocks:
        if current_content and (len(common_header) + len(current_content) + len(block) > max_bytes):
            filename = f"{prefix}-source-{current_chunk_idx}.txt"
            with open(filename, "w", encoding="utf-8") as f:
                f.write(common_header + current_content)
            print(f"      📄 {filename} created ({len(current_content)//1024} KB)")
            current_chunk_idx += 1
            current_content = ""
        
        current_content += block

    if current_content:
        filename = f"{prefix}-source-{current_chunk_idx}.txt"
        with open(filename, "w", encoding="utf-8") as f:
            f.write(common_header + current_content)
        print(f"      📄 {filename} created ({len(current_content)//1024} KB)")

def main():
    print("==========================================")
    print("   GBA Project: Smart Split Ingest v2.2")
    print("==========================================")
    
    prefix = input("👉 Enter project name (e.g., gba_sandbox): ").strip()
    if not prefix: return

    size_input = input("👉 Source split size (KB) (0 for no split): ").strip()
    max_size_kb = int(size_input) if size_input.isdigit() else 0

    # Common exclusion patterns
    common_exclude = [
        "archive/*", "build/*", ".git/*", 
        "*.gba", "*.elf", "*.sav", "*.o", "*.d"
    ]

    print(f"\n🚀 Analyzing '{prefix}' (Including sandbox/)...")

    try:
        # 1. Structure
        s, t, _ = ingest(".", exclude_patterns=common_exclude)
        with open(f"{prefix}-structure.txt", "w", encoding="utf-8") as f:
            f.write(s + "\n" + t)
        print(f"1️⃣  [Structure] Done")

        # 2. Settings (Included sandbox for potential config files)
        setting_patterns = [
            "makefile", "Makefile", "README.md", "*.json", 
            "*.cmake", "CMakeLists.txt", "*.md", "sandbox/*.md"
        ]
        s, t, c = ingest(".", include_patterns=setting_patterns, exclude_patterns=common_exclude)
        with open(f"{prefix}-setting.txt", "w", encoding="utf-8") as f:
            f.write(f"{s}\n\n{t}\n\n{c}")
        print(f"2️⃣  [Settings] Done")

        # 3. Source (Added sandbox/* to inclusion list)
        source_patterns = [
            "*.c", "*.h", "*.s", "*.asm", "*.cpp", "*.hpp",
            "source/*", "include/*", "sandbox/*"
        ]
        s, t, c = ingest(".", include_patterns=source_patterns, exclude_patterns=common_exclude)
        
        if max_size_kb > 0:
            save_chunked_source(prefix, s, t, c, max_size_kb)
        else:
            with open(f"{prefix}-source.txt", "w", encoding="utf-8") as f:
                f.write(f"{s}\n\n{t}\n\n{c}")
        print(f"3️⃣  [Source] Done")

    except Exception as e:
        print(f"❌ Error: {e}")

    print("\n✨ Process completed! Upload the generated files to Gemini.")

if __name__ == "__main__":
    main()