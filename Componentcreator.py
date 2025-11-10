import os
import sys


def create_component(component_name):
    component_name = component_name.strip()
    folder_name = component_name
    header_guard = component_name.upper() + "_H"

    # Paths
    base_dir = os.getcwd()  # assuming script runs from Zephyr main dir
    src_dir = os.path.join(base_dir, "src")
    component_dir = os.path.join(src_dir, folder_name)

    # Create directory
    os.makedirs(component_dir, exist_ok=True)
    print(f"✅ Created directory: {component_dir}")

    # Create .c file
    c_file_path = os.path.join(component_dir, f"{component_name}.c")
    with open(c_file_path, "w") as c_file:
        c_file.write(f'#include "{component_name}.h"\n')
    print(f"✅ Created: {c_file_path}")

    # Create .h file
    h_file_path = os.path.join(component_dir, f"{component_name}.h")
    with open(h_file_path, "w") as h_file:
        h_file.write(
            f"""#ifndef {header_guard}
#define {header_guard}



#endif // {header_guard}
"""
        )
    print(f"✅ Created: {h_file_path}")

    # Path to CMakeLists.txt
    cmake_path = os.path.join(base_dir, "CMakeLists.txt")
    if not os.path.exists(cmake_path):
        print("❌ CMakeLists.txt not found in main directory.")
        return

    # Read and modify CMakeLists.txt
    with open(cmake_path, "r") as f:
        cmake_content = f.read()

    # Add new .c file to target_sources
    new_source_line = f"  src/{folder_name}/{component_name}.c\n"
    if new_source_line not in cmake_content:
        cmake_content = cmake_content.replace(
            "src/NonBlockdelay/NonBlockdelay.c",
            f"src/NonBlockdelay/NonBlockdelay.c\n  {new_source_line.strip()}"
        )
        print(f"✅ Added {component_name}.c to target_sources")

    # Add include directory
    new_include_line = f"  ${{CMAKE_CURRENT_SOURCE_DIR}}/src/{folder_name}\n"
    if new_include_line not in cmake_content:
        cmake_content = cmake_content.replace(
            "${CMAKE_CURRENT_SOURCE_DIR}/src/NonBlockdelay",
            f"${{CMAKE_CURRENT_SOURCE_DIR}}/src/NonBlockdelay\n  {new_include_line.strip()}"
        )
        print(f"✅ Added {component_name} include directory")

    # Write updated CMakeLists.txt
    with open(cmake_path, "w") as f:
        f.write(cmake_content)

    print("🎉 Component setup complete!")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python create_component.py <ComponentName>")
    else:
        create_component(sys.argv[1])
