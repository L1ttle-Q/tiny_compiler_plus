"""Compile and execute small programs, and verify compiler error exit codes."""
import argparse
import math
from pathlib import Path
import re
import subprocess
import tempfile


CASES = {
    "integer": ("int x := 6; write x ^ 3", [5]),
    "float": ("float x := 1.5; write x * 2", [3]),
    "condition": ("int x := 4; if (x >= 4) * (x <= 4) then write 1 else write 0 end", [1]),
    "undefined": ("write undeclared", None),
    "syntax": ("int x := ; write x", None),
}


def run_case(compiler, machine, case):
    repo = Path(__file__).resolve().parents[1]
    if case in CASES:
        source, expected = CASES[case]
    else:
        sample = {"triangle": "triangle", "legacy_sample": "sample", "legacy_calc": "calc"}[case]
        source = (repo / "TC" / (sample + ".tny")).read_text()
        expected = [] if case == "triangle" else None
    with tempfile.TemporaryDirectory(prefix="tiny-test-") as directory:
        work = Path(directory)
        (work / "program.tny").write_text(source + "\n", encoding="utf-8")
        compiled = subprocess.run([str(compiler), "program.tny"], cwd=work,
                                  stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=15)
        log = compiled.stdout.decode(errors="replace")
        if expected is None:
            if compiled.returncode == 0 or "Fail to Compile" not in log or (work / "program.tm").exists():
                raise RuntimeError("invalid source did not fail cleanly:\n" + log[-3000:])
            return
        if compiled.returncode or "Compile Successfully" not in log or not (work / "program.tm").is_file():
            raise RuntimeError("compilation failed:\n" + log[-3000:])
        executed = subprocess.run([str(machine), "program.tm"], cwd=work, input=b"g\nq\n",
                                  stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=15)
        log = executed.stdout.decode(errors="replace")
        numbers = [float(value) for value in re.findall(r"OUT instruction prints:\s*([-+\d.eE]+)", log)]
        if executed.returncode or "HALT:" not in log or len(numbers) != len(expected) or any(
                not math.isclose(a, b, rel_tol=1e-6, abs_tol=1e-6) for a, b in zip(numbers, expected)):
            raise RuntimeError("unexpected machine output:\n" + log[-3000:])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", required=True, type=Path)
    parser.add_argument("--machine", required=True, type=Path)
    choices = list(CASES) + ["triangle", "legacy_sample", "legacy_calc"]
    parser.add_argument("--case", choices=choices)
    args = parser.parse_args()
    compiler, machine = args.compiler.resolve(strict=True), args.machine.resolve(strict=True)
    failures = 0
    for case in [args.case] if args.case else choices:
        try:
            run_case(compiler, machine, case)
            print("PASS", case)
        except (OSError, RuntimeError, subprocess.TimeoutExpired) as error:
            failures += 1
            print("FAIL", case, error)
    return int(failures != 0)


if __name__ == "__main__":
    raise SystemExit(main())
