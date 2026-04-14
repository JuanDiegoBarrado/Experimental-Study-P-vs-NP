import subprocess, pathlib

BIN = pathlib.Path(__file__).parent / "bin/test_binaries.exe"

def test_parser_ok():
    result = subprocess.run([BIN], capture_output=True)
    assert result.returncode == 0, result.stderr.decode()