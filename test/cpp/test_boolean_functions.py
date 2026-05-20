import subprocess
import pathlib
import sys

def test_boolean_functions_executable():
    # The path to the binary compiled from test_boolean_functions.cpp
    bin_path = pathlib.Path(__file__).parent / "bin" / "test_boolean_functions.exe"
    
    if not bin_path.exists():
        print(f"Error: Executable not found at {bin_path}")
        print("Please compile the tests first.")
        sys.exit(1)
        
    print(f"Running C++ test suite at {bin_path}...")
    result = subprocess.run([str(bin_path)], capture_output=True, text=True)
    
    print("STDOUT:")
    print(result.stdout)
    
    if result.stderr:
        print("STDERR:")
        print(result.stderr)
        
    assert result.returncode == 0, f"C++ tests failed with return code {result.returncode}"
    print("All C++ tests passed successfully from Python!")

if __name__ == "__main__":
    test_boolean_functions_executable()
